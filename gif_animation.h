#ifndef ANIMATION_H
#define ANIMATION_H

#include <gtk/gtk.h>

struct _ILiekCats { // Handled by Gdk
        GObject catnip;
        void *argument; // for gif_data struct
        GdkPixbufAnimation *animation;
        GdkPixbufAnimationIter *iterator;
        guint timeout;
	gboolean playing;
};

// Declare our custom type of paintable pixbuf
G_DECLARE_FINAL_TYPE(ILiekCats,pixbuf_paintable,PIXBUF,PAINTABLE,GObject)

// This macro is annoying but necessary. Returns GType, which is an integer, however it is not static so we can't hardcode it.
#define GTK_MAKES_ME_RAGE (pixbuf_paintable_get_type())

struct gif_data { // Entire gif file from 0 to EOF. Essentially the same as GBytes but without abstraction.
	const unsigned char *data;
	size_t data_len;
};

#endif
