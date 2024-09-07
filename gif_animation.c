
#include "gif_animation.h"
/*
XXX XXX XXX BEWARE: The contents of this file *may* be constrained by the LGPLv2 license, therefore it is compiled seperately.
	For analysis, see https://github.com/GNOME/gtk/blob/main/demos/gtk-demo/pixbufpaintable.c

TODO The GIF _animation_ code here is extremely inefficient and CPU intensive.
It has been better implemented by EOS( https://gitlab.gnome.org/GNOME/eog/-/blob/master/src/eog-image.c ) and ristretto, etc
However their code is GPL'd so I haven't spent time reviewing how theirs is more efficient, or how much more efficient it is.
In fact, any efficiency differences could be due to gtk3 vs gtk4

Note: For retrieving sizes (in UI) ( Useful for aspect ratio )
	gdk_pixbuf_animation_get_width(PIXBUF_PAINTABLE(paintable)->animation)
	gdk_pixbuf_animation_get_height(PIXBUF_PAINTABLE(paintable)->animation)

*/

enum { // Currently we only have one property (ie the data itself), but we can specify more
	GIF_DATA = 1
};

static inline void snapshot(GdkPaintable *paintable,GdkSnapshot *snapshot,double width,double height)
{ // Get a frame  // Runs frequently, so do not bloat.
	ILiekCats *pixbuf_paintable = PIXBUF_PAINTABLE(paintable);
	GdkPixbuf *pixbuf;
	if(pixbuf_paintable->playing == FALSE)
		pixbuf = gdk_pixbuf_animation_get_static_image(pixbuf_paintable->animation);
	else
	{
		pixbuf = gdk_pixbuf_animation_iter_get_pixbuf(pixbuf_paintable->iterator);
		gdk_pixbuf_animation_iter_advance(pixbuf_paintable->iterator, NULL);
	}
	GdkTexture *texture = gdk_texture_new_for_pixbuf(pixbuf); // TODO this is probably our bottleneck?
	gdk_paintable_snapshot(GDK_PAINTABLE(texture),snapshot,width,height); // TODO this is probably our bottleneck?
	g_object_unref(texture);
}

static inline void get_frame(GdkPaintableInterface *iface)
{ // Runs frequently, so do not bloat. NOTE: Argument *must* be named iface or errors occur on Windows builds.
	iface->snapshot = snapshot;
}

G_DEFINE_TYPE_WITH_CODE(ILiekCats,pixbuf_paintable,G_TYPE_OBJECT,G_IMPLEMENT_INTERFACE(GDK_TYPE_PAINTABLE,get_frame))

static inline void pixbuf_paintable_init(ILiekCats *paintable)
{ // Could get / set the size here, if desired?
	(void)paintable;
}

static inline gboolean next_frame(gpointer data)
{ // Runs frequently, so do not bloat.
	ILiekCats *paintable = data;
	guint delay = (guint)gdk_pixbuf_animation_iter_get_delay_time(paintable->iterator); // consider calling this once in pixbuf_paintable_class_init or pixbuf_paintable_init. WARNING: calling once will break variable rate gif animations, which exist.
	paintable->timeout = g_timeout_add(delay,next_frame,paintable);
	gdk_paintable_invalidate_contents(GDK_PAINTABLE(paintable));
	return G_SOURCE_REMOVE;
}

static inline void set_property(GObject *object,const guint prop_id,const GValue *value,GParamSpec *pspec)
{ // Set our data as a fake stream
	(void) prop_id; // could be used if we have more than one property
	(void) pspec;
	ILiekCats *paintable = PIXBUF_PAINTABLE(object);

	const struct gif_data *gif_data = (const struct gif_data*)(const void *) g_value_get_string(value);
	guint delay = 0;
	g_clear_object(&paintable->animation);

	GInputStream *stream = g_memory_input_stream_new_from_data(gif_data->data,(gssize)gif_data->data_len,NULL);
	paintable->animation = gdk_pixbuf_animation_new_from_stream(stream,NULL,NULL);

	if(!GDK_IS_PIXBUF_ANIMATION(paintable->animation))
		return; // 2024/05 A gif is bunk/corrupt. Don't remove this test.

	g_clear_object(&paintable->iterator);
	if(!gdk_pixbuf_animation_is_static_image(paintable->animation))
	{
		paintable->iterator = gdk_pixbuf_animation_get_iter(paintable->animation,NULL);
		paintable->timeout = g_timeout_add(delay,next_frame,paintable);
		paintable->playing = TRUE;
	}
	else
		paintable->playing = FALSE;
	gdk_paintable_invalidate_contents(GDK_PAINTABLE (paintable));
}

static inline void get_property(GObject *object,const guint prop_id,GValue *value,GParamSpec *pspec)
{ // we don't use it but it can't be NULL
	(void) object;
	(void) prop_id;
	(void) value;
	(void) pspec;
}

static inline void dispose(GObject *object)
{
	ILiekCats *paintable = PIXBUF_PAINTABLE(object);
	g_clear_pointer(&paintable->argument,g_free);
	g_clear_object(&paintable->animation);
	g_clear_object(&paintable->iterator);
	if(paintable->timeout)
	{
		g_source_remove(paintable->timeout);
		paintable->timeout = 0;
	}
	G_OBJECT_CLASS(pixbuf_paintable_parent_class)->dispose(object);
}

static inline void pixbuf_paintable_class_init(ILiekCatsClass *paintable_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(paintable_class);
	object_class->dispose = dispose;
	object_class->get_property = get_property;
	object_class->set_property = set_property;
	g_object_class_install_property(object_class,GIF_DATA,g_param_spec_string("gif-data",NULL,NULL,NULL,G_PARAM_READWRITE));
}
