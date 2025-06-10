/*
TorX: Metadata-safe Tor Chat Library
Copyright (C) 2024 TorX

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License version 3 as published by the Free
Software Foundation.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.

Appendix:

Section 7 Exceptions:

1) Modified versions of the material and resulting works must be clearly titled
in the following manner: "Unofficial TorX by Financier", where the word
Financier is replaced by the financier of the modifications. Where there is no
financier, the word Financier shall be replaced by the organization or
individual who is primarily responsible for causing the modifications. Example:
"Unofficial TorX by The United States Department of Defense". This amended
full-title must replace the word "TorX" in all source code files and all
resulting works. Where utilizing spaces is not possible, underscores may be
utilized. Example: "Unofficial_TorX_by_The_United_States_Department_of_Defense".
The title must not be replaced by an acronym or short title in any form of
distribution.

2) Modified versions of the material and resulting works must be distributed
with alternate logos and imagery that is substantially different from the
original TorX logo and imagery, especially the 7-headed snake logo. Modified
material and resulting works, where distributed with a logo or imagery, should
choose and distribute a logo or imagery that reflects the Financier,
organization, or individual primarily responsible for causing modifications and
must not cause any user to note similarities with any of the original TorX
imagery. Example: Modifications or works financed by The United States
Department of Defense should choose a logo and imagery similar to existing logos
and imagery utilized by The United States Department of Defense.

3) Those who modify, distribute, or finance the modification or distribution of
modified versions of the material or resulting works, shall not avail themselves
of any disclaimers of liability, such as those laid out by the original TorX
author in sections 15 and 16 of the License.

4) Those who modify, distribute, or finance the modification or distribution of
modified versions of the material or resulting works, shall jointly and
severally indemnify the original TorX author against any claims of damages
incurred and any costs arising from litigation related to any changes they are
have made, caused to be made, or financed. 

5) The original author of TorX may issue explicit exemptions from some or all of
the above requirements (1-4), but such exemptions should be interpreted in the
narrowest possible scope and to only grant limited rights within the narrowest
possible scope to those who explicitly receive the exemption and not those who
receive the material or resulting works from the exemptee.

6) The original author of TorX grants no exceptions from trademark protection in
any form.

7) Each aspect of these exemptions are to be considered independent and
severable if found in contradiction with the License or applicable law.
*/
#include <stdio.h>
#include <unistd.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

/* Things to note:
	Be sure to call gst_init(NULL, NULL);
	Metadata https://gstreamer.freedesktop.org/documentation/application-development/advanced/metadata.html?gi-language=c
*/

struct rec_info {
	GstElement *pipeline;
	unsigned char *buffer;
	time_t start_time;
	time_t start_nstime;
	void (*callback)(void*,const unsigned char*,const size_t);
	void *callback_arg;
};

struct play_info {
	GstElement *pipeline;
	unsigned char *data;
	size_t data_len;
	uint8_t loop; // play on repeat
	int n; // for cache play ONLY, NOT for voice messages
	gboolean (*callback)(GstBus *bus, GstMessage *msg, gpointer arg);
//	gpointer *callback_arg; // play_info is passed
};

struct rec_info current_recording = {0};
struct play_info current_play_pausable = {0}; // This is for 'pausable' audio (such as voice messages), not streaming.
struct play_info current_play_ringtone = {0}; // for ringtone (sending / receiving calls)
struct play_info current_play_beep = {0}; // for beeps

void record_start(struct rec_info *rec_info,const int sample_rate,void (*callback)(void*,const unsigned char*,const size_t),void *callback_arg);
unsigned char *record_stop(size_t *data_len,uint32_t *duration,struct rec_info *rec_info);
void playback_start(struct play_info *play_info);
void playback_stop(struct play_info *play_info); // must be double so function can null it
/*static void
print_one_tag (const GstTagList * list, const gchar * tag, gpointer user_data)
{
	(void)user_data;
  guint i;
  guint num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i < num; ++i) {
    // Note: when looking for specific tags, use the gst_tag_list_get_xyz() API, we only use the GValue approach here because it is more generic
    GValue *val = gst_tag_list_get_value_index (list, tag, i);
    if (G_VALUE_HOLDS_STRING (val)) {
      g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
    } else if (G_VALUE_HOLDS_UINT (val)) {
      g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
    } else if (G_VALUE_HOLDS_DOUBLE (val)) {
      g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
    } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
      g_print ("\t%20s : %s\n", tag,
          (g_value_get_boolean (val)) ? "true" : "false");
    } else if (GST_VALUE_HOLDS_BUFFER (val)) {
      GstBuffer *buf = gst_value_get_buffer (val);
      gsize buffer_size = gst_buffer_get_size (buf);

      g_print ("\t%20s : buffer of size %lu\n", tag, buffer_size);
    } else if (GST_VALUE_HOLDS_DATE_TIME (val)) {
      GstDateTime *dt = g_value_get_boxed (val);
      gchar *dt_str = gst_date_time_to_iso8601_string (dt);

      g_print ("\t%20s : %s\n", tag, dt_str);
      g_free (dt_str);
    } else {
      g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
  }
}*/

static inline void on_pad_added(GstElement *src, GstPad *new_pad, GstElement *sink)
{ // Callback function to link the decoder dynamically
	(void)src;
	GstPad *sink_pad = gst_element_get_static_pad(sink, "sink");
	if(!gst_pad_is_linked(sink_pad))
	{
		if(gst_pad_link(new_pad, sink_pad) != GST_PAD_LINK_OK)
			error_simple(0, "Failed to link decoder to converter.");
	}
	gst_object_unref(sink_pad);
}

/* static void print_duration(GstElement *pipe)
{
	gint64 duration = -1;
	//if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ASYNC_DONE) {
	if (gst_element_query_duration(pipe, GST_FORMAT_TIME, &duration))
	{
		printf("Checkpoint duration is: %ld\n",duration);
		g_print("Duration: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(duration));
	}
	else
		g_print("Could not query the duration.\n");
	//}
} */

void playback_stop(struct play_info *play_info)
{ // If calling to stop a stream, remember to t_peer[n].t_cache_info.playing = 0;
	if(!play_info || !play_info->pipeline)
	{ // Could be caused by play_info->pipeline = NULL in playback_async?
		error_simple(0,"Nothing to stop yet playback_stop was called. Possible coding error. Report to UI Devs.");
		return; // Bad args or already stopped
	}
	gst_element_set_state(play_info->pipeline, GST_STATE_NULL);
	gst_object_unref(play_info->pipeline);
	play_info->pipeline = NULL;
	torx_free((void*)&play_info->data);
}

void playback_start(struct play_info *play_info)
{ // Play any type of audio file, from memory, syncronously (this function WILL BLOCK until playing completes)
//	write_bytes("fishing.aac",play_info->data,play_info->data_len);
//	print_metadata("fishing.aac");
	#define local_cleanup if(play_info->pipeline) { gst_object_unref(play_info->pipeline); play_info->pipeline = NULL; }
	if(!play_info || !play_info->data || !play_info->data_len || !play_info->callback)
	{
		error_simple(0, "Playback failed sanity check");
		return;
	}
	GstElement *appsrc = gst_element_factory_make("appsrc", "audio-source");
	GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
	GstElement *convert = gst_element_factory_make("audioconvert", "converter");
	GstElement *sink = gst_element_factory_make("autoaudiosink", "audio-output");
	if(!play_info->pipeline)
		play_info->pipeline = gst_pipeline_new("audio-playback");
	if(!play_info->pipeline || !appsrc || !decoder || !convert || !sink)
	{
		local_cleanup
		error_simple(0, "Failed to create GStreamer elements.");
		return;
	}
	gst_bin_add_many(GST_BIN(play_info->pipeline), appsrc, decoder, convert, sink, NULL);
	if(!gst_element_link(appsrc, decoder)) // TODO gst_element_link_pads_full: assertion 'GST_IS_ELEMENT (src)' failed
	{
		local_cleanup
		error_simple(0, "Failed to link appsrc to decoder.");
		return;
	}
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), convert);
	if(!gst_element_link(convert, sink))
	{
		local_cleanup
		error_simple(0, "Failed to link converter to sink.");
		return;
	}
//	g_object_set(G_OBJECT(appsrc),"stream-type", GST_APP_STREAM_TYPE_STREAM,"format", GST_FORMAT_TIME,"is-live", FALSE,NULL); // XXX TRUE here means no buffering. Must include #include <gst/app/gstappsrc.h>
	GstBuffer *buffer = gst_buffer_new_allocate(NULL, play_info->data_len, NULL);
	gst_buffer_fill(buffer, 0, play_info->data, play_info->data_len);
	GstFlowReturn ret;
	g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
	gst_buffer_unref(buffer);
	if(ret != GST_FLOW_OK)
	{
		local_cleanup
		error_simple(0, "Failed to push buffer to appsrc.");
		return;
	}
	g_signal_emit_by_name(appsrc, "end-of-stream", &ret); // TODO ??? necessary??
	if(gst_element_set_state(play_info->pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
	{
		local_cleanup
		error_simple(0, "Unable to set the pipeline to the playing state.");
		return;
	}
	GstBus *bus = gst_element_get_bus(play_info->pipeline);
	gst_bus_add_signal_watch(bus);
	g_signal_connect(bus, "message", G_CALLBACK(play_info->callback), play_info); // TODO move this before gst_element_set_state ??
/*	print_duration(pipeline); */
	// Cleanup
	gst_object_unref(bus);
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
		if(rec_info->callback) // Streaming audio, handle immediately
			rec_info->callback(rec_info->callback_arg,map.data, map.size);
		else
		{ // Audio message, keep buffering
			const uint32_t buffer_len = torx_allocation_len(rec_info->buffer);
			if(rec_info->buffer)
				rec_info->buffer = torx_realloc(rec_info->buffer,buffer_len + map.size);
			else
				rec_info->buffer = torx_secure_malloc(map.size);
			memcpy(&rec_info->buffer[buffer_len], map.data, map.size);
		}
		gst_buffer_unmap(buffer, &map);
	}
	gst_sample_unref(sample);
	return GST_FLOW_OK;
}

void record_start(struct rec_info *rec_info,const int sample_rate,void (*callback)(void*,const unsigned char*,const size_t),void *callback_arg)
{
	if(!rec_info || sample_rate < 1 || rec_info->pipeline)
	{ // Note that we require rec_info->pipeline to be NULL
		error_simple(0,"Sanity check failed in record_start. Perhaps there is an existing recording?");
		return;
	}
	rec_info->pipeline = gst_pipeline_new("audio_stream");
	GstElement *audio_source = gst_element_factory_make("autoaudiosrc", "audio_source");
	GstElement *sink = gst_element_factory_make("appsink", "audio_sink");
	GstElement *encoder = gst_element_factory_make("avenc_aac", "aac_enc");
	GstElement *aacparse = gst_element_factory_make("aacparse", "aac_parse");
	GstElement *aac_caps = gst_element_factory_make("capsfilter", "aac_caps_filter");
	GstElement *audioconvert = gst_element_factory_make("audioconvert", "audio_convert");
	GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps_filter");
	GstElement *queue = gst_element_factory_make("queue", "queue");
	if(!rec_info->pipeline || !audio_source || !encoder || !sink || !aacparse || !aac_caps || !audioconvert || !capsfilter || !queue)
	{
		error_simple(0,"Audio: Element could not be obtained, or sanity check otherwise failed.");
		return;
	}
	rec_info->buffer = NULL;
	rec_info->start_time = 0; // must initialize
	rec_info->start_nstime = 0; // must initialize
	rec_info->callback = callback;
	rec_info->callback_arg = callback_arg;
	// Configure appsink
	g_object_set(G_OBJECT(sink), "emit-signals", TRUE, "sync", FALSE, NULL);
	g_signal_connect(sink, "new-sample", G_CALLBACK(new_sample), rec_info);
	// Add a bus watch
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(rec_info->pipeline));
	gst_object_unref(bus);
	// Build the pipeline
	gst_bin_add_many(GST_BIN(rec_info->pipeline), audio_source, encoder, sink, NULL);
	GstCaps *caps = gst_caps_new_simple("audio/mpeg","mpegversion", G_TYPE_INT, 4,"stream-format",G_TYPE_STRING,"adts","rate", G_TYPE_INT, sample_rate,"channels", G_TYPE_INT, 1,NULL);
	g_object_set(G_OBJECT(aac_caps), "caps", caps, NULL);
	gst_caps_unref(caps);
	gst_bin_add_many(GST_BIN(rec_info->pipeline), audio_source, capsfilter, audioconvert, encoder, aacparse, aac_caps, queue,sink, NULL);
	if(gst_element_link_many(audio_source, capsfilter, audioconvert, encoder, aacparse, aac_caps, queue,sink, NULL) != TRUE)
	{
		error_simple(0,"Linked elements failed. Coding error. Report this.");
		gst_object_unref(rec_info->pipeline);
		rec_info->pipeline = NULL;
		return;
	}
	// Start playing
	set_time(&rec_info->start_time,&rec_info->start_nstime);
	if(gst_element_set_state(rec_info->pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
	{
		error_simple(0,"Unable to set the pipeline to the playing state. Coding error. Report this.");
		gst_object_unref(rec_info->pipeline);
		rec_info->pipeline = NULL;
		return;
	}
}

unsigned char *record_stop(size_t *data_len,uint32_t *duration,struct rec_info *rec_info)
{ // XXX WARNING: Frees rec_info
	if(!rec_info || !rec_info->pipeline)
	{
		error_simple(0,"Bad arguments passed to record_stop. Coding error. Report this.");
		if(data_len)
			*data_len = 0;
		if(duration)
			*duration = 0;
		return NULL;
	}
	if(duration)
	{
		time_t end_time = 0;
		time_t end_nstime = 0;
		set_time(&end_time,&end_nstime);
		const double diff = (double)(end_time - rec_info->start_time) * 1e9 + (double)(end_nstime - rec_info->start_nstime);
		*duration = (uint32_t)(diff / 1e6); // convert nanoseconds to milliseconds
	}
	gst_element_send_event(rec_info->pipeline, gst_event_new_eos());
	gst_element_set_state(rec_info->pipeline, GST_STATE_NULL);
/*	print_duration(rec_info->pipeline); */
	gst_object_unref(GST_OBJECT(rec_info->pipeline));
	rec_info->pipeline = NULL;
	if(data_len)
		*data_len = torx_allocation_len(rec_info->buffer);
	return rec_info->buffer;
}
