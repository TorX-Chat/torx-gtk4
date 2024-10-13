#include <stdio.h>
#include <unistd.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

/* Things to note:
	Be sure to call gst_init(NULL, NULL);
*/

struct rec_info {
	GstElement *pipeline;
	unsigned char *buffer;
	size_t buffer_len;
};

struct rec_info *record_start(const int sample_rate);
unsigned char *record_stop(size_t *data_len,struct rec_info *rec_info);
void playback(const unsigned char *data, const size_t data_len);

static inline void handle_gst_message_and_free(GstMessage *msg)
{ // Do not modify. This works well.
	if(msg != NULL)
	{
		if(GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR)
		{
			gchar *debug_info;
			GError *error;
			gst_message_parse_error(msg, &error, &debug_info);
			g_free(debug_info);
			error_printf(0,"GstMessage: %s",error->message);
			g_error_free(error);
		}
		gst_message_unref(msg);
	}
}

static inline void on_pad_added(GstElement *src, GstPad *new_pad, GstElement *sink)
{ // Callback function to link the decoder dynamically
	(void)src;
	GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");
	if(gst_pad_is_linked(sink_pad))
	{
		gst_object_unref(sink_pad);
		return;
	}
	GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
	if(ret != GST_PAD_LINK_OK)
		error_simple(0, "Failed to link decoder to converter.");
	gst_object_unref(sink_pad);
}

void playback(const unsigned char *data, const size_t data_len)
{ // Play any type of audio file, from memory, syncronously (this function WILL BLOCK until playing completes)
	if(!data || !data_len)
	{
		error_simple(0, "Playback was passed no data.");
		return;
	}
	GstElement *pipeline = gst_pipeline_new("audio-playback");
	GstElement *appsrc = gst_element_factory_make("appsrc", "audio-source");
	GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
	GstElement *convert = gst_element_factory_make("audioconvert", "converter");
	GstElement *sink = gst_element_factory_make("autoaudiosink", "audio-output");
	if(!pipeline || !appsrc || !decoder || !convert || !sink)
	{
		error_simple(0, "Failed to create GStreamer elements.");
		if(pipeline) gst_object_unref(pipeline);
			return;
	}
	gst_bin_add_many(GST_BIN(pipeline), appsrc, decoder, convert, sink, NULL);
	if(!gst_element_link(appsrc, decoder))
	{
		error_simple(0, "Failed to link appsrc to decoder.");
		gst_object_unref(pipeline);
		return;
	}
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), convert);
	if(!gst_element_link(convert, sink))
	{
		error_simple(0, "Failed to link converter to sink.");
		gst_object_unref(pipeline);
		return;
	}
//	g_object_set(G_OBJECT(appsrc),"stream-type", GST_APP_STREAM_TYPE_STREAM,"format", GST_FORMAT_TIME,"is-live", FALSE,NULL); // XXX TRUE here means no buffering. Must include #include <gst/app/gstappsrc.h>

	GstBuffer *buffer = gst_buffer_new_allocate(NULL, data_len, NULL);
	gst_buffer_fill(buffer, 0, data, data_len);
	GstFlowReturn ret;
	g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
	gst_buffer_unref(buffer);
	if(ret != GST_FLOW_OK)
	{
		error_simple(0, "Failed to push buffer to appsrc.");
		gst_object_unref(pipeline);
		return;
	}
	g_signal_emit_by_name(appsrc, "end-of-stream", &ret);
	// Start playing the pipeline
	if(gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
	{
		error_simple(0, "Unable to set the pipeline to the playing state.");
		gst_object_unref(pipeline);
		return;
	}
	// Wait for error or end of stream
	GstBus *bus = gst_element_get_bus(pipeline);
	GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	handle_gst_message_and_free(msg);
	// Cleanup
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}

static inline GstFlowReturn new_sample(GstElement *sink, gpointer data)
{
	struct rec_info *rec_info = (struct rec_info*)data;
	GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
	if(!sample)
		return GST_FLOW_ERROR;
	GstBuffer *buffer = gst_sample_get_buffer(sample);
	GstMapInfo map;
	if(gst_buffer_map(buffer, &map, GST_MAP_READ))
	{ // Add new data to the current global buffer
		rec_info->buffer = torx_realloc(rec_info->buffer,rec_info->buffer_len + map.size);
		memcpy(&rec_info->buffer[rec_info->buffer_len], map.data, map.size);
		rec_info->buffer_len += map.size;
		gst_buffer_unmap(buffer, &map);
	}
	gst_sample_unref(sample);
	return GST_FLOW_OK;
}

struct rec_info *record_start(const int sample_rate)
{
	GstElement *pipeline = gst_pipeline_new("audio_stream");
	GstElement *audio_source = gst_element_factory_make("autoaudiosrc", "audio_source");
	GstElement *sink = gst_element_factory_make("appsink", "audio_sink");
	GstElement *encoder = gst_element_factory_make("avenc_aac", "aac_enc");
	GstElement *aacparse = gst_element_factory_make("aacparse", "aac_parse");
	GstElement *aac_caps = gst_element_factory_make("capsfilter", "aac_caps_filter");
	GstElement *audioconvert = gst_element_factory_make("audioconvert", "audio_convert");
	GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps_filter");
	GstElement *queue = gst_element_factory_make("queue", "queue");
	if(!pipeline || !audio_source || !encoder || !sink || !aacparse || !aac_caps || !audioconvert || !capsfilter || !queue)
	{
		error_simple(0,"Audio: Element could not be obtained");
		return NULL;
	}
	struct rec_info *rec_info = torx_insecure_malloc(sizeof(struct rec_info));
	rec_info->pipeline = pipeline;
	rec_info->buffer = NULL;
	rec_info->buffer_len = 0;
	// Configure appsink
	g_object_set(G_OBJECT(sink), "emit-signals", TRUE, "sync", FALSE, NULL);
	g_signal_connect(sink, "new-sample", G_CALLBACK(new_sample), rec_info);
	// Add a bus watch
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_object_unref(bus);
	// Build the pipeline
	gst_bin_add_many(GST_BIN(pipeline), audio_source, encoder, sink, NULL);
	GstCaps *caps = gst_caps_new_simple("audio/mpeg","mpegversion", G_TYPE_INT, 4,"stream-format",G_TYPE_STRING,"adts","rate", G_TYPE_INT, sample_rate,"channels", G_TYPE_INT, 1,NULL);
	g_object_set(G_OBJECT(aac_caps), "caps", caps, NULL);
	gst_caps_unref(caps);
	gst_bin_add_many(GST_BIN(pipeline), audio_source, capsfilter, audioconvert, encoder, aacparse, aac_caps, queue,sink, NULL);
	if(gst_element_link_many(audio_source, capsfilter, audioconvert, encoder, aacparse, aac_caps, queue,sink, NULL) != TRUE)
	{
		g_printerr("Linked elements failed.\n");
		gst_object_unref(pipeline);
		return NULL;
	}
	// Start playing
	if(gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
	{
		error_simple(0,"Unable to set the pipeline to the playing state.");
		return NULL;
	}
	return rec_info;
}

unsigned char *record_stop(size_t *data_len,struct rec_info *rec_info)
{
	if(!data_len || !rec_info || !rec_info->pipeline)
	{
		error_simple(0,"Bad arguments passed to record_stop. Coding error. Report this.");
		if(data_len)
			*data_len = 0;
		return NULL;
	}
	gst_element_send_event(rec_info->pipeline, gst_event_new_eos());
	gst_element_set_state(rec_info->pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(rec_info->pipeline));
	*data_len = rec_info->buffer_len;
	return rec_info->buffer;
}
