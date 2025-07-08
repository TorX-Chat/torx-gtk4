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
/*

DEPENDS: libevent-core,libevent-pthreads(libevent-pthreads-2.1-7),libgtk-4-1,libsodium TODO libsqlcipher0,libssl3,libsqlcipher-dev
## To update GResources, if required:		glib-compile-resources --generate-source torx-gtk4.gresource.xml		# this is now part of cmake

## For static, use:
 /usr/lib/x86_64-linux-gnu/libevent_pthreads.a /usr/lib/x86_64-linux-gnu/libevent_core.a /usr/lib/x86_64-linux-gnu/libsodium.a
## For dynamic, use:
 -levent_pthreads -levent_core -lsodium 

## For -Wall -Wextra
pkill tor ; rm /home/user/Code/git/chat/torx-gtk4 ; clear ; gcc -std=gnu11 -Wall -Wextra -O2 /home/user/Code/git/chat/main_gtk4.c /usr/lib/x86_64-linux-gnu/libevent_pthreads.a /usr/lib/x86_64-linux-gnu/libevent_core.a /usr/lib/x86_64-linux-gnu/libsodium.a -lm `pkg-config --cflags --libs gtk4` -o /home/user/Code/git/chat/torx-gtk4 && /home/user/Code/git/chat/torx-gtk4

## For Segfaults (use "bt" after crash for backtrace)
pkill tor ; rm /home/user/Code/git/chat/torx-gtk4 ; clear ; gcc -std=gnu11 -Wall -Wextra -g -o /home/user/Code/git/chat/torx-gtk4 /home/user/Code/git/chat/main_gtk4.c /usr/lib/x86_64-linux-gnu/libevent_pthreads.a /usr/lib/x86_64-linux-gnu/libevent_core.a /usr/lib/x86_64-linux-gnu/libsodium.a -lm `pkg-config --cflags --libs gtk4` -export-dynamic && gdb /home/user/Code/git/chat/torx-gtk4 

## For memory leaks
pkill tor ; rm /home/user/Code/git/chat/torx-gtk4  ; clear ; gcc -std=gnu11 -Wall -Wextra -g /home/user/Code/git/chat/main_gtk4.c /usr/lib/x86_64-linux-gnu/libevent_pthreads.a /usr/lib/x86_64-linux-gnu/libevent_core.a /usr/lib/x86_64-linux-gnu/libsodium.a -lm `pkg-config --cflags --libs gtk4` -o /home/user/Code/git/chat/torx-gtk4 && valgrind --track-origins=yes --leak-check=full /home/user/Code/git/chat/torx-gtk4

## For CMAKE build (dynamic):
clear && rm -rf build/_deps/torx-* && proxychains4 -f /etc/proxychains/tor.conf cmake -B build && cd build && proxychains4 -f /etc/proxychains/tor.conf make && cd ..  && G_MESSAGES_DEBUG=all ./build/torx-gtk4
clear && rm -rf build/_deps/torx-* && proxychains4 -f /etc/proxychains/tor.conf cmake -DCMAKE_BUILD_TYPE=Debug -B build && cd build && proxychains4 -f /etc/proxychains/tor.conf make && cd ..  && export G_DEBUG=fatal-criticals && gdb -ex "set print thread-events off" -ex run ./build/torx-gtk4
clear && rm -rf build/_deps/torx-* && proxychains4 -f /etc/proxychains/tor.conf cmake -DCMAKE_BUILD_TYPE=Debug -B build && cd build && proxychains4 -f /etc/proxychains/tor.conf make && cd ..  && valgrind --track-origins=yes --leak-check=full ./build/torx-gtk4

## send to mill-lan
rsync --checksum /home/user/Code/git/chat/torx-gtk4 mill-lan:/usr/local/bin/torx-gtk4
rsync --checksum /home/user/Code/git/chat/org.gnome.torx-gtk4.desktop  mill-lan:/usr/share/applications/org.gnome.TorX.desktop
rsync --checksum /home/user/Code/git/chat/other/256x256/apps/icon_square.png mill-lan:/usr/share/icons/hicolor/48x48/apps/org.gnome.TorX.png

XXX NOTES XXX
* to gtk_widget_destroy in GTK4, gtk_widget_set_visible(widget,FALSE); g_object_unref(widget);
* idle && UI thread tasks do not need g_idle_add (UI thread)
* _cb tasks MUST make ui_ calls by g_idle_add (non-UI thread)

XXX To Do Before First Release XXX
* have toggle option for response to kill code
* have a "delete log" function (killcode && killcode global should also do this; revoking is as simple as deleting the logs
* have a big notification if tor is not running (from "Tor Control Port not running on" error message)
* be able to include GTK4 library in the compiled binary (currently it is not, it uses the shared libgtk-4.so.1
* Mobile: 720x1280 1080x1920.... for testing: 480x854
* SECURITY: memzero sensitive things, G object free or whatever, free anything we forgot to free
* remove all remaining sizes from .css files (NOTE: fonts might be REQUIRED to be CSS???)

XXX DESIGN XXX
* change all UI thread function names to be prefixed with ui_, as a reminder to g_idle_add(). NOTE: static ones only, excluding _cb and _idle.

XXX ERRORS XXX

* Need to get/run "GTK inspector"
* GTK_IS_WIDGET is causing invalid reads everywhere we use it. It is not suitable for what we use it for. Adding a null check does not help or aid in any way.
* gtk_label_set_max_width_chars() in print_message_cb is ignored completely
* XXX PANGO wrapping is causing the issue with 3 second message loading. Apparently the height of our lines is not stable due to a GTK or PANGO error. See #GTK
* No panel icon functioning
* Gnotification can't use gresource icon
* printing a message that ends on space results in GTK error. not sure why. the space appears. spaces and newlines are counted by strlen().
* we make 0 efforts to free any resources in GTK.
* 2022/11 there are memory leaks when we pass data to G_CALLBACK via malloc. Passing it with (void*)(intptr) 1, like we did before, would work fine and prevent memory leaks. However this method can ONLY be used for passing data to functions that will only exist temporarily and be rebuilt (which is most). At this time, this is an dust sized issue and it should be ignored because mishandling such a shift will cause big problems.

*/

#include <locale.h> 	// Note: libraries probably shouldn't be writing files. Better for clients to handle writing files, so that library can be OS independant.
			// library can still pass / be passed the contents of the file though, to encrypt/decrypt
#include <gtk/gtk.h>
#include <torx.h> 		// TorX core
#include "torx-gtk4.c"		// gresource
#include "gif_animation.c" // XXX XXX XXX BEWARE: The contents of this file *may* be constrained by the LGPLv2 license, therefore it is compiled seperately
#include "audio.c"
//#include "other/scalable/apps/logo_torx.h" // XXX Fun alternative to GResource (its a .svg in b64 defined as a macro). but TODO DO NOT USE IT, use g_resources_lookup_data instead to get gbytes

#define ALPHA_VERSION 1 // enables debug print to stderr
#define CLIENT_VERSION "TorX-GTK4 Alpha 2.0.34 2025/07/02 by TorX\n© Copyright 2025 TorX.\n"
#define DBUS_TITLE "org.torx.gtk4" // GTK Hardcoded Icon location: /usr/share/icons/hicolor/48x48/apps/org.gnome.TorX.png
#define DARK_THEME 0
#define LIGHT_THEME 1
#define THEME_DEFAULT DARK_THEME
#define RESOURCE_CSS_DARK "/org/torx/gtk4/theme/dark_chat.css"
#define RESOURCE_CSS_LIGHT "/org/torx/gtk4/theme/light_chat.css"
#define MESSAGES_START_AT_TOP_OF_WINDOW 1 // 0 seems a bit buggy
#define INVERSION_TEST 1 // https://gitlab.gnome.org/GNOME/gtk/-/issues/4680 NOTE: using g_list_store_insert / g_list_store_splice at position 0 will cause a rebuild of the entire list
#define GTK_FACTORY_BUG 1 // Memory intensive. Enabling this is kind of necessary until https://gitlab.gnome.org/GNOME/gtk/-/merge_requests/7420 is widely merged.
static GtkWidget *popover_level_one = NULL; // XXX For working-around GTK bug on double level popovers
#define QR_IS_PNG 1
/* XXX NOTE: Bundled files are the two below, the .desktop, and /usr/share/icons/hicolor/48x48/apps/org.gnome.TorX.png XXX */
#define FILENAME_ICON "icon.svg" // .png/.svg
//#define FILENAME_BEEP "beep.wav" // .wav files only, on linux and windows
#define MAX_STICKER_COLUMNS 5
#define MAX_STICKER_ROWS 6
#define ENABLE_SPINNERS 0 // this should only be enabled if people have a GPU.
#define LONG_PRESS_TIME 0.5 // 0.5 to 2.0 seconds, default is 1.0.
#define MINIMUM_AUDIO_MESSAGE_LENGTH_IN_MILLISECONDS 500
#define REALISTIC_MINIMUM_BINARY_SIZE 15360 // minimum realistic binary size... 10kb (this is smaller than hello-world... this is to prevent confusion between binaries and folders). Warning: SVG files and soft links can be less than this.

#define ICON_COMMUNICATION_ATTEMPTS 10 // currently must be 4+ on a ThinkPad X200. Setting to 10 in anticipation of slower systems.
#define ENABLE_APPINDICATOR 1 // DO NOT USE TO VERIFY FUNCTIONALITY

static uint8_t appindicator_functioning = 0; // DO NOT default to 1. This will be set upon successful connection.
/*#ifdef WIN32
	#define ENABLE_APPINDICATOR 0 // TODO no, lets let it ride. docs say windows is at least somewhat supported.
#else
	#define ENABLE_APPINDICATOR 1
#endif*/

/* Global Variables */ // NOTE: Access must be in UI thread (_idle) or all usage must have mutex/rwlock
static char language[5+1] = {0};
static char starting_dir[PATH_MAX/2] = {0};
static char *argv_0 = NULL;
static char *binary_path = NULL; // current binary's full path
static uint8_t running = 0;
static uint8_t show_keyboard = 1;
static int global_n = -1; // Always CTRL or GROUP_CTRL. Currently open chat window. Avoid using where possible. (except notifications perhaps)
static int treeview_n = -1;
static int global_theme = -1;
static int log_unread = 1;
static int vertical_mode = 0; // see ui_determine_orientation()
static int8_t force_sign = 0; // TODO Should be 0. Global value for testing only. This should be per-peer. can add timestamp to message (as a salt to prevent relay attacks in groups), Tor project does this with all signed messages.
static uint8_t save_all_stickers = 0; // Do not default to 1 for legal reasons
static uint8_t send_sticker_data = 1; // not really that useful because if we don't send stickers, people can't request stickers.
static uint8_t close_to_tray = 0; // works well but currently not offered as UI option, in favor of minimize_to_tray
static uint8_t minimize_to_tray = 1;
static uint8_t display_images = 1; // TODO should be clickable (to show larger), should automatically render after 100%. Gifs are a problem if corrupt (because we don't have sufficient sanity checks). Other corrupt images are fine.

static int size_window_default_width = 480; // 1100 Desktop, TODO mobile should just maximize?
static int size_window_default_height = 854; // 600 Desktop, mobile should just maximize?
static uint8_t start_maximized = 0;
static uint8_t start_minimized = 0;
static uint8_t start_daemonized = 0;
static uint8_t no_password = 0; // UI setting that is only relevant during first_start

static pid_t tray_pid = -1;
static size_t totalUnreadPeer = 0;
static size_t totalUnreadGroup = 0;
static size_t totalIncoming = 0;
static size_t last_totalUnreadPeer = 0; // for checking for changes, in icon_communicator_idle. DO NOT USE FOR OTHER THINGS.
static size_t last_totalUnreadGroup = 0; // for checking for changes in icon_communicator_idle. DO NOT USE FOR OTHER THINGS.
static size_t last_totalIncoming = 0; // for checking for changes in icon_communicator_idle. DO NOT USE FOR OTHER THINGS.
static int global_group = -1; // TODO dumb but currently fine. add_chat_node only takes one arg (n) and we need two for ui_group_invite
static int generated_n = -1; // -1 is invalid, dont let anything get done with -val
static int last_played_n = -1; // for playback_message, to allow pausing or changing of playing audio message
static int last_played_i = INT_MIN; // for playback_message, to allow pausing or changing of playing audio message

/* Global Structs */ // NOTE: Access must be in UI thread (_idle) or all usage must have mutex/rwlock

static pthread_t thread_icon_communicator = {0};

const char *supported_image_formats[] = {".jpg",".jpeg",".png",".gif",".bmp",".svg"}; // case insensitive

#define MAX_PEERS 4096 // TODO this isnt ideal because library has no such limitation. this is just laziness.
#define MAX_STICKER_REQUESTS 15 // Concurrent number, per peer. keep it low for RAM reasons and to avoid spamming people. this is just laziness.
#define STICKERS_LIST_SIZE 500
#define ARBITRARY_ARRAY_SIZE 512

#define ENUM_STATUS_GROUP_CTRL 4

static struct sticker_list {
	GdkPaintable *paintable_animated; // Check before printing
	GdkPaintable *paintable_static; // DO NOT FREE upon delete UNTIL shutdown. // TODO display except when mouseover, in ui_sticker_chooser
	unsigned char checksum[CHECKSUM_BIN_LEN];
	int peers[MAX_PEERS]; // must initialize as -1. This is a list of peers we sent this sticker to. Only these peers can request data, to prevent fingerprinting by sticker list.
	unsigned char *data; // Only exists for unsaved stickers
	size_t data_len; // Only exists for unsaved stickers
	GtkWidget *outer_box; // this is for sticker popover, which holds the green square
} sticker [STICKERS_LIST_SIZE] = {0};

static struct t_peer_list { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	size_t unread; // number of new unread messages (currently since startup only, otherwise this needs to be in peer not t_peer
	int8_t mute; // 0 no, 1 yes
	int pm_n;
	int edit_n;
	int edit_i;
	int pointer_location;
	GtkTextBuffer* buffer_write;
	struct t_cache_info {
		unsigned char **audio_cache;
		time_t *audio_time;
		time_t *audio_nstime;
		time_t last_played_time;
		time_t last_played_nstime;
		GstElement *stream_pipeline;
		uint8_t playing; // can't use stream_pipeline because it is repeatedly created and null'd every ~20-50ms of playback
	} t_cache_info; // MUST BE AN ALLOCATED POINTER which is NEVER free'd or realloc'd because we *cannot* have this moving if we realloc the t_peer or t_cache_info struct since t_cache_info sees multi-threaded usage
	struct t_message_list { // XXX DO NOT DELETE XXX
		int pos;
		uint8_t visible;
		uint8_t unheard;
		#if GTK_FACTORY_BUG
		GtkWidget *message_box;
		#endif
	} *t_message; // TODO do not initialize automatically, but upon use
	struct t_file_list {
		GtkWidget *progress_bar;
		GtkWidget *file_size;
		int n; // The peer iter that progress_bar is in, for rebuilds upon completion
		int i; // The message iter that progress_bar is in, for rebuilds upon completion
		uint8_t previously_completed; // We need to track this so we can limit complete notifications to once in group file transfers.
	} *t_file;
	struct t_call_list {
		uint8_t joined; // Whether we accepted/joined it
		uint8_t waiting; // Whether it is awaiting an acceptance/join or has been declined/ignored
		uint8_t mic_on;
		uint8_t speaker_on;
		uint8_t speaker_phone;
		uint8_t audio_only;
		time_t start_time;
		time_t start_nstime;
		int participating[256]; // Maximum 256 concurrent peers for simplicity TODO
		uint8_t participant_mic[256]; // Maximum 256 concurrent peers for simplicity TODO
		uint8_t participant_speaker[256]; // Maximum 256 concurrent peers for simplicity TODO
		GtkWidget *column; // Vertical box, contains who is calling and then a row of applicable widgets related to the call
	} t_call[25]; // Maximum 25 concurrent calls for simplicity TODO
	unsigned char stickers_requested[MAX_STICKER_REQUESTS][CHECKSUM_BIN_LEN];
} *t_peer; // TODO do not initialize automatically, but upon use

#define create_button(text,callback,vp)\
{\
	GtkWidget *button = gtk_button_new();\
	gtk_button_set_label(GTK_BUTTON(button),text);\
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(callback), vp);\
	gtk_box_append(GTK_BOX(box),button);\
}

/*
#define create_button_for_popover(text,callback,vp)\
{\
	GtkWidget *button = gtk_button_new();\
	gtk_button_set_label(GTK_BUTTON(button),text);\
	g_signal_connect(button, "clicked", G_CALLBACK(callback), vp);\
	gtk_box_append(GTK_BOX(box),button);\
} // Calback function format: static void function(GtkWidget *button,const void *arg)

static GtkWidget *ui_popover_destroy(GtkWidget *widget)
{ // Discovers parent popover and pops it down // does not work on multi-level popovers, so not using
	GtkWidget *parent = gtk_widget_get_parent(widget);
	while(parent && !GTK_IS_POPOVER(parent) && !GTK_IS_WINDOW(parent))
		parent = gtk_widget_get_parent(parent);
	if(GTK_IS_POPOVER(parent))
		gtk_popover_popdown(GTK_POPOVER(parent));
	return parent;
}
*/

static struct { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX // Putting these global values in a struct lets us easily initialize as NULL
	GListStore *treestore_incoming;
	GListStore *treestore_outgoing;
	GListStore *treestore_mult;
	GListStore *treestore_sing;

	GListStore *list_store_chat;

	int global_pos_max;
	int global_pos_min;
	int window;	// see enum windows{}
//	GdkDisplay *display;

	GtkWidget *paned; // widget with two panes

	GtkWidget *main_window;
	GtkWidget *headerbar;
	GtkWidget *headerbar_buttons_box_leave;
	GtkWidget *headerbar_buttons_box_enter;
	GtkCssProvider *provider;

	GtkWidget *auth_error;
	GtkWidget *auth_entry_pass; // GtkPasswordEntry
	GtkWidget *auth_button;
	GtkWidget *switch_censored_region;

	/* All pages */
	GtkWidget *panel_left;
	GtkWidget *search_panel;
	GtkWidget *entry_search;
	GtkWidget *add;
	GtkWidget *home;
	GtkWidget *settings;
	GtkWidget *scrolled_window_peer;
	GtkWidget *scrolled_window_group;
	GtkWidget *scrolled_window_block;
	GtkWidget *panel_right;
	GtkWidget *scroll_box_right;
	GtkWidget *scrolled_window_right;
	GtkWidget *chat_headerbar;
	GtkWidget *chat_headerbar_left;
	GtkWidget *chat_headerbar_right;
	GtkWidget *image_header;
	GtkWidget *custom_switcher_box;
	GtkWidget *panel_left_stack;

	/* Individual Peer Chat Pages */
	GtkWidget *last_online;
	GtkWidget *call_box;
	GtkWidget *button_activity; // editing, private messaging, etc
	GtkWidget *write_message; // TODO can probably be eliminated with some work
	GtkWidget *button_record;
	GtkWidget *button_keyboard_microphone;
	GtkWidget *button_emoji;
	GtkWidget *button_sticker;
	GtkWidget *button_attach;
	GtkWidget *button_send;
	GtkWidget *popover_message; // needs to be global to avoid being made permanently parentless when children of outer message box get deleted in print_message(,,3)
	GtkWidget *popover_sticker;

	/* Individual Group Chat Pages */
	GtkWidget *button_participants;
	GtkWidget *popover_invite; // TODO can probably be eliminated with lots of work
	GtkWidget *popover_group_peerlist;
	GtkWidget *popover_participant_list;
	GtkWidget *popover_more;

	/* Home Page */
	GtkWidget *treeview_incoming; // TODO can probably be eliminated with lots of work
	GtkWidget *treeview_outgoing; // TODO can probably be eliminated with lots of work
	GtkWidget *treeview_mult; // TODO can probably be eliminated with lots of work
	GtkWidget *treeview_sing; // TODO can probably be eliminated with lots of work
	GtkWidget *button_accept;
	GtkWidget *button_reject;
	GtkWidget *button_copy;
	GtkWidget *button_copy_generated_qr;
	GtkWidget *button_save_generated_qr;
	GtkWidget *button_show_qr;
	GtkWidget *button_delete;
	GtkWidget *popover_qr;
	const char *stack_current;

	/* From generate page, should be global because people might want to generate in the background */
	GtkWidget *entry_add_peernick;
	GtkWidget *entry_add_peeronion;
	GtkWidget *entry_generate_peernick;
	GtkTextBuffer *textbuffer_generated_onion;
	GtkWidget *generated_qr_onion;

	/* From generate group page, only global for convenience */
	GtkWidget *entry_add_group_identifier;
	GtkWidget *entry_add_group_id;
	GtkWidget *entry_generate_group;
	GtkTextBuffer *textbuffer_generated_group;
	GtkWidget *generated_qr_group;

	/* From settings page, custom input */
	GtkWidget *popover_torrc_errors; // TODO ? could be eliminated but would require being the only child of button
	GtkWidget *entry_custom_identifier;
	GtkWidget *entry_custom_privkey;
	GtkTextBuffer *textbuffer_custom_onion;
	char *custom_input_location;

	/* From Change Password Page */
	GtkWidget *button_change_password_continue;
	GtkWidget *button_change_password;
	GtkWidget *pass_old;
	GtkWidget *pass_new;
	GtkWidget *pass_verify;

	/* From Log Pages */
	GtkTextBuffer *tor_log_buffer;
	GtkTextBuffer *torx_log_buffer;

	/* From Torrc Editing Page */
	GtkTextBuffer *textbuffer_torrc;
} t_main = {0}; // TODO Eliminate as many of these as possible. Having widgets exist in global scope is #1 cause of memory leaks and errors.

enum sizes { /* TODO Remove all remaining sizing from .css files TODO */
	size_zero = 0, // must be first or set to 0. Enum sets values sequentially from ero, unless otherwise set
	size_natural = -1,
	size_horizontal_mode_minimum_width = 955,
	size_peerlist_minimum_width = 200,
	size_peerlist_minimum_height = 100,
	size_peerlist_button_height = 54,
	size_peerlist_icon_size = 27,
	size_logo_top = 47,
	size_logo_auth = 47,
	size_image_left_headerbar = 35,
	size_icon_top_left = 22, // +,home,settings -- beside search bar
	size_file_icon = 45,
	size_sticker_small = 90,
	size_sticker_large = 150,
	size_icon_top_right = 45,
	size_icon_bottom_right = 45,
	size_icon_send_width = 70,
	size_box_qr_code = 289,
	size_textbuffer_single_line_height = 30,

	size_button_auth_width = 240,
	size_button_auth_height = 50,
	size_entry_auth = 310,

	size_spacing_zero = 0,
	size_spacing_three = 3,
	size_spacing_five = 5,
	size_spacing_eight = 8,
	size_spacing_ten = 10,
	size_margin_fifteen = 15,
	size_margin_ten = 10,
	size_margin_eight = 8,

	size_message_spacing = 10
};

enum windows { // for t_main.window = none
	none = 0,
	window_auth = 1,
	window_main = 2,// add,generate
	window_home = 3, // (friend requests, outgoing requests, active sing, active mult), 
	window_settings = 4,
	window_peer = 5, 
	window_change_password = 6, 
	window_global_killcode = 7, 
	window_edit_torrc = 8,
	window_log_tor = 9,
	window_log_torx = 10,
	window_group_generate = 11
};

enum buttons { // for ui_button_generate
	ENUM_BUTTON_LOGGING,
	ENUM_BUTTON_MUTE,
	ENUM_BUTTON_CALL,
	ENUM_BUTTON_ADD_BLOCK_MENU,
	ENUM_BUTTON_ADD_BLOCK,
	ENUM_BUTTON_KILL,
	ENUM_BUTTON_DELETE_CTRL,
	ENUM_BUTTON_DELETE_LOG,
	ENUM_BUTTON_KEYBOARD_MICROPHONE,
	ENUM_BUTTON_EMOJI,
	ENUM_BUTTON_STICKER,
	ENUM_BUTTON_ATTACH,
	ENUM_BUTTON_ADD_STICKER
};

enum spin { // for spin_change
	ENUM_SPIN_DEBUG,
	ENUM_SPIN_CPU_THREADS,
	ENUM_SPIN_SUFFIX_LENGTH,
	ENUM_SPIN_LOG_DAYS,
	ENUM_SPIN_SING_EXPIRATION,
	ENUM_SPIN_MULT_EXPIRATION,
	ENUM_SPIN_TOR_PORT_SOCKS,
	ENUM_SPIN_TOR_PORT_CONTROL
};

enum ui_protocols {
	ENUM_PROTOCOL_STICKER_HASH = 29812,		// Sticker will be sending a sticker hash. If peer doesn't have the sticker, request.
	ENUM_PROTOCOL_STICKER_HASH_PRIVATE = 40505,
	ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED = 1891,
	ENUM_PROTOCOL_STICKER_REQUEST = 24931,		// hash
	ENUM_PROTOCOL_STICKER_DATA_GIF = 46093,		// hash + data
	// TODO vibration... but do it only in flutter as a UI specific protocol
	ENUM_PROTOCOL_AAC_AUDIO_MSG = 43474, // uint32_t duration (milliseconds, big endian) + data
	ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE = 29304, // uint32_t duration (milliseconds, big endian) + data
	ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED = 47904, // uint32_t duration (milliseconds, big endian) + data
	ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN = 65303, // Start Time + nstime // This is also offer
	ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE = 33037, // Start Time + nstime // This is also offer
	ENUM_PROTOCOL_AAC_AUDIO_STREAM_PEERS = 16343, // Start Time + nstime + 56*participating
	ENUM_PROTOCOL_AAC_AUDIO_STREAM_LEAVE = 23602, // Start Time + nstime
	ENUM_PROTOCOL_AAC_AUDIO_STREAM_DATA_DATE = 54254 // Start Time + nstime + data + date
};

void initialize_n_cb_ui(const int n);
void initialize_i_cb_ui(const int n,const int i);
void initialize_f_cb_ui(const int n,const int f);
void initialize_g_cb_ui(const int g);
void shrinkage_cb_ui(const int n,const int shrinkage);
void expand_file_struc_cb_ui(const int n,const int f);
void expand_message_struc_cb_ui(const int n,const int i);
void expand_peer_struc_cb_ui(const int n);
void expand_group_struc_cb_ui(const int g);
void transfer_progress_cb_ui(const int n,const int f,const uint64_t transferred);
void change_password_cb_ui(const int value);
void incoming_friend_request_cb_ui(const int n);
void onion_deleted_cb_ui(const uint8_t owner,const int n);
void peer_online_cb_ui(const int n);
void peer_offline_cb_ui(const int n);
void peer_new_cb_ui(const int n);
void peer_loaded_cb_ui(const int n);
void onion_ready_cb_ui(const int n);
void cleanup_cb_ui(const int sig_num);
void tor_log_cb_ui(char *message);
void error_cb_ui(char *error_message);
void fatal_cb_ui(char *error_message);
void custom_setting_cb_ui(const int n,char *setting_name,char *setting_value,const size_t setting_value_len,const int plaintext);
void message_new_cb_ui(const int n,const int i);
void message_modified_cb_ui(const int n,const int i);
void message_deleted_cb_ui(const int n,const int i);
void stream_cb_ui(const int n,const int p_iter,char *data,const uint32_t data_len);
void message_extra_cb_ui(const int n,const int i,unsigned char *data,const uint32_t data_len);
void message_more_cb_ui(const int loaded,int *loaded_array_n,int *loaded_array_i);
void login_cb_ui(const int value);
void ui_show_auth_screen(void);
void ui_show_missing_binaries(void);
void ring_start(void);
void ring_stop(void);
void playback_async(struct play_info *play_info,const uint8_t loop,const unsigned char *data, const size_t data_len);
void cache_play(const int n);

/* Global Text Declarations for ui_initialize_language() */
static const char *text_title = {0};
static const char *text_welcome = {0};
static const char *text_transfer_completed = {0};
static const char *text_online = {0};
static const char *text_new_friend = {0};
static const char *text_accepted_file = {0};
static const char *text_spoiled = {0};
static const char *text_placeholder_privkey = {0};
static const char *text_placeholder_identifier = {0};
static const char *text_placeholder_add_identifier = {0};
static const char *text_placeholder_add_onion = {0};
static const char *text_placeholder_add_group_identifier = {0};
static const char *text_placeholder_add_group_id = {0};
static const char *text_placeholder_search = {0};
static const char *text_dark = {0};
static const char *text_light = {0};
static const char *text_minimize_to_tray = {0};
static const char *text_generate_onionid = {0};
static const char *text_generate_torxid = {0};
static const char *text_disable = {0};
static const char *text_enable = {0};
static const char *text_no = {0};
static const char *text_yes = {0};
static const char *text_leave_after = {0};
static const char *text_delete_after = {0};
static const char *text_select = {0};
static const char *text_save_sing = {0};
static const char *text_save_mult = {0};
static const char *text_emit_global_kill = {0};
static const char *text_vertical_emit_global_kill = {0};
static const char *text_peer = {0};
static const char *text_group = {0};
static const char *text_group_offer = {0};
static const char *text_audio_message = {0};
static const char *text_audio_call = {0};
static const char *text_sticker = {0};
static const char *text_current_members = {0};
static const char *text_group_public = {0};
static const char *text_group_private = {0};
static const char *text_block = {0};
static const char *text_unblock = {0};
static const char *text_ignore = {0};
static const char *text_unignore = {0};
static const char *text_edit = {0};
static const char *text_incoming = {0};
static const char *text_outgoing = {0};
static const char *text_active_mult = {0};
static const char *text_active_sing = {0};
static const char *text_you = {0};
static const char *text_queued = {0};
static const char *text_draft = {0};
static const char *text_accept = {0};
static const char *text_reject = {0};
static const char *text_copy = {0};
static const char *text_resend = {0};
static const char *text_start = {0};
static const char *text_pause = {0};
static const char *text_choose_file = {0};
static const char *text_choose_files = {0};
static const char *text_choose_folder = {0};
static const char *text_open_folder = {0}; // TODO implement after 4.10 drops
static const char *text_cancel = {0};
static const char *text_transfer_paused = {0};
static const char *text_transfer_rejected = {0};
static const char *text_transfer_cancelled = {0};
static const char *text_show_qr = {0};
static const char *text_save_qr = {0};
static const char *text_copy_qr = {0};
static const char *text_delete = {0};
static const char *text_delete_log = {0};
static const char *text_hold_to_talk = {0};
static const char *text_cancel_editing = {0};
static const char *text_private_messaging = {0};
static const char *text_rename = {0};
static const char *text_button_add = {0};
static const char *text_button_join = {0};
static const char *text_button_sing = {0};
static const char *text_button_mult = {0};
static const char *text_button_generate_invite = {0};
static const char *text_button_generate_public = {0};
static const char *text_wait = {0};
static const char *text_tooltip_image_header_0 = {0};
static const char *text_tooltip_image_header_1 = {0};
static const char *text_tooltip_image_header_2 = {0};
static const char *text_tooltip_image_header_3 = {0};
static const char *text_tooltip_image_header_4 = {0};
static const char *text_tooltip_image_header_5 = {0};
static const char *text_tooltip_image_header_6 = {0};
static const char *text_tooltip_image_header_7 = {0};
static const char *text_tooltip_image_header_8 = {0};
static const char *text_tooltip_image_header_9 = {0};
static const char *text_tooltip_logging_disabled = {0};
static const char *text_tooltip_logging_enabled = {0};
static const char *text_tooltip_logging_global_on = {0};
static const char *text_tooltip_logging_global_off = {0};
static const char *text_tooltip_notifications_off = {0};
static const char *text_tooltip_notifications_on = {0};
static const char *text_tooltip_blocked_on = {0};
static const char *text_tooltip_blocked_off = {0};
static const char *text_tooltip_button_select_custom = {0};
static const char *text_tooltip_button_custom_sing = {0};
static const char *text_tooltip_button_custom_mult = {0};
static const char *text_tooltip_group_or_user_name = {0};
static const char *text_tooltip_button_kill = {0};
static const char *text_tooltip_button_delete = {0};
static const char *text_tooltip_button_delete_log = {0};
static const char *text_status_online = {0};
static const char *text_of = {0};
static const char *text_status_last_seen = {0};
static const char *text_status_never = {0};
static const char *text_edit_torrc = {0};
static const char *text_saving_will_restart_tor = {0};
static const char *text_save = {0};
static const char *text_override = {0};
static const char *text_change_password = {0};
//static const char *text_resume_interupted = {0};
static const char *text_old_password = {0};
static const char *text_new_password = {0};
static const char *text_new_password_again = {0};
static const char *text_settings = {0};
static const char *text_set_select_theme = {0};
static const char *text_set_select_language = {0};
static const char *text_set_onionid_or_torxid = {0};
static const char *text_set_global_log = {0};
static const char *text_set_auto_resume_inbound = {0};
static const char *text_set_save_all_stickers = {0};
static const char *text_set_download_directory = {0};
static const char *text_tor = {0};
static const char *text_snowflake = {0};
static const char *text_lyrebird = {0};
static const char *text_conjure = {0};
static const char *text_binary_location = {0};
static const char *text_set_cpu = {0};
static const char *text_set_suffix = {0};
static const char *text_set_validity_sing = {0};
static const char *text_set_validity_mult = {0};
static const char *text_set_auto_mult = {0};
static const char *text_set_tor_port_socks = {0};
static const char *text_set_tor_port_ctrl = {0};
static const char *text_set_tor_password = {0};
static const char *text_set_externally_generated = {0};
static const char *text_tor_log = {0};
static const char *text_torx_log = {0};
static const char *text_global_kill = {0};
static const char *text_global_kill_warning = {0};
static const char *text_home = {0};
static const char *text_add_generate = {0};
static const char *text_add_peer_by = {0};
static const char *text_add_group_by = {0};
static const char *text_generate_for = {0};
static const char *text_generate_group_for = {0};
static const char *text_qr_code = {0};
static const char *text_enter_password = {0};
static const char *text_enter = {0};
static const char *text_incorrect = {0};
static const char *text_debug_level = {0};
static const char *text_debug_level_notice = {0};
static const char *text_fatal_error = {0};
static const char *text_active = {0};
static const char *text_identifier = {0};
static const char *text_onionid = {0};
static const char *text_torxid = {0};
static const char *text_invitor = {0};
static const char *text_groupid = {0};
static const char *text_successfully_created_group = {0};
static const char *text_error_creating_group = {0};
static const char *text_censored_region = {0};
static const char *text_invite_friend = {0};  // unused in GTK
static const char *text_group_peers = {0}; // unused in GTK
static const char *text_incoming_call = {0};

const char **options[] = { &text_peer, &text_group, &text_block, &text_incoming, &text_outgoing, &text_active_mult, &text_active_sing }; // do not modify without thinking

void call_update(const int n,const int c);
void call_join(void *arg);
void call_ignore(void *arg);
void call_leave(void *arg);
void call_leave_all_except(const int except_n,const int except_c);
void call_peer_joining(const int call_n,const int c,const int peer_n);
void call_peer_leaving(const int call_n,const int c,const int peer_n);
void call_peer_leaving_all_except(const int n,const int except_n,const int except_c);
static void ui_input_new(GtkWidget *entry);
static void ui_input_bad(GtkWidget *entry);
static void ui_select_changed(const void *data);
static void ui_decorate_panel_left(const int n);
static void ui_theme(const int theme_local);
static void ui_show_generate(void);
static void ui_show_home(void);
static void ui_show_settings(void);
static void ui_go_back(void *arg);
static int ui_populate_peers(void *arg);
static void ui_decorate_panel_left_top(void);
GtkWidget *ui_add_chat_node(const int n,const int call_n,const int call_c,void (*callback_click)(const void *),const int minimal_size)__attribute__((warn_unused_result));
GtkWidget *ui_button_generate(const int type,const int n)__attribute__((warn_unused_result));
GtkWidget *ui_choose_binary(char **location,const char *widget_name,const char *label_text)__attribute__((warn_unused_result));
GtkWidget *ui_setting_entry(void (*callback)(GtkWidget *,void*), const char *description,const char *existing)__attribute__((warn_unused_result));
static inline uint8_t is_image_file(const char *filename)__attribute__((warn_unused_result));
static void ui_print_message(const int n,const int i,const int scroll);
int print_message_idle(void *arg);

static GtkApplication *gtk_application_gtk4;

static GdkTexture *texture_logo;
static GdkTexture *texture_headerbar_button1;
static GdkTexture *texture_headerbar_button2;
static GdkTexture *texture_headerbar_button3;
static GdkTexture *texture_headerbar_button_leave1;
static GdkTexture *texture_headerbar_button_leave2;
static GdkTexture *texture_headerbar_button_leave3;
static GdkTexture *file_file;
static GdkTexture *file_txt;
static GdkTexture *file_music;
static GdkTexture *file_image;
static GdkTexture *file_zip;

static GIcon *search_dark;
static GIcon *search_light;
static GdkTexture *fail_ico;
static GdkTexture *add_active;
static GdkTexture *add_inactive;
static GdkTexture *add_inactive_light;
static GdkTexture *home_active;
static GdkTexture *home_inactive;
static GdkTexture *home_inactive_light;
static GdkTexture *settings_active;
static GdkTexture *settings_inactive;
static GdkTexture *settings_inactive_light;
static GdkTexture *dot_red;
static GdkTexture *dot_green;
static GdkTexture *dot_yellow;
static GdkTexture *dot_orange;
static GdkTexture *dot_grey;
static GdkTexture *lock_red;
static GdkTexture *lock_green;
static GdkTexture *lock_yellow;
static GdkTexture *lock_orange;
static GdkTexture *lock_grey;
static GdkTexture *unlock_red;
static GdkTexture *unlock_green;
static GdkTexture *unlock_yellow;
static GdkTexture *unlock_orange;
static GdkTexture *unlock_grey;
static GdkTexture *logging_active;
static GdkTexture *logging_inactive;
static GdkTexture *logging_inactive_light;
static GdkTexture *globe_active;
static GdkTexture *globe_inactive;
static GdkTexture *globe_inactive_light;
static GdkTexture *bell_active;
static GdkTexture *bell_inactive;
static GdkTexture *bell_inactive_light;
static GdkTexture *block_active;
static GdkTexture *block_inactive;
static GdkTexture *block_inactive_light;
static GdkTexture *kill_active;
static GdkTexture *kill_inactive;
static GdkTexture *kill_inactive_light;
static GdkTexture *delete_active;
static GdkTexture *delete_inactive;
static GdkTexture *delete_inactive_light;
static GdkTexture *keyboard_dark;
static GdkTexture *keyboard_light;
static GdkTexture *microphone_dark;
static GdkTexture *microphone_light;
static GdkTexture *emoji_dark;
static GdkTexture *emoji_light;
static GdkTexture *attach_dark;
static GdkTexture *attach_light;
static GdkTexture *clear_all_dark;
static GdkTexture *clear_all_light;
static GdkTexture *clear_all_active;
static GdkTexture *gif_box_dark;
static GdkTexture *gif_box_light;
static GdkTexture *more_vert_dark;
static GdkTexture *more_vert_light;
static GdkTexture *send_dark;
static GdkTexture *send_light;
//static GdkTexture *clear_dark; // unused so far
//static GdkTexture *clear_light; // unused so far
static GdkTexture *call_light;
static GdkTexture *call_dark;
static GdkTexture *call_end_light;
static GdkTexture *call_end_dark;
//static GdkTexture *diversity_2_light; // TODO unused so far, for replacing "Group"
//static GdkTexture *diversity_2_dark; // TODO unused so far, for replacing "Group"
static GdkTexture *group_light;
static GdkTexture *group_dark;
//static GdkTexture *groups_light; // TODO unused so far, might not use, in favor of diversity_2
//static GdkTexture *groups_dark; // TODO unused so far, might not use, in favor of diversity_2
static GdkTexture *mic_off_light;
static GdkTexture *mic_off_dark;
//static GdkTexture *no_accounts_light; // TODO unused so far, for replacing "Block"
//static GdkTexture *no_accounts_dark; // TODO unused so far, for replacing "Block"
//static GdkTexture *person_light; // TODO unused so far, for replacing "Peer"
//static GdkTexture *person_dark; // TODO unused so far, for replacing "Peer"
//static GdkTexture *phone_callback_light; // TODO unused so far, might not use
//static GdkTexture *phone_callback_dark; // TODO unused so far, might not use
//static GdkTexture *phone_callback_green; // unused so far
//static GdkTexture *phone_missed_light; // TODO unused so far, might not use
//static GdkTexture *phone_missed_dark; // TODO unused so far, might not use
//static GdkTexture *phone_missed_red; // unused so far
static GdkTexture *volume_off_light;
static GdkTexture *volume_off_dark;
static GdkTexture *volume_up_light;
static GdkTexture *volume_up_dark;

/* Non-global struct models */
/*
struct long_presses {
	GtkWidget *button;
	int n;
}; */

struct stack_change { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	GtkStack *stack;
	size_t iter;
};

struct printing { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	int n;
	int i;
	int scroll;
};

struct notification { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	char *heading;
	char *message;
};

struct custom_setting { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	int n;
	char *setting_name;
	char *setting_value;
	size_t setting_value_len;
	int plaintext;
};

struct progress { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	int n;
	int f;
	uint64_t transferred;
};

struct stream_data { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	int n;
	int p_iter;
	char *data;
	uint32_t data_len;
};

struct int_p_p {
	int loaded;
	int *loaded_array_n;
	int *loaded_array_i;
};

struct int_int { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	int n;
	int i;
};

G_DECLARE_FINAL_TYPE(IntPair, int_pair, G, INT_PAIR, GObject)

struct _IntPair {
	GObject parent_instance;
	gint first;
	gint second;
	gint third;
	gint fourth;
};

static void int_pair_class_init(IntPairClass *klass)
{ // Class initialization
	(void) klass;
}

static void int_pair_init(IntPair *self)
{ // Instance initialization
	(void) self;
}

static IntPair *int_pair_new(gint first,gint second,gint third,gint fourth)
{ // Create new IntPair instances.
	IntPair *pair = g_object_new(int_pair_get_type(), NULL);
	pair->first = first;
	pair->second = second;
	pair->third = third;
	pair->fourth = fourth;
	return pair;
}

G_DEFINE_TYPE(IntPair, int_pair, G_TYPE_OBJECT)

static inline void *iitovp(const int n,const int i)
{ // This is ONLY for functions where it is NOT POSSIBLE to free the argument. If it is possible to free, pass an int_int struct manually and free it.
	if(sizeof(void*) == 8) // 64 bit, no malloc
	{
		if(n+SHIFT == 0)
		{
			error_printf(0,"Shift is insufficient in iitovp. Must increase it or determine source of error. Coding error. Report this. %d",n);
			breakpoint();
		}
		return (void*)(((uint64_t)(n+SHIFT) << 32) | (uint32_t)i);
	}
	struct int_int *int_int = torx_insecure_malloc(sizeof(struct int_int));
	int_int->n = n;
	int_int->i = i;
	return int_int;
}

static inline int vptoii_n(const void *arg)
{ // WARNING: Does not free arg (applicable to 32 bit only)
	if(sizeof(void*) == 8) // 64 bit, no malloc
		return (int)((uint64_t)arg >> 32)-SHIFT;
	const struct int_int *int_int = (const struct int_int*) arg; // Casting passed struct
	return int_int->n;
}

static inline int vptoii_i(const void *arg)
{ // WARNING: Does not free arg (applicable to 32 bit only)
	if(sizeof(void*) == 8) // 64 bit, no malloc
		return (int)((uint64_t)arg & 0xFFFFFFFF);
	const struct int_int *int_int = (const struct int_int*) arg; // Casting passed struct
	return int_int->i;
}

static void gtk_box_remove_all(GtkWidget *box)
{ // Similar to ui_kill_children
	if(!box)
		return;
	for(GtkWidget *child = gtk_widget_get_first_child(box) ; child != NULL ; child = gtk_widget_get_first_child(box))
		gtk_box_remove(GTK_BOX(box), child);
}

static int set_c(const int n,const time_t time,const time_t nstime)
{ // XXX -1 is error. Be sure to accomodate.
	size_t c;
	for (c = 0; c < sizeof(t_peer[n].t_call)/sizeof(struct t_call_list); c++)
		if (t_peer[n].t_call[c].start_time == time && t_peer[n].t_call[c].start_nstime == nstime)
			return (int)c; // found existing match
	for (c = 0; c < sizeof(t_peer[n].t_call)/sizeof(struct t_call_list); c++)
		if(t_peer[n].t_call[c].start_time == 0 && t_peer[n].t_call[c].start_nstime == 0)
			break; // found an empty slot
	if(c == sizeof(t_peer[n].t_call)/sizeof(struct t_call_list))
	{ // This cannot be a fatal error because otherwise a malicious user could trigger this to crash everyone in a group
		error_simple(0,"Set_c has too many calls. More than reasonably anticipated. Coding error. Report this."); // array of struct is too small
		return -1;
	}
	t_peer[n].t_call[c].start_time = time;
	t_peer[n].t_call[c].start_nstime = nstime;
	return (int)c;
}

static void initialize_peer_call(const int n,const int c)
{
	if(n < 0 || (size_t)c >= sizeof(t_peer[n].t_call)/sizeof(struct t_call_list))
	{
		error_simple(0,"Sanity check failed in initialize_peer_call. Coding error. Report this.");
		return;
	}
	t_peer[n].t_call[c].joined = 0;
	t_peer[n].t_call[c].waiting = 0;
	t_peer[n].t_call[c].mic_on = 1;
	t_peer[n].t_call[c].speaker_on = 1;
	t_peer[n].t_call[c].speaker_phone = 0;
	t_peer[n].t_call[c].audio_only = 1;
	t_peer[n].t_call[c].start_time = 0;
	t_peer[n].t_call[c].start_nstime = 0;
	for(size_t iter = 0 ; iter < sizeof(t_peer[n].t_call[c].participating)/sizeof(int) ; iter++)
	{
		t_peer[n].t_call[c].participating[iter] = -1;
		t_peer[n].t_call[c].participant_mic[iter] = 1; // default, enabled
		t_peer[n].t_call[c].participant_speaker[iter] = 1; // default, enabled
	}
	t_peer[n].t_call[c].column = NULL; // TODO when zeroing an existing, call g_object_unref(t_peer[n].t_call[c].column);
}

static void initialize_peer_call_list(const int n)
{
	for(size_t c = 0 ; c < sizeof(t_peer[n].t_call)/sizeof(struct t_call_list) ; c++)
		initialize_peer_call(n,(int)c);
}

static int call_participant_count(const int n, const int c)
{
	if(n < 0 || (size_t)c >= sizeof(t_peer[n].t_call)/sizeof(struct t_call_list))
	{
		error_simple(0,"Sanity check failed in call_participant_count. Coding error. Report this.");
		return 0;
	}
	int participating = 0;
	for(size_t iter = 0 ; iter < sizeof(t_peer[n].t_call[c].participating)/sizeof(int) ; iter++)
		if(t_peer[n].t_call[c].participating[iter] > -1)
			participating++;
	return participating;
}

static void toggle_int8(void *arg)
{ // Works for int8_t and uint8_t // XXX DO NOT USE WITH g_signal_connect / g_signal_connect_swapped / any usage requiring "&" prefix
	if(*(uint8_t *)arg)
		*(uint8_t *)arg = 0;
	else
		*(uint8_t *)arg = 1;
}

static void toggle_mic(GtkWidget *button,void *arg)
{ // For not a specific participant, pass -1 as participant_iter
	(void)button;
	struct printing *printing = (struct printing*) arg; // Casting passed struct
	const int call_n = printing->n;
	const int call_c = printing->i;
	const int participant_iter = printing->scroll;
	if(call_n < 0 || call_c < 0 || participant_iter >= (int)sizeof(t_peer[call_n].t_call[call_c].participant_mic))
	{
		error_simple(0,"Sanity check failed in toggle_mic. Coding error. Report this to UI Devs.");
		return;
	}
	if(participant_iter > -1)
		toggle_int8(&t_peer[call_n].t_call[call_c].participant_mic[participant_iter]); // safe usage
	else
	{
		if(t_peer[call_n].t_call[call_c].joined && t_peer[call_n].t_call[call_c].mic_on && current_recording.pipeline)
		{
			unsigned char *to_free = record_stop(NULL,NULL,&current_recording);
			torx_free((void*)&to_free);
		}
		toggle_int8(&t_peer[call_n].t_call[call_c].mic_on); // safe usage
	}
	call_update(call_n,call_c);
}

static void toggle_speaker(GtkWidget *button,void *arg)
{ // For not a specific participant, pass -1 as participant_iter
	(void)button;
	struct printing *printing = (struct printing*) arg; // Casting passed struct
	const int call_n = printing->n;
	const int call_c = printing->i;
	const int participant_iter = printing->scroll;
	if(call_n < 0 || call_c < 0 || participant_iter >= (int)sizeof(t_peer[call_n].t_call[call_c].participant_speaker))
	{
		error_simple(0,"Sanity check failed in toggle_speaker. Coding error. Report this to UI Devs.");
		return;
	}
	if(participant_iter > -1)
		toggle_int8(&t_peer[call_n].t_call[call_c].participant_speaker[participant_iter]); // safe usage
	else
		toggle_int8(&t_peer[call_n].t_call[call_c].speaker_on); // safe usage
	call_update(call_n,call_c);
}

static void custom_popover_closed(GtkPopover* self,gpointer user_data)
{ // XXX For working-around GTK bug on double level popovers
	(void)user_data;
	(void)self;
	if(popover_level_one)
	{
		gtk_popover_popdown(GTK_POPOVER(popover_level_one));
		popover_level_one = NULL;
	}
}

static GtkWidget *custom_popover_new(GtkWidget *parent)
{
	GtkWidget *custom_popover = gtk_popover_new();
	gtk_widget_set_parent(custom_popover,parent);
	g_signal_connect(custom_popover, "closed", G_CALLBACK (custom_popover_closed), NULL); // XXX For working-around GTK bug on double level popovers
	g_signal_connect(custom_popover, "closed", G_CALLBACK (gtk_widget_unparent), custom_popover); // XXX necessary or a warning occurs
	if(!popover_level_one) // XXX For working-around GTK bug on double level popovers
		popover_level_one = custom_popover; // XXX For working-around GTK bug on double level popovers
	return custom_popover;
}

static void chat_builder(GtkListItemFactory *factory, GtkListItem *list_item,void (*callback_click)(const void *))
{ // This is for peer list
	(void) factory;
	IntPair *pair = gtk_list_item_get_item(list_item);
	if(pair->fourth == 2)
		gtk_list_item_set_activatable(list_item,FALSE);
	else
		gtk_list_item_set_activatable(list_item,TRUE); // XXX this changes whether hover is visible
	if(pair)
	{
		GtkWidget *node = ui_add_chat_node(pair->first,pair->second,pair->third,callback_click,pair->fourth);
		gtk_widget_add_css_class(node, "node");
		gtk_list_item_set_child(list_item, node);
	}
}

static void ui_participant_list(void *arg)
{ // modelled after ui_group_peerlist
	const int call_n = vptoii_n(arg); // DO NOT FREE ARG
	const int call_c = vptoii_i(arg); // DO NOT FREE ARG

	t_main.popover_participant_list = custom_popover_new(t_main.button_participants);

	GtkWidget *box_popover_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box_popover_outer, "popover_inner");
	GtkWidget *scrolled_window_popover = gtk_scrolled_window_new();
	gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW (scrolled_window_popover),size_peerlist_minimum_width);

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_popover),box_popover_outer);
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_participant_list),scrolled_window_popover);
	gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (scrolled_window_popover),size_peerlist_button_height*8);
	GtkWidget *box_popover_inner = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);

	gtk_box_append(GTK_BOX(box_popover_outer), box_popover_inner);

	const int participants = call_participant_count(call_n,call_c);
	if(participants)
	{
		GListStore *list_store = g_list_store_new(int_pair_get_type());
		GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (list_store));

		for(size_t iter = 0 ; iter < sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int) ; iter++)
			if(t_peer[call_n].t_call[call_c].participating[iter] > -1)
				g_list_store_append(list_store, int_pair_new(t_peer[call_n].t_call[call_c].participating[iter],call_n,call_c,2));

		GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
		g_signal_connect(factory, "bind", G_CALLBACK(chat_builder),NULL); // TODO make click do something
		GtkWidget *list_view = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory); // TODO NOT A direct child of a scrolled window, could have issues with > 205 widgets
		gtk_box_append (GTK_BOX(box_popover_inner), list_view);
	}

	gtk_popover_popup(GTK_POPOVER(t_main.popover_participant_list));
}

static void audio_ready(void *arg,const unsigned char *data,const size_t data_len)
{
	const int call_n = vptoii_n(arg); // DO NOT FREE ARG
	const int call_c = vptoii_i(arg); // DO NOT FREE ARG
	if(call_n < 0 || call_c < 0)
	{
		error_simple(0,"Sanity check failed in audio_ready. Coding error. Report to UI Devs.");
		return;
	}
	int recipient_list[sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int)];
	uint32_t recipient_count = 0;
	for(size_t iter = 0 ; iter < sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int) ; iter++)
		if(t_peer[call_n].t_call[call_c].participating[iter] > -1 && t_peer[call_n].t_call[call_c].participant_mic[iter])
		{
			recipient_list[recipient_count] = t_peer[call_n].t_call[call_c].participating[iter];
			recipient_count++;
		}
	if(!recipient_count)
	{
		unsigned char *to_free = record_stop(NULL,NULL,&current_recording);
		torx_free((void*)&to_free);
		return;
	}
	const uint32_t message_len = (uint32_t)(8 + data_len);
	unsigned char message[message_len]; // zero'd
	uint32_t trash = htobe32((uint32_t)t_peer[call_n].t_call[call_c].start_time);
	memcpy(message,&trash,sizeof(trash));
	trash = htobe32((uint32_t)t_peer[call_n].t_call[call_c].start_nstime);
	memcpy(&message[4],&trash,sizeof(trash));
	memcpy(&message[8],data,data_len);
	message_send_select(recipient_count,recipient_list,ENUM_PROTOCOL_AAC_AUDIO_STREAM_DATA_DATE,message,message_len);
	sodium_memzero(message,sizeof(message));
}

void call_update(const int n,const int c)
{ // Here should handle everything within a column. Note: N could be CTRL, GROUP_PEER, or GROUP_CTRL
	if(n < 0 || (size_t)c >= sizeof(t_peer[n].t_call)/sizeof(struct t_call_list))
	{
		error_simple(0,"Sanity check failed in call_update. Coding error. Report this.");
		return;
	}
	const int participants = call_participant_count(n,c);
	if(t_peer[n].t_call[c].joined || t_peer[n].t_call[c].waiting)
	{
		int group_n = -800; // DO NOT INITIALIZE AS -1!!!!
		const uint8_t owner = getter_uint8(n, INT_MIN, -1, offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_GROUP_PEER)
		{
			const int g = set_g(n,NULL);
			group_n = getter_group_int(g,offsetof(struct group_list,n));
		}
		GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
		if(t_peer[n].t_call[c].column == NULL)
		{
			t_peer[n].t_call[c].column = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
			g_object_ref(t_peer[n].t_call[c].column); // XXX THIS IS NECESSARY BECAUSE OTHERWISE IT WILL BE DESTROYED WHEN WE NAVIGATE AWAY. Otherwise we need to re-write this function and call_update instead of just adding .column to .call_box
		//	printf("Checkpoint call_update 2: global_n=%d group_n=%d n=%d c=%d\n",global_n,group_n,n,c);
			if(n == global_n || group_n == global_n)
				gtk_box_append(GTK_BOX(t_main.call_box), t_peer[n].t_call[c].column);
		}
		else
			gtk_box_remove_all(t_peer[n].t_call[c].column);
		if(t_peer[n].t_call[c].joined)
		{
			if(participants)
			{ // We are in an existing call
				struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
				printing->n = n;
				printing->i = c;
				printing->scroll = -1; // no specific participant iter

				GtkWidget *button_mic = gtk_button_new();
				gtk_widget_add_css_class(button_mic, "invisible");
				if(t_peer[n].t_call[c].mic_on)
				{
					if(!current_recording.pipeline)
						record_start(&current_recording,16000,audio_ready,iitovp(n,c));
					if(global_theme == DARK_THEME)
						gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(mic_off_dark)));
					else
						gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(mic_off_light)));
				}
				else
				{
					if(global_theme == DARK_THEME)
						gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_dark)));
					else
						gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_light)));
				}
				gtk_widget_set_size_request(button_mic, size_icon_bottom_right, size_icon_bottom_right);
				g_signal_connect(button_mic, "clicked", G_CALLBACK(toggle_mic),printing);
				gtk_box_append(GTK_BOX(row),button_mic);

				GtkWidget *button_speaker = gtk_button_new();
				gtk_widget_add_css_class(button_speaker, "invisible");
				if(t_peer[n].t_call[c].speaker_on)
				{
					if(global_theme == DARK_THEME)
						gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_off_dark)));
					else
						gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_off_light)));
				}
				else
				{
					if(global_theme == DARK_THEME)
						gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_up_dark)));
					else
						gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_up_light)));
				}
				gtk_widget_set_size_request(button_speaker, size_icon_bottom_right, size_icon_bottom_right);
				g_signal_connect(button_speaker, "clicked", G_CALLBACK(toggle_speaker),printing);
				gtk_box_append(GTK_BOX(row),button_speaker);

				if(owner == ENUM_OWNER_GROUP_CTRL)
				{
					t_main.button_participants = gtk_button_new();
					gtk_widget_add_css_class(t_main.button_participants, "invisible");
					if(global_theme == DARK_THEME)
						gtk_button_set_child(GTK_BUTTON(t_main.button_participants),gtk_image_new_from_paintable(GDK_PAINTABLE(group_dark)));
					else
						gtk_button_set_child(GTK_BUTTON(t_main.button_participants),gtk_image_new_from_paintable(GDK_PAINTABLE(group_light)));
					gtk_widget_set_size_request(t_main.button_participants, size_icon_bottom_right, size_icon_bottom_right);
					g_signal_connect_swapped(t_main.button_participants, "clicked", G_CALLBACK(ui_participant_list),iitovp(n,c));
				/*	GtkGesture *click = gtk_gesture_click_new();
					g_signal_connect_swapped(click, "pressed", G_CALLBACK(ui_participant_list), iitovp(n,c)); // DO NOT FREE arg because this only gets passed ONCE.
					gtk_widget_add_controller(t_main.button_participants, GTK_EVENT_CONTROLLER(click));	*/
					gtk_box_append(GTK_BOX(row),t_main.button_participants);
				}
			}
			else if(t_peer[n].t_call[c].mic_on && current_recording.pipeline) // + no participants
			{ // NOTE: This probably won't trigger, or certainly won't always. Its more likely that record_stop triggers elsewhere.
				unsigned char *to_free = record_stop(NULL,NULL,&current_recording);
				torx_free((void*)&to_free);
			}
			GtkWidget *button_hangup = gtk_button_new();
			gtk_widget_add_css_class(button_hangup, "invisible");
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_hangup),gtk_image_new_from_paintable(GDK_PAINTABLE(call_end_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_hangup),gtk_image_new_from_paintable(GDK_PAINTABLE(call_end_light)));
			gtk_widget_set_size_request(button_hangup, size_icon_bottom_right, size_icon_bottom_right);
			g_signal_connect_swapped(button_hangup, "clicked", G_CALLBACK(call_leave),iitovp(n,c));
			gtk_box_append(GTK_BOX(row),button_hangup);
		}
		else if(t_peer[n].t_call[c].waiting)
		{ // Incoming call, should display only Reject / Accept
			GtkWidget *button_accept = gtk_button_new();
			gtk_widget_add_css_class(button_accept, "invisible");
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_accept),gtk_image_new_from_paintable(GDK_PAINTABLE(call_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_accept),gtk_image_new_from_paintable(GDK_PAINTABLE(call_light)));
			gtk_widget_set_size_request(button_accept, size_icon_bottom_right, size_icon_bottom_right);
			g_signal_connect_swapped(button_accept, "clicked", G_CALLBACK(call_join),iitovp(n,c));
			gtk_box_append(GTK_BOX(row),button_accept);

			GtkWidget *button_reject = gtk_button_new();
			gtk_widget_add_css_class(button_reject, "invisible");
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_reject),gtk_image_new_from_paintable(GDK_PAINTABLE(call_end_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_reject),gtk_image_new_from_paintable(GDK_PAINTABLE(call_end_light)));
			gtk_widget_set_size_request(button_reject, size_icon_bottom_right, size_icon_bottom_right);
			g_signal_connect_swapped(button_reject, "clicked", G_CALLBACK(call_leave),iitovp(n,c));
			gtk_box_append(GTK_BOX(row),button_reject);
		}
		gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
		if(owner == ENUM_OWNER_GROUP_PEER)
		{
			char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			GtkWidget *label = gtk_label_new(peernick);
			torx_free((void*)&peernick);
			gtk_box_append(GTK_BOX(t_peer[n].t_call[c].column),label);
		}
		gtk_box_append(GTK_BOX(t_peer[n].t_call[c].column),row);
	}
	else if(t_peer[n].t_call[c].column)
	{ // Destroy the row, we are leaving or not joining the call
		gtk_box_remove_all(t_peer[n].t_call[c].column);
		gtk_widget_unparent(t_peer[n].t_call[c].column);
		g_object_unref(t_peer[n].t_call[c].column);
		t_peer[n].t_call[c].column = NULL;
	}
	else
		error_simple(0,"Unexpected or repeated call_update. Coding error. Report this.");
}

void call_join(void *arg)
{
	const int n = vptoii_n(arg);
	const int c = vptoii_i(arg);
	if(n < 0 || (size_t)c >= sizeof(t_peer[n].t_call)/sizeof(struct t_call_list))
	{
		error_simple(0,"Sanity check failed in call_join. Coding error. Report this.");
		return;
	}
	if(t_peer[n].t_call[c].start_time == 0 && t_peer[n].t_call[c].start_nstime == 0)
	{
		error_simple(0, "Cannot join a call with zero time/nstime. Coding error. Report this.");
		return;
	}
	call_leave_all_except(n,c);
	if(t_peer[n].t_call[c].waiting)
		ring_stop(); // XXX stop ringing
	t_peer[n].t_call[c].waiting = 0;
	t_peer[n].t_call[c].joined = 1;
	const uint8_t owner = getter_uint8(n, INT_MIN, -1, offsetof(struct peer_list,owner));
	uint16_t protocol;
	if(owner == ENUM_OWNER_GROUP_PEER)
		protocol = ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE;
	else
		protocol = ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN;
	unsigned char message[8];
	uint32_t trash = htobe32((uint32_t)t_peer[n].t_call[c].start_time);
	memcpy(message,&trash,sizeof(trash));
	trash = htobe32((uint32_t)t_peer[n].t_call[c].start_nstime);
	memcpy(&message[4],&trash,sizeof(trash));
//	printf("Checkpoint call_join to ${t_peer[n].t_call[c].participating[c].length} peers ${t_peer[n].t_call[c].start_time[c]}:${t_peer[n].t_call[c].start_time[c]}");
	const int participants = call_participant_count(n,c);
	if(participants)
	{
		for(size_t iter = 0; iter < sizeof(t_peer[n].t_call[c].participating)/sizeof(int); iter++) // Join Existing call
			if(t_peer[n].t_call[c].participating[iter] > -1)
				message_send(t_peer[n].t_call[c].participating[iter],protocol,message,sizeof(message));
	}
	else // Start new call
		message_send(n,protocol,message,(uint32_t)sizeof(message));
	sodium_memzero(message,sizeof(message));
	call_update(n,c);
}

void call_ignore(void *arg)
{ // so far kind of under-utilized
	const int n = vptoii_n(arg);
	const int c = vptoii_i(arg);
	if(n < 0 || (size_t)c >= sizeof(t_peer[n].t_call)/sizeof(struct t_call_list) || (!t_peer[n].t_call[c].joined && !t_peer[n].t_call[c].waiting))
	{
		error_simple(0,"Sanity check failed in call_ignore. Coding error. Report this.");
		return;
	}
	if(t_peer[n].t_call[c].waiting)
		ring_stop(); // XXX stop ringing
	if(t_peer[n].t_call[c].joined && t_peer[n].t_call[c].mic_on && current_recording.pipeline)
	{
		unsigned char *to_free = record_stop(NULL,NULL,&current_recording);
		torx_free((void*)&to_free);
	}
	t_peer[n].t_call[c].waiting = 0;
	t_peer[n].t_call[c].joined = 0;
	call_update(n,c);
}

void call_leave(void *arg)
{
	const int n = vptoii_n(arg);
	const int c = vptoii_i(arg);
	if(n < 0 || (size_t)c >= sizeof(t_peer[n].t_call)/sizeof(struct t_call_list) || (!t_peer[n].t_call[c].joined && !t_peer[n].t_call[c].waiting))
	{
		error_simple(0,"Sanity check failed in call_leave. Coding error. Report this.");
		return;
	}
printf("Checkpoint call_leave n=%d c=%d\n",n,c);
	unsigned char message[8];
	uint32_t trash = htobe32((uint32_t)t_peer[n].t_call[c].start_time);
	memcpy(message,&trash,sizeof(trash));
	trash = htobe32((uint32_t)t_peer[n].t_call[c].start_nstime);
	memcpy(&message[4],&trash,sizeof(trash));
	// printf("Checkpoint call_leave to ${t_peer.t_call[n].participating[c].length} peers ${t_peer.t_call[n].start_time[c]}:${t_peer.t_call[n].start_time[c]}");
	int send_count = 0;
	if(t_peer[n].t_call[c].joined)
		for(size_t iter = 0; iter < sizeof(t_peer[n].t_call[c].participating)/sizeof(int); iter++)
			if(t_peer[n].t_call[c].participating[iter] > -1)
			{
				message_send(t_peer[n].t_call[c].participating[iter],ENUM_PROTOCOL_AAC_AUDIO_STREAM_LEAVE,message,sizeof(message));
				send_count++;
			}
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(send_count == 0 && (owner == ENUM_OWNER_CTRL || owner == ENUM_OWNER_GROUP_PEER))
		message_send(n,ENUM_PROTOCOL_AAC_AUDIO_STREAM_LEAVE,message,sizeof(message));
	sodium_memzero(message,sizeof(message));
	call_ignore(arg);
}

void call_leave_all_except(const int except_n,const int except_c)
{ // Leave or reject all active calls, except one (or none if -1). To be called primarily when call_join is called, but may also be called for other purposes.
	for(int call_n = 0; getter_byte(call_n,INT_MIN,-1,offsetof(struct peer_list,onion)) != 0 || getter_int(call_n,INT_MIN,-1,offsetof(struct peer_list,peer_index)) > -1 ; call_n++)
	{
		const uint8_t owner = getter_uint8(call_n,INT_MIN,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_CTRL || owner == ENUM_OWNER_GROUP_CTRL || owner == ENUM_OWNER_GROUP_PEER)
			for(size_t c = 0; c < sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list); c++)
				if((t_peer[call_n].t_call[c].joined || t_peer[call_n].t_call[c].waiting) && (t_peer[call_n].t_call[c].start_time != 0 || t_peer[call_n].t_call[c].start_nstime != 0))
					if(call_n != except_n || (int)c != except_c)
						call_leave(iitovp(call_n, (int)c));
	}
}

static void call_mute_all_except(const int except_n,const int except_c)
{ // Leave or reject all active calls, except one (or none if -1). To be called primarily when call_join is called, but may also be called for other purposes.
	for(int call_n = 0; getter_byte(call_n,INT_MIN,-1,offsetof(struct peer_list,onion)) != 0 || getter_int(call_n,INT_MIN,-1,offsetof(struct peer_list,peer_index)) > -1 ; call_n++)
	{
		const uint8_t owner = getter_uint8(call_n,INT_MIN,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_CTRL || owner == ENUM_OWNER_GROUP_CTRL || owner == ENUM_OWNER_GROUP_PEER)
			for(size_t c = 0; c < sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list); c++)
				if((t_peer[call_n].t_call[c].joined || t_peer[call_n].t_call[c].waiting) && (t_peer[call_n].t_call[c].start_time != 0 || t_peer[call_n].t_call[c].start_nstime != 0))
					if(call_n != except_n || (int)c != except_c)
					{
						if(t_peer[call_n].t_call[c].joined && t_peer[call_n].t_call[c].mic_on && current_recording.pipeline)
						{ // This isn't 100% certainty that the current_recording is for this call but it's a very good indication
							unsigned char *to_free = record_stop(NULL,NULL,&current_recording);
							torx_free((void*)&to_free);
						}
						t_peer[call_n].t_call[c].mic_on = 0;
						t_peer[call_n].t_call[c].speaker_on = 0;
					}
	}
}

void call_peer_joining(const int call_n,const int c,const int peer_n)
{
	if(call_n < 0 || peer_n < 0 || (size_t)c >= sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list))
	{
		error_simple(0,"Sanity check failed in call_peer_joining. Coding error. Report this.");
		return;
	}
	for(size_t iter = 0; iter < sizeof(t_peer[call_n].t_call[c].participating)/sizeof(int); iter++)
		if (peer_n == t_peer[call_n].t_call[c].participating[iter])
		{
			error_simple(0, "Peer is already part of call. Possible coding error. Report this.");
			return; // Peer is already in the call
		}
	call_peer_leaving_all_except(peer_n,call_n,c);
	// printf("Checkpoint call_peer_joining $call_n $c $peer_n");
	const int participants = call_participant_count(call_n,c);
	if(participants)
	{ // send a list of peer onions that are already in the call, excluding this peer, if any
		const uint32_t message_len = (uint32_t)(8 + 56 * participants);
		unsigned char message[message_len];
		uint32_t trash = htobe32((uint32_t)t_peer[call_n].t_call[c].start_time);
		memcpy(message,&trash,sizeof(trash));
		trash = htobe32((uint32_t)t_peer[call_n].t_call[c].start_nstime);
		memcpy(&message[4],&trash,sizeof(trash));
		for(size_t iter = 0; iter < sizeof(t_peer[call_n].t_call[c].participating)/sizeof(int); iter++)
			if(t_peer[call_n].t_call[c].participating[iter] > -1)
			{
				char *peeronion = getter_string(NULL,t_peer[call_n].t_call[c].participating[iter], INT_MIN, -1, offsetof(struct peer_list, peeronion));
				memcpy(&message[8+iter*56], peeronion, 56);
				torx_free((void*)&peeronion);
			}
		message_send(peer_n,ENUM_PROTOCOL_AAC_AUDIO_STREAM_PEERS,message,message_len);
		sodium_memzero(message,sizeof(message));
	}
	for(size_t iter = 0; iter < sizeof(t_peer[call_n].t_call[c].participating)/sizeof(int); iter++)
		if(t_peer[call_n].t_call[c].participating[iter] == -1)
		{ // Add this peer then break
			t_peer[call_n].t_call[c].participating[iter] = peer_n;
			t_peer[call_n].t_call[c].participant_mic[iter] = 1; // default, enabled
			t_peer[call_n].t_call[c].participant_speaker[iter] = 1; // default, enabled
			break;
		}
	call_update(call_n,c);
}

void call_peer_leaving(const int call_n,const int c,const int peer_n)
{
	if(call_n < 0 || peer_n < 0 || (size_t)c >= sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list))
	{
		error_simple(0,"Sanity check failed in call_peer_leaving. Coding error. Report this.");
		return;
	}
	for(size_t iter = 0; iter < sizeof(t_peer[call_n].t_call[c].participating)/sizeof(int); iter++)
		if (peer_n == t_peer[call_n].t_call[c].participating[iter])
		{ // TODO perhaps we should shift all forward
			t_peer[call_n].t_call[c].participating[iter] = -1;
			t_peer[call_n].t_call[c].participant_mic[iter] = 1; // default, enabled
			t_peer[call_n].t_call[c].participant_speaker[iter] = 1; // default, enabled
			break;
		}
	const uint8_t owner = getter_uint8(call_n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if((t_peer[call_n].t_call[c].joined || t_peer[call_n].t_call[c].waiting) && (owner == ENUM_OWNER_CTRL || owner == ENUM_OWNER_GROUP_PEER))
	{ // Ending the call if it is non-group
		if(t_peer[call_n].t_call[c].waiting)
			ring_stop(); // XXX stop ringing
		t_peer[call_n].t_call[c].joined = 0;
		t_peer[call_n].t_call[c].waiting = 0;
	}
	call_update(call_n,c);
}

void call_peer_leaving_all_except(const int n,const int except_n,const int except_c)
{ // Peer is leaving all calls (ex: they went offline)
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_PEER)
	{
		const int g = set_g(n,NULL);
		const int group_n = getter_group_int(g,offsetof(struct group_list,n));
		const int call_n = group_n;
		for(size_t c = 0; c < sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list); c++)
			if((t_peer[call_n].t_call[c].joined || t_peer[call_n].t_call[c].waiting) && (t_peer[call_n].t_call[c].start_time != 0 || t_peer[call_n].t_call[c].start_nstime != 0))
				if(call_n != except_n || (int)c != except_c)
					call_peer_leaving(call_n, (int)c, n);
	} // NOT ELSE
	const int call_n = n;
	for(size_t c = 0; c < sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list); c++)
		if((t_peer[call_n].t_call[c].joined || t_peer[call_n].t_call[c].waiting) && (t_peer[call_n].t_call[c].start_time != 0 || t_peer[call_n].t_call[c].start_nstime != 0))
			if(call_n != except_n || (int)c != except_c)
				call_peer_leaving(call_n, (int)c, n);
}

static int initialize_n_idle(void *arg)
{ // Remember to also cleanup in onion_deleted_idle
	const int n = vptoi(arg);
	t_peer[n].unread = 0;
	t_peer[n].mute = 0;
	t_peer[n].pm_n = -1;
	t_peer[n].edit_n = -1;
	t_peer[n].edit_i = INT_MIN;
	t_peer[n].buffer_write = NULL;

	t_peer[n].t_cache_info.audio_cache = NULL;
	t_peer[n].t_cache_info.audio_time = NULL;
	t_peer[n].t_cache_info.audio_nstime = NULL;
	t_peer[n].t_cache_info.last_played_time = 0;
	t_peer[n].t_cache_info.last_played_nstime = 0;
	t_peer[n].t_cache_info.stream_pipeline = NULL;
	t_peer[n].t_cache_info.playing = 0;

	t_peer[n].pointer_location = -10;
	t_peer[n].t_message = (struct t_message_list *)torx_insecure_malloc(sizeof(struct t_message_list) *21) - t_peer[n].pointer_location; // XXX Note this shift
	t_peer[n].t_file = torx_insecure_malloc(sizeof(struct t_file_list) *11);
	initialize_peer_call_list(n);
	sodium_memzero(t_peer[n].stickers_requested,sizeof(t_peer[n].stickers_requested));
	return 0;
}

void initialize_n_cb_ui(const int n)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,initialize_n_idle,itovp(n),NULL);
}

static int initialize_i_idle(void *arg)
{ // XXX NOTICE: These also need to be carefully reset following message_deleted_cb (currently they are)
	const int n = vptoii_n(arg);
	const int i = vptoii_i(arg);
	t_peer[n].t_message[i].pos = 0;
	t_peer[n].t_message[i].visible = 0;
	t_peer[n].t_message[i].unheard = 1;
	#if GTK_FACTORY_BUG
	t_peer[n].t_message[i].message_box = NULL;
	#endif
	return 0;
}

void initialize_i_cb_ui(const int n,const int i)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,initialize_i_idle,iitovp(n,i),NULL);
}

static int initialize_f_idle(void *arg)
{
	const int n = vptoii_n(arg);
	const int f = vptoii_i(arg);
	t_peer[n].t_file[f].n = -1;
	t_peer[n].t_file[f].i = INT_MIN;
	t_peer[n].t_file[f].progress_bar = NULL;
	t_peer[n].t_file[f].file_size = NULL;
	t_peer[n].t_file[f].previously_completed = 0; // TODO Consider moving this to library?
	return 0;
}

void initialize_f_cb_ui(const int n,const int f)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,initialize_f_idle,iitovp(n,f),NULL);
}

void initialize_g_cb_ui(const int g)
{
	(void) g;
}

static int shrinkage_idle(void *arg)
{
	const int n = vptoii_n(arg);
	const int shrinkage = vptoii_i(arg);
	if(shrinkage)
	{
		const uint32_t current_allocation_size = torx_allocation_len(t_peer[n].t_message + t_peer[n].pointer_location); // XXX Note this shift
		const size_t current_shift = (size_t)abs(shrinkage);
		if(shrinkage > 0) // We shift everything forward
			t_peer[n].t_message = (struct t_message_list*)torx_realloc_shift(t_peer[n].t_message + t_peer[n].pointer_location, current_allocation_size - sizeof(struct t_message_list) *current_shift,1) - t_peer[n].pointer_location - current_shift;
		else
			t_peer[n].t_message = (struct t_message_list*)torx_realloc(t_peer[n].t_message + t_peer[n].pointer_location, current_allocation_size - sizeof(struct t_message_list) *current_shift) - t_peer[n].pointer_location;
	}
	return 0;
}

void shrinkage_cb_ui(const int n,const int shrinkage)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,shrinkage_idle,iitovp(n,shrinkage),NULL);
}

static int expand_file_struc_idle(void *arg)
{
	const int n = vptoii_n(arg);
//	const int f = vptoii_i(arg);
	const uint32_t current_allocation_size = torx_allocation_len(t_peer[n].t_file);
	t_peer[n].t_file = torx_realloc(t_peer[n].t_file,current_allocation_size + sizeof(struct t_file_list) *10);
	return 0;
}

void expand_file_struc_cb_ui(const int n,const int f)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,expand_file_struc_idle,iitovp(n,f),NULL);
}

static int expand_message_struc_idle(void *arg)
{ // XXX DO NOT DELETE XXX
	const int n = vptoii_n(arg);
	const int i = vptoii_i(arg);
	const uint32_t current_allocation_size = torx_allocation_len(t_peer[n].t_message + t_peer[n].pointer_location);
	int current_shift = 0;
	if(i < 0)
	{
		current_shift = -10;
		t_peer[n].t_message = (struct t_message_list*)torx_realloc_shift(t_peer[n].t_message + t_peer[n].pointer_location,current_allocation_size + sizeof(struct t_message_list) *10,1) - t_peer[n].pointer_location - current_shift;
	}
	else
		t_peer[n].t_message = (struct t_message_list*)torx_realloc(t_peer[n].t_message + t_peer[n].pointer_location,current_allocation_size + sizeof(struct t_message_list) *10) - t_peer[n].pointer_location;
	t_peer[n].pointer_location += current_shift;
	return 0;
}

void expand_message_struc_cb_ui(const int n,const int i)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,expand_message_struc_idle,iitovp(n,i),NULL);
}

static int expand_peer_struc_idle(void *arg)
{
	(void) arg;
	const uint32_t current_allocation_size = torx_allocation_len(t_peer);
	t_peer = torx_realloc(t_peer,current_allocation_size + sizeof(struct t_peer_list) *10);
	return 0;
}

void expand_peer_struc_cb_ui(const int n)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,expand_peer_struc_idle,itovp(n),NULL);
}

void expand_group_struc_cb_ui(const int g)
{
	(void) g;
}

static gboolean write_test(const char *path)
{ // Returns TRUE if path (directory) is writable
	char temp_file_path[PATH_MAX];
	snprintf(temp_file_path, sizeof(temp_file_path), "%s%cjgIYZZHLdU9gCKud1VxptmJlH3zWd0bA",path,platform_slash); // Random string must not clash with any file on user's device
	FILE *file = fopen(temp_file_path, "wb");
	if (file == NULL)
	{
		error_simple(0,"Directory is not writable. Choose a different directory.");
		return FALSE; // not writable
	}
	fclose(file); file = NULL;
	remove(temp_file_path);
	return TRUE; // writable
}

static inline int display_notification_idle(void *arg)
{
	struct notification *notification = (struct notification*) arg;
	GNotification *gnotification;
	if(notification->heading)
		gnotification = g_notification_new(notification->heading);
	else
		gnotification = g_notification_new("");
	if(notification->message)
		g_notification_set_body (gnotification,notification->message);

//	GIcon *icon_logo = g_file_icon_new (NULL); // TODO creating anull icon is a work-around to force the use of the icon registered with DBUS by .desktop

/* Resource attempt 1, wont work definately
	GFile *file = g_file_new_for_uri("resource:///org/torx/gtk4/other/scalable/apps/logo-torx-symbolic.svg");
	GIcon *icon_logo = g_file_icon_new (file); */

/* Gbytes attempt 1, fail
	GFile *file = g_file_new_for_uri("resource:///org/torx/gtk4/other/scalable/apps/logo-torx-symbolic.svg");	// SHOULD WORK
	GBytes *bytes_file = g_file_load_bytes(file,NULL,NULL,NULL); // get gbytes from a gfile
	GIcon *icon_logo = g_bytes_icon_new (bytes_file); */

/* Gbytes attempt 2, fail
	char ** contents = {0};
	gsize size;
	g_file_load_contents (file,NULL,contents,&size,NULL,NULL);
	GBytes *bytes_file = g_bytes_new_take (contents,size);
	GIcon *icon_logo = g_bytes_icon_new (bytes_file); */

/* XXX Functional but not portable  XXX */
	GFile *file = g_file_new_for_path(FILENAME_ICON);		// WORKS
	GIcon *icon_logo = g_file_icon_new (file);

//	GBytes *bytes = g_resources_lookup_data ("/org/torx/gtk4/other/scalable/apps/logo-torx-symbolic.svg",G_RESOURCE_LOOKUP_FLAGS_NONE,NULL);
//	GIcon *icon_logo = g_bytes_icon_new (bytes);

//	GIcon *icon_logo = g_themed_icon_new(DBUS_TITLE); // is known to work, if we place icons in the right place???

/*	TODO 2024/03/10: Utilize the following to write logo_data to disk if the .svg doesn't already exist on disk, then utilize that disk location as DEFAULT_PATH_ICON
	TODO Note: requires #include "other/scalable/apps/logo_torx.h"	*/
/*	const size_t logo_len = b64_decoded_size(LOGO_B64);
	unsigned char *logo_data = torx_insecure_malloc(logo_len);
	if(b64_decode(logo_data,logo_len,LOGO_B64) != logo_len)
	{
		error_simple(0,"Issue with b64_decode. Report this.");
		return -1;
	}
	GBytes* bytes = g_bytes_new_static(logo_data,logo_len);
	GIcon *icon_logo = g_bytes_icon_new(bytes);	*/

	g_notification_set_icon (gnotification, G_ICON(icon_logo));
//	g_notification_set_icon (gnotification, G_ICON(texture_logo));
	g_application_send_notification (G_APPLICATION (gtk_application_gtk4), NULL, gnotification); // was g_application_gtk4
	g_object_unref (icon_logo);
	g_object_unref (gnotification);
	torx_free((void*)&notification->heading);
	torx_free((void*)&notification->message);
	torx_free((void*)&arg);
	return 0;
}

static void ui_notify(const char *heading, const char *message)
{ // Safe to run directly from _cb (threadsafe)
	if(!heading && !message)
		return;
	struct notification *notification = torx_insecure_malloc(sizeof(struct notification));
	if(heading)
	{
		const size_t len = strlen(heading);
		notification->heading = torx_secure_malloc(len+1);
		snprintf(notification->heading,len+1,"%s",heading);
	}
	else
		notification->heading = NULL;
	if(message)
	{
		const size_t len = strlen(message);
		notification->message = torx_secure_malloc(len+1);
		snprintf(notification->message,len+1,"%s",message);
	}
	else
		notification->message = NULL;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,display_notification_idle,notification,NULL);
}

static int ui_populate_list(void *arg)
{ // Note: since we already have arranged lists of n provided by refined_list, we might not need treestore ? TODO XXX (its being depreciated anyway, need to replace it)
	const uint8_t list = (uint8_t)vptoi(arg);
	treeview_n = -1;
	int len = 0;
	int *array = refined_list(&len,list,ENUM_STATUS_PENDING,NULL); // last arg not being used except with ctrl
	GListStore *list_store;
	if(list == ENUM_OWNER_CTRL) // Unconfirmed Mult requests (incoming requests)
		list_store = t_main.treestore_incoming;
	else if(list == ENUM_OWNER_PEER) // Outgoing requests list
		list_store = t_main.treestore_outgoing;
	else if(list == ENUM_OWNER_MULT)
		list_store = t_main.treestore_mult;
	else if(list == ENUM_OWNER_SING)
		list_store = t_main.treestore_sing;
	else
	{
		error_printf(0,"Requested to populate unknown list: %u. Report this.",list);
		breakpoint();
		return -1;
	}
	if(list_store && G_IS_LIST_STORE(list_store)) // TODO 2025/04/29 Hit segfault on G_IS_LIST_STORE when generating a SING, adding it, and then generating a MULT at approximately the same time of SING deletion. Possible race condition???
	{
		g_list_store_remove_all(list_store);
		for(int pos = 0 ; pos < len ; pos++) // or len if starting from other direction, then count down instead of up
		{
			const int n = array[pos];
			g_list_store_append(list_store,int_pair_new(n,list,-1,-1));
		}
	}
	torx_free((void*)&array);
	return 0;
}

static int transfer_progress_idle(void *arg)
{ // NOTE: this will not update time left if the transfer totally stalls/stops or peer goes offline.
	struct progress *progress = (struct progress*) arg; // Casting passed struct
	const int n = progress->n;
	const int f = progress->f;
	const uint64_t transferred = progress->transferred;
	torx_free((void*)&progress);
	int g = -1;
	int group_n = -1;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_PEER)
	{
		g = set_g(n,NULL);
		group_n = getter_group_int(g,offsetof(struct group_list,n));
	}
	if((global_n != n && (g == -1 || global_n != group_n)) || !t_peer[n].t_file[f].progress_bar || !GTK_IS_WIDGET(t_peer[n].t_file[f].progress_bar))
		return 0; // should check if visible? Note: We null progress_bar as mitigation to segfaults here. If segfault, null progress bar some more.
	const uint64_t size = getter_uint64(n,INT_MIN,f,offsetof(struct file_list,size));
	char *file_path = getter_string(NULL,n,INT_MIN,f,offsetof(struct file_list,file_path));
	char *file_size_text = file_progress_string(n,f);
	gtk_label_set_text(GTK_LABEL(t_peer[n].t_file[f].file_size),file_size_text);
	torx_free((void*)&file_size_text);
	double fraction = 0;
	if(size > 0)
		fraction = ((double)transferred*1.000/(double)size);
	const int file_status = file_status_get(n,f);
	if(t_peer[n].t_file[f].previously_completed == 0 && (file_status == ENUM_FILE_INACTIVE_COMPLETE || transferred == size))
	{ // Ensure that we didn't notify of completion already (relevant to GROUP_CTRL)
		t_peer[n].t_file[f].previously_completed = 1;
		ui_print_message(t_peer[n].t_file[f].n,t_peer[n].t_file[f].i,2);
		if(t_peer[n].mute == 0)
		{
			const uint8_t stat = getter_uint8(t_peer[n].t_file[f].n,t_peer[n].t_file[f].i,-1,offsetof(struct message_list,stat));
			if(stat == ENUM_MESSAGE_RECV)
			{ // Notify of completion only if the file was originally inbound
				char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
				char heading[ARBITRARY_ARRAY_SIZE]; // zero'd
				snprintf(heading,sizeof(heading),"%s: %s",text_transfer_completed,peernick);
				torx_free((void*)&peernick);
				ui_notify(heading,file_path);
				sodium_memzero(heading,sizeof(heading));
			}
		}
	}
	if(t_peer[n].t_file[f].progress_bar)
	{ // Sanity check
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(t_peer[n].t_file[f].progress_bar),fraction);
		if(file_status != ENUM_FILE_INACTIVE_AWAITING_ACCEPTANCE_INBOUND && file_status != ENUM_FILE_INACTIVE_CANCELLED)
			gtk_widget_set_visible(t_peer[n].t_file[f].progress_bar,TRUE);
	}
	if(file_status == ENUM_FILE_INACTIVE_COMPLETE && is_image_file(file_path))
		ui_print_message(t_peer[n].t_file[f].n,t_peer[n].t_file[f].i,2); // rebuild message to display image. Do this last.
	torx_free((void*)&file_path);
	return 0;
}

void transfer_progress_cb_ui(const int n,const int f,const uint64_t transferred)
{ // GUI Callback. // This is called every packet on a file transfer. Packets are PACKET_LEN-10 in size, so 488 (as of 2022/08/19, may be changed to accomodate sequencing)
	struct progress *progress = torx_insecure_malloc(sizeof(struct progress));
	progress->n = n;
	progress->f = f;
	progress->transferred = transferred;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,transfer_progress_idle,progress,NULL);
}

static int change_password_idle(void *arg)
{
	const int return_value = vptoi(arg);
	if(t_main.window != window_change_password)
		return 0;
//	gtk_widget_set_opacity(t_main.button_change_password_continue,1.0);
	gtk_widget_set_opacity(t_main.button_change_password,1.0);
	if(return_value == 0 || return_value == -1)
	{
		gtk_editable_delete_text(GTK_EDITABLE(t_main.pass_old),0,-1);
		gtk_editable_delete_text(GTK_EDITABLE(t_main.pass_new),0,-1);
		gtk_editable_delete_text(GTK_EDITABLE(t_main.pass_verify),0,-1);
		ui_input_new(t_main.pass_old);
		ui_input_new(t_main.pass_new);
		ui_input_new(t_main.pass_verify);
	}
	else if(return_value == 1)
	{
		gtk_editable_delete_text(GTK_EDITABLE(t_main.pass_old),0,-1);
		ui_input_bad(t_main.pass_old);
	}
	else if(return_value == 2)
	{
		gtk_editable_delete_text(GTK_EDITABLE(t_main.pass_new),0,-1);
		gtk_editable_delete_text(GTK_EDITABLE(t_main.pass_verify),0,-1);
		ui_input_bad(t_main.pass_new);
		ui_input_bad(t_main.pass_verify);
	}
	return 0;
}

static void ui_set_image_lock(const int n)
{
	if(n != global_n)
		return;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(texture_logo));
		const char *identifier;
		char onion[56+1];
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		{
			identifier = text_torxid;
			getter_array(&onion,52+1,n,INT_MIN,-1,offsetof(struct peer_list,torxid));
		}
		else
		{
			identifier = text_onionid;
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,onion));
		}
		const int g = set_g(n,NULL);
		pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
		char *groupid = b64_encode(group[g].id,GROUP_ID_SIZE);
		pthread_rwlock_unlock(&mutex_expand_group); // 🟩
		char tooltip[256];
		snprintf(tooltip,sizeof(tooltip),"%s: %s\n%s: %s",text_groupid,groupid,identifier,onion);
		gtk_widget_set_tooltip_text(t_main.image_header,tooltip);
		sodium_memzero(onion,sizeof(onion));
		sodium_memzero(tooltip,sizeof(tooltip));
		torx_free((void*)&groupid);
	}
	else
	{
		if(status == ENUM_STATUS_BLOCKED) // Handle blocked first
		{
			gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(lock_red));
			gtk_widget_set_tooltip_text(t_main.image_header,text_tooltip_image_header_0);
		}
		else
		{
			const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,sendfd_connected));
			const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,recvfd_connected));
			if(sendfd_connected > 0 && recvfd_connected > 0) // https://docs.gtk.org/Pango/enum.Weight.html
			{
				gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(lock_green));
				gtk_widget_set_tooltip_text(t_main.image_header,text_tooltip_image_header_1);
			}
			else if(sendfd_connected > 0 && recvfd_connected < 1)
			{  // This occurs when our proxy doesn't realize it is broken yet (common)
				gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(lock_yellow));
				gtk_widget_set_tooltip_text(t_main.image_header,text_tooltip_image_header_2);
			}
			else if(sendfd_connected < 1 && recvfd_connected > 0)
			{
				gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(lock_orange));
				gtk_widget_set_tooltip_text(t_main.image_header,text_tooltip_image_header_3);
			}
			else
			{
				gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(lock_grey));
				gtk_widget_set_tooltip_text(t_main.image_header,text_tooltip_image_header_4);
			}
		}
	}
}

static void ui_set_last_seen(const int n)
{
	if(n != global_n)
		return;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL); // just looking up existing
		const uint32_t g_peercount = getter_group_uint32(g,offsetof(struct group_list,peercount));
		char peer_count[256];
		snprintf(peer_count,sizeof(peer_count),"%s: %d %s %u",text_status_online,group_online(g),text_of,g_peercount);
		gtk_label_set_text(GTK_LABEL(t_main.last_online),peer_count);
		return;
	}
	const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,sendfd_connected));
	const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,recvfd_connected));
	if(sendfd_connected > 0 && recvfd_connected > 0)
	{
		gtk_label_set_text(GTK_LABEL(t_main.last_online),text_status_online);
		return;
	}
	char last_online_text[256];
	const time_t last_seen = getter_time(n,INT_MIN,-1,offsetof(struct peer_list,last_seen));
	if(last_seen > 0)
	{
		struct tm *info = localtime(&last_seen);
		char timebuffer[20] = {0};
		strftime(timebuffer,20,"%Y/%m/%d %H:%M:%S",info);
		snprintf(last_online_text,sizeof(last_online_text),"%s: %s",text_status_last_seen,timebuffer);
	}
	else
		snprintf(last_online_text,sizeof(last_online_text),"%s: %s",text_status_last_seen,text_status_never);
	gtk_label_set_text(GTK_LABEL(t_main.last_online),last_online_text);
}

void change_password_cb_ui(const int value)
{ // GUI Callb	char *heading = "redrawing";//peer [n]. peernick;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,change_password_idle,itovp(value),NULL);
}

static int incoming_friend_request_idle(void *arg)
{
	totalIncoming++;
	ui_decorate_panel_left_top();
	ui_populate_list(arg);
	return 0;
}

void incoming_friend_request_cb_ui(const int n)
{ // GUI Callback
	char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
	ui_notify(peernick,text_new_friend);
	torx_free((void*)&peernick);
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,incoming_friend_request_idle,itovp(ENUM_OWNER_CTRL),NULL);
}

static int onion_deleted_idle(void *arg)
{
	const int n = vptoii_n(arg);
	const int owner = vptoii_i(arg);
	t_peer[n].unread = 0;
	t_peer[n].mute = 0;
	t_peer[n].pm_n = -1;
	t_peer[n].edit_n = -1;
	t_peer[n].edit_i = INT_MIN;
	if(t_peer[n].buffer_write)
		t_peer[n].buffer_write = NULL; // g_free(t_peer[n].buffer_write); // Causes issues when sending a kill. Do not g_free before NULLing.

	for(uint32_t count = torx_allocation_len(t_peer[n].t_cache_info.audio_cache)/sizeof(unsigned char *); count ; ) // do not change logic without thinking
		torx_free((void*)&t_peer[n].t_cache_info.audio_cache[--count]); // clear out all unplayed audio data
	torx_free((void*)&t_peer[n].t_cache_info.audio_cache);
	torx_free((void*)&t_peer[n].t_cache_info.audio_time);
	torx_free((void*)&t_peer[n].t_cache_info.audio_nstime);
	t_peer[n].t_cache_info.last_played_time = 0;
	t_peer[n].t_cache_info.last_played_nstime = 0;
	if(t_peer[n].t_cache_info.stream_pipeline)
	{
		gst_object_unref(t_peer[n].t_cache_info.stream_pipeline);
		t_peer[n].t_cache_info.stream_pipeline = NULL;
	}
	t_peer[n].t_cache_info.playing = 0;

	t_peer[n].pointer_location = -10;
	t_peer[n].t_message = (struct t_message_list *)torx_realloc(t_peer[n].t_message + t_peer[n].pointer_location,sizeof(struct t_message_list) *21) - t_peer[n].pointer_location; // XXX Note this shift
	t_peer[n].t_file = torx_realloc(t_peer[n].t_file,sizeof(struct t_file_list) *11);
	for(int i = -10 ; i <= 10 ; i++) // re-initialize t_message
		initialize_i_idle(iitovp(n,i)); // TODO Note: this doesn't g_free things if they aren't already NULL (neither did the realloc above). We maybe should if(stuff) g_free(stuff) before the realloc.
	for(int f = 0 ; f <= 10 ; f++) // re-initialize t_file
		initialize_f_idle(iitovp(n,f)); // TODO Note: this doesn't g_free things if they aren't already NULL (neither did the realloc above). We maybe should if(stuff) g_free(stuff) before the realloc.
	initialize_peer_call_list(n);
	sodium_memzero(t_peer[n].stickers_requested,sizeof(t_peer[n].stickers_requested));
	if(generated_n == n)
	{
		generated_n = -1;
		if(t_main.window == window_main)
		{
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,"",-1);
			gtk_widget_set_visible(t_main.generated_qr_onion,FALSE);
			gtk_widget_set_visible(t_main.button_copy_generated_qr,FALSE);
			gtk_widget_set_visible(t_main.button_save_generated_qr,FALSE);
		}
	}
	if(owner == ENUM_OWNER_CTRL)
		ui_populate_peers(itovp(0));
	else if(owner == 20)
	{
		ui_populate_list(itovp(ENUM_OWNER_SING));
		ui_notify("",text_spoiled);
	}
	else if(owner != ENUM_OWNER_GROUP_PEER && owner != ENUM_OWNER_GROUP_CTRL)
		ui_populate_list(itovp(owner));
	else
		ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
/*	else if(t_main.window == window_main && owner == ENUM_OWNER_SING)
	{ // Remove existing visible SING if it is no longer in peer struct ( successful handshake just occured )
		GtkTextIter start, end = {0};
		gtk_text_buffer_get_bounds(t_main.textbuffer_generated_onion, &start, &end);
		char *buffer = gtk_text_buffer_get_text(t_main.textbuffer_generated_onion, &start, &end, FALSE);
		const int n = set_n(buffer);
		if(strstr(peer[n].onion,buffer) == NULL)
		{
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,"",-1);
			gtk_widget_set_visible(t_main.generated_qr_onion,FALSE);
			gtk_widget_set_visible(t_main.button_copy_generated_qr,FALSE);
			gtk_widget_set_visible(t_main.button_save_generated_qr,FALSE);
		}
		sodium_memzero(buffer,strlen(buffer));
		g_free(buffer);
		buffer = NULL;
	} */
	if(global_n == n)
		ui_go_back(itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	return 0;
}

void onion_deleted_cb_ui(const uint8_t owner,const int n)
{ // GUI Callback, // array necessary because owner about to be zero'd
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,onion_deleted_idle,iitovp(n,owner),NULL);
}

static int peer_online_idle(void *arg)
{
	const int n = vptoi(arg);
	if(n < 0)
	{
		error_simple(0,"Error. Peer_online_cb called on invalid n. Report this.");
		breakpoint();
		return 0;
	}
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	int group_n = -1;
	if(owner == ENUM_OWNER_GROUP_PEER)
	{
		const int g = set_g(n,NULL);
		group_n = getter_group_int(g,offsetof(struct group_list,n));
		if(group_n < 0)
		{
			error_simple(0,"Print_online_idle called set_g on a GROUP_PEER and the group_n is -1. Report this.");
			breakpoint();
			return 0;
		}
		ui_set_last_seen(group_n);
	}
	else
	{
		ui_populate_peers(itovp(ENUM_STATUS_FRIEND));
		ui_set_image_lock(n);
		ui_set_last_seen(n);
		const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,sendfd_connected));
		const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,recvfd_connected));
		if((owner != ENUM_OWNER_GROUP_PEER || t_peer[group_n].mute == 0) && t_peer[n].mute == 0 && sendfd_connected > 0 && recvfd_connected > 0)
		{
			char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			ui_notify(peernick,text_online);
			torx_free((void*)&peernick);
		}
	}
	return 0;
}

void peer_online_cb_ui(const int n)
{ // GUI Callback from socks.c (successful send_init)
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,peer_online_idle,itovp(n),NULL);
}

static int peer_offline_idle(void *arg)
{ // Mostly a duplicate of peer_online_idle and the contents of that exists in a 3rd place
	const int n = vptoi(arg);
	const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,sendfd_connected));
	const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,recvfd_connected));
	const uint8_t online = recvfd_connected + sendfd_connected;
	if(!online) // Peer is completely offline
		call_peer_leaving_all_except(n,-1,-1);
	return peer_online_idle(arg); // XXX
}

void peer_offline_cb_ui(const int n)
{ // GUI Callback from socks.c (successful send_init)
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,peer_offline_idle,itovp(n),NULL);
}

static int peer_new_idle(void *arg)
{ // N is passed but we currently don't use it
	const int n = vptoi(arg);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_PEER)
	{
		const int g = set_g(n,NULL);
		const int group_n = getter_group_int(g,offsetof(struct group_list,n));
		ui_set_last_seen(group_n);
	}
	ui_populate_peers(itovp(ENUM_STATUS_FRIEND));
	return 0;
}

void peer_new_cb_ui(const int n)
{ // GUI Callback from from client_init.c and libevent.c ( new friend, it seems )
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,peer_new_idle,itovp(n),NULL);
}

static int totalIncoming_increase_idle(void *arg)
{
	(void) arg;
	totalIncoming++;
	// TODO rebuild ui_decorate_panel_left_top(); // TODO not doing because expensive due to frequent run on startup. may need to do or simplify. We already do it in flutter, where it is less costly. Low priority, nearly non-issue.
	return 0;
}

void peer_loaded_cb_ui(const int n)
{ // NOTE: this runs frequently on startup
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
		g_idle_add_full(G_PRIORITY_HIGH_IDLE,ui_populate_peers,itovp(ENUM_STATUS_GROUP_CTRL),NULL);
	else
	{
		const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
		if(owner == ENUM_OWNER_CTRL && status == ENUM_STATUS_PENDING)
			g_idle_add_full(G_PRIORITY_HIGH_IDLE,totalIncoming_increase_idle,NULL,NULL);
		g_idle_add_full(G_PRIORITY_HIGH_IDLE,ui_populate_peers,itovp(status),NULL);
	}
}

static void ui_copy_qr(const void *arg)
{ // UTF8 only, no PNG
	if(t_main.popover_qr && GTK_IS_WIDGET(t_main.popover_qr))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_qr)); // TODO throws GTK error if not existing. should check first
	const int n = vptoi(arg);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	struct qr_data *qr_data;
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL); // just looking up existing
		unsigned char id[GROUP_ID_SIZE];
		pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
		memcpy(id,group[g].id,sizeof(id));
		pthread_rwlock_unlock(&mutex_expand_group); // 🟩
		char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
		sodium_memzero(id,sizeof(id));
		qr_data = qr_bool(group_id_encoded,1);
		torx_free((void*)&group_id_encoded);
	}
	else
	{
		char torxid[52+1];
		getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,offsetof(struct peer_list,torxid));
		qr_data = qr_bool(torxid,1);
		sodium_memzero(torxid,sizeof(torxid));
	}
	char *qr = qr_utf8(qr_data);
	if(qr)
	{
		gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),qr);
		torx_free((void*)&qr);
	}
	torx_free((void*)&qr_data->data);
	torx_free((void*)&qr_data);
}

static void ui_save_qr_to_file(GtkFileDialog *dialog,GAsyncResult *res,const gpointer data)
{ // Saves QR to file using data from dialog. PNG only, no UTF8
	const int n = vptoi(data);
	GFile *chosen_path = gtk_file_dialog_save_finish(dialog,res,NULL);
	if(chosen_path)
	{
		struct qr_data *qr_data;
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_GROUP_CTRL)
		{
			const int g = set_g(n,NULL); // just looking up existing
			unsigned char id[GROUP_ID_SIZE];
			pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
			memcpy(id,group[g].id,sizeof(id));
			pthread_rwlock_unlock(&mutex_expand_group); // 🟩
			char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
			sodium_memzero(id,sizeof(id));
			qr_data = qr_bool(group_id_encoded,8);
			torx_free((void*)&group_id_encoded);
		}
		else
		{
			char torxid[52+1];
			getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,offsetof(struct peer_list,torxid));
			qr_data = qr_bool(torxid,8);
			sodium_memzero(torxid,sizeof(torxid));
		}
		size_t png_size = 0;
		void* png_data = return_png(&png_size,qr_data);
		char *file_path = g_file_get_path(chosen_path); // free'd
		write_bytes(file_path,png_data,png_size);
	//	printf("Checkpoint png_size: %lu qr_data->size_allocated: %lu saving to: %s\n",png_size,qr_data->size_allocated,file_path);
		g_free(file_path); file_path = NULL;
	//	sodium_memzero(png_data,png_size);
		torx_free((void*)&png_data);

		torx_free((void*)&qr_data->data);
		torx_free((void*)&qr_data);
		g_object_unref (chosen_path);
	}
}

static void ui_save_qr(void *arg)
{ /* Opens a dialog for saving QR to file */
	if(t_main.popover_qr && GTK_IS_WIDGET(t_main.popover_qr))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_qr));

	GtkFileDialog *dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_modal(dialog,TRUE); // block interaction with UI
	gtk_file_dialog_set_title(dialog,text_save_qr); // also: gtk_file_dialog_set_filters
	gtk_file_dialog_set_initial_name(dialog,"qr.png");
	gtk_file_dialog_save (dialog,GTK_WINDOW(t_main.main_window),NULL,(GAsyncReadyCallback)ui_save_qr_to_file, arg);
}

static int onion_ready_idle(void *arg)
{
	const int n = vptoi(arg);
	generated_n = n;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(t_main.window == window_main)
	{
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_generate_peernick)),0,-1); // Clear peernick
		char torxid[52+1];
		getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,offsetof(struct peer_list,torxid));
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,torxid,-1);
		else
		{
			char onion[56+1];
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,onion));
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,onion,-1);
			sodium_memzero(onion,sizeof(onion));
		}
		struct qr_data *qr_data;
		if(QR_IS_PNG)
		{
			qr_data = qr_bool(torxid,8);
			size_t png_size = 0;
			void* png_data = return_png(&png_size,qr_data);
			GBytes *bytes = g_bytes_new(png_data,png_size);
			GdkTexture *texture = gdk_texture_new_from_bytes(bytes,NULL);
			gtk_image_set_from_paintable(GTK_IMAGE(t_main.generated_qr_onion),GDK_PAINTABLE(texture));
			torx_free((void*)&png_data);
		}
		else
		{
			qr_data = qr_bool(torxid,1);
			char *qr = qr_utf8(qr_data);
			if(qr)
			{
				gtk_label_set_text(GTK_LABEL(t_main.generated_qr_onion),qr);
				torx_free((void*)&qr);
			}
		}
		g_signal_connect_swapped(t_main.button_copy_generated_qr, "clicked", G_CALLBACK(ui_copy_qr),itovp(n));
		g_signal_connect_swapped(t_main.button_save_generated_qr, "clicked", G_CALLBACK(ui_save_qr),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_set_visible(t_main.generated_qr_onion,TRUE);
		gtk_widget_set_visible(t_main.button_copy_generated_qr,TRUE);
		gtk_widget_set_visible(t_main.button_save_generated_qr,TRUE);
	//	sodium_memzero(qr,qr_data->size_allocated); 
		sodium_memzero(torxid,sizeof(torxid));
		torx_free((void*)&qr_data->data);
		torx_free((void*)&qr_data);
	}
	else if(t_main.window == window_home)
		ui_populate_list(itovp(owner));
	return 0;
}

void onion_ready_cb_ui(const int n)
{ // GUI Callback. Should provide the nick and onion that is ready.
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,onion_ready_idle,itovp(n),NULL);
}

static GdkPaintable *gif_static_new_from_data(const unsigned char *data,const size_t data_len)
{
	if(!data || !data_len)
		return NULL;
	GBytes* bytes = g_bytes_new_static(data,data_len);
	GdkTexture *texture = gdk_texture_new_from_bytes(bytes,NULL);
	return GDK_PAINTABLE(texture);
}

static GdkPaintable *gif_animated_new_from_data(const unsigned char *data,const size_t data_len)
{
	if(!data || !data_len)
		return NULL;
	struct gif_data gif_data;
	gif_data.data = data;
	gif_data.data_len = data_len;

	GdkPaintable *paintable = g_object_new(GTK_MAKES_ME_RAGE,"gif-data",&gif_data,NULL);
//	g_object_unref(paintable);
	return paintable;
}

static int ui_sticker_set(const unsigned char checksum[CHECKSUM_BIN_LEN])
{
	if(!checksum)
	{
		error_simple(0,"Null passed to set_sticker. Coding error. Report this.");
		breakpoint();
		return -1;
	}
	int s = 0;
	while(s < STICKERS_LIST_SIZE && sticker[s].paintable_static && memcmp(sticker[s].checksum,checksum,CHECKSUM_BIN_LEN))
		s++;
	if(s == STICKERS_LIST_SIZE)
	{
		error_simple(0,"Hit STICKERS_LIST_SIZE, cannot search more stickers.");
		breakpoint();
		return -1;
	}
	else if (sticker[s].paintable_static == NULL)
		return -1; // Does not exist yet
	return s;
}

static void ui_sticker_save(const void *arg)
{
	const int s = vptoi(arg);
	if(s < 0 || !sticker[s].data || !sticker[s].data_len)
	{
		error_simple(0,"Cannot save sticker. No data. Coding error. Report this.");
		breakpoint();
		return;
	}
	char setting_name[256];
	char *encoded = b64_encode(sticker[s].checksum,sizeof(sticker[s].checksum));
	snprintf(setting_name,sizeof(setting_name),"sticker-gif-%s",encoded);
	torx_free((void*)&encoded);
	sql_setting(0,-1,setting_name,(const char*)sticker[s].data,sticker[s].data_len);
	sodium_memzero(setting_name,sizeof(setting_name));
	torx_free((void*)&sticker[s].data);
	sticker[s].data_len = 0;
	if(t_main.popover_sticker && GTK_IS_WIDGET(t_main.popover_sticker) && sticker[s].outer_box)
		gtk_widget_remove_css_class(sticker[s].outer_box,"unsaved_sticker");
}

static int ui_sticker_register(const unsigned char *data,const size_t data_len)
{ // If utilizing, be sure to check return for if(s > -1 && sticker[s].paintable_animated != NULL)
	unsigned char checksum[CHECKSUM_BIN_LEN];
	if(!data || !data_len || b3sum_bin(checksum,NULL,data,0,data_len) != data_len)
	{
		sodium_memzero(checksum,sizeof(checksum));
		return -1; // bug
	}
	int s = 0;
	while(s < STICKERS_LIST_SIZE && sticker[s].paintable_static && memcmp(sticker[s].checksum,checksum,CHECKSUM_BIN_LEN))
		s++;
	if(s == STICKERS_LIST_SIZE)
	{
		error_simple(0,"Hit STICKERS_LIST_SIZE, cannot add more stickers.");
		sodium_memzero(checksum,sizeof(checksum));
		breakpoint();
		return -1; // bug
	}
	if(sticker[s].paintable_animated == NULL)
	{ // Register new sticker
		for(int iter = MAX_PEERS-1 ; iter > -1 ; iter--)
			sticker[s].peers[iter] = -1; // initialize as array of -1
		sticker[s].paintable_static = gif_static_new_from_data(data,data_len);
		sticker[s].paintable_animated = gif_animated_new_from_data(data,data_len);
		memcpy(sticker[s].checksum,checksum,sizeof(checksum));
	}
	sodium_memzero(checksum,sizeof(checksum));
	return s;
}

static void handle_chosen_file_and_restart_tor(GtkWidget *button,GFile *file,char **global_location,const char *name)
{ // TODO consider having a option to unset / reset default path
	char *path = g_file_get_path(file); // free'd
	if(!path)
		return;
	gtk_button_set_label ( GTK_BUTTON(button),path);
	const size_t len = strlen(path);
	pthread_rwlock_wrlock(&mutex_global_variable); // 🟥
	torx_free((void*)global_location);
	*global_location = torx_insecure_malloc(len+1);
	snprintf(*global_location,len+1,"%s",path);
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	sql_setting(1,-1,name,path,len);
	if(threadsafe_read_uint8(&mutex_global_variable,&keyed) && !strncmp(name,"tor_location",12)) // restart if running
		start_tor();
	g_free(path); path = NULL;
}

static void ui_on_choose_file(GtkFileDialog *dialog,GAsyncResult *res,GtkWidget *button)
{ // For selecting a file for reasons other than file sharing
	GFile *file = gtk_file_dialog_open_finish (dialog,res,NULL);
	if(file)
	{
		const char *name = gtk_widget_get_name(button);
		if(!strncmp(name,"tor_location",12))
			handle_chosen_file_and_restart_tor(button,file,&tor_location,name);
		else if(!strncmp(name,"snowflake_location",18))
			handle_chosen_file_and_restart_tor(button,file,&snowflake_location,name);
		else if(!strncmp(name,"lyrebird_location",17))
			handle_chosen_file_and_restart_tor(button,file,&lyrebird_location,name);
		else if(!strncmp(name,"conjure_location",16))
			handle_chosen_file_and_restart_tor(button,file,&conjure_location,name);
		else if(!strncmp(name,"custom_input_location",21))
		{
			t_main.custom_input_location = g_file_get_path(file); // TODO g_free
			char *path = g_file_get_path(file); // free'd
			char *privkey = custom_input_file(path);
			gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_custom_privkey)),privkey,-1);
			torx_free((void*)&privkey);
			g_free(path); path = NULL;
		}
		else if(!strncmp(name,"button_add_sticker",18))
		{
			char *path = g_file_get_path(file); // free'd
			if(!path)
				return;
			size_t data_len = 0;
			unsigned char *data = read_bytes(&data_len,path);
			const int s = ui_sticker_register(data,data_len);
			sticker[s].data = torx_secure_malloc(data_len);
			memcpy(sticker[s].data,data,data_len);
			sticker[s].data_len = data_len;
			ui_sticker_save(itovp(s));
			torx_free((void*)&data);
			g_free(path); path = NULL;
		}
// ??? what??	fprintf(stderr,"GTK bug where placeholder text is not overwritten. Should be fixed by publication time.\n"); // https://gitlab.gnome.org/GNOME/gtk/-/merge_requests/4875
	}
}

static void ui_on_choose_files(GtkFileDialog *dialog,GAsyncResult *res,const void *arg)
{ // For selecting file(s) for sending
	const int n = vptoi(arg); // DO NOT FREE ARG
	GListModel *file_list = gtk_file_dialog_open_multiple_finish (dialog,res,NULL);
	if(file_list)
	{
		guint z = 0;
        	for(GFile *file ; (file = g_list_model_get_item(file_list,z)) != NULL ; z++)
		{
			char *file_path = g_file_get_path(file); // free'd
			file_send(n,file_path);
			g_free(file_path); file_path = NULL;
		}
	}
}

static void ui_on_choose_download_dir(GtkFileDialog *dialog,GAsyncResult *res,const gpointer data)
{ // Choosing download_dir. Data is button.
	GFile *folder = gtk_file_dialog_select_folder_finish(dialog,res,NULL);
	if(folder)
	{ // Set
		char *folder_path = g_file_get_path(folder); // free'd (in most cases)
		if(folder_path == NULL || !write_test(folder_path)) // null has happened, stupid GTK
			return;
		const size_t len = strlen(folder_path);
		char *allocation = torx_secure_malloc(len+1);
		memcpy(allocation,folder_path,len+1);
		pthread_rwlock_wrlock(&mutex_global_variable); // 🟥
		torx_free((void*)&download_dir);
		download_dir = allocation;
		pthread_rwlock_unlock(&mutex_global_variable); // 🟩
		sql_setting(0,-1,"download_dir",folder_path,len);
		g_free(folder_path); folder_path = NULL;
	}
	else
	{ // Unset if cancelled
		pthread_rwlock_wrlock(&mutex_global_variable); // 🟥
		torx_free((void*)&download_dir);
		pthread_rwlock_unlock(&mutex_global_variable); // 🟩
		sql_delete_setting(0,-1,"download_dir");
	}
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	gtk_button_set_label (GTK_BUTTON(data),download_dir);
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
}

static void ui_on_choose_folder(GtkFileDialog *dialog,GAsyncResult *res,gpointer data)
{ // For selecting a folder for saving file
	if(data)
	{ // Choosing folder path
		const int n = vptoii_n(data);
		const int f = vptoii_i(data);
		GFile *folder = gtk_file_dialog_select_folder_finish(dialog,res,NULL);
		if(folder)
		{
			torx_read(n) // 🟧🟧🟧
			const uint8_t filename_exists = peer[n].file[f].filename ? 1 : 0;
			torx_unlock(n) // 🟩🟩🟩
			if(!filename_exists)
			{
				error_simple(0,"Null file name. Failed sanity check.");
				gtk_window_destroy(GTK_WINDOW(dialog));
				return;
			}
			char *folder_path = g_file_get_path(folder); // free'd (in most cases)
			if(folder_path == NULL || !write_test(folder_path)) // null has happened, stupid GTK
				return;
			torx_write(n) // 🟥🟥🟥
			torx_free((void*)&peer[n].file[f].file_path);
			const size_t maxlen = strlen(folder_path) + strlen(peer[n].file[f].filename) + 2;
			peer[n].file[f].file_path = torx_secure_malloc(maxlen);
			snprintf(peer[n].file[f].file_path,maxlen,"%s%c%s",folder_path,platform_slash,peer[n].file[f].filename);
			torx_unlock(n) // 🟩🟩🟩
			file_accept(n,f);
			g_free(folder_path); folder_path = NULL;
		}
	}
}

/*
static void ui_kill_children(GtkWidget *arg)
{ // Unparent children, not exactly killing, but ideally we would.
	printf("Checkpoint ui_kill_children\n");
	for(GtkWidget *child = gtk_widget_get_first_child(arg) ; child != NULL ; child = gtk_widget_get_first_child(arg))
	{
		printf("Checkpoint unparenting a plebian\n");
		gtk_widget_unparent(child);
	}
} */

/*static GdkPaintable * // does not work
pixbuf_paintable_new_from_resource (const char *path)
{
  return g_object_new (GDK_TYPE_PAINTABLE,
                       "resource-path", path,
                       NULL);
}*/

static void ui_sticker_delete(const gpointer *arg)
{
	const int s = vptoi(arg);
	if(s < 0)
		return; // should not happen
	char setting_name[256];
	char *encoded = b64_encode(sticker[s].checksum,sizeof(sticker[s].checksum));
	snprintf(setting_name,sizeof(setting_name),"sticker-gif-%s",encoded);
	torx_free((void*)&encoded);
	sql_delete_setting(0,-1,setting_name);
	sodium_memzero(setting_name,sizeof(setting_name));

	g_object_unref(sticker[s].paintable_animated); sticker[s].paintable_animated = NULL;
//	GdkPaintable *paintable_static; // DO NOT FREE upon delete UNTIL shutdown.
//	unsigned char checksum[CHECKSUM_BIN_LEN]; // should zero on shutdown, no need to zero when deleting?
//	int peers[MAX_PEERS];
	torx_free((void*)&sticker[s].data);
	sticker[s].data_len = 0;
	if(t_main.popover_sticker && GTK_IS_WIDGET(t_main.popover_sticker) && sticker[s].outer_box)
		gtk_widget_set_visible(sticker[s].outer_box,FALSE);
}

static void ui_sticker_send(gpointer *arg)
{
	const int s = vptoi(arg);
	if(s < 0 || sticker[s].paintable_animated == NULL)
		return;
	int g = -1;
	uint8_t g_invite_required = 0;
	const uint8_t owner = getter_uint8(global_n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		g = set_g(global_n,NULL);
		g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
	}
	int recipient_n = global_n;
	if(t_peer[global_n].pm_n > -1)
	{
		recipient_n = t_peer[global_n].pm_n;
		message_send(recipient_n,ENUM_PROTOCOL_STICKER_HASH_PRIVATE,sticker[s].checksum,(uint32_t)sizeof(sticker[s].checksum));
	}
	else if(owner == ENUM_OWNER_GROUP_CTRL && g_invite_required) // date && sign private group messages
		message_send(recipient_n,ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED,sticker[s].checksum,(uint32_t)sizeof(sticker[s].checksum));
	else // regular messages, private messages (in authenticated pipes), public messages in public groups (in authenticated pipes)
		message_send(recipient_n,ENUM_PROTOCOL_STICKER_HASH,sticker[s].checksum,(uint32_t)sizeof(sticker[s].checksum));
	// THE FOLLOWING IS IMPORTANT TO PREVENT FINGERPRINTING BY STICKER WALLET. XXX Note: this is in two places because reliability is better this way
	int iter = 0;
	while(iter < MAX_PEERS && sticker[s].peers[iter] != recipient_n && sticker[s].peers[iter] > -1)
		iter++;
	if(iter < MAX_PEERS && sticker[s].peers[iter] < 0) // Register a new recipient of sticker so that they can request data
		sticker[s].peers[iter] = recipient_n;
}

static inline GtkWidget *ui_sticker_box(GdkPaintable *paintable,const int square_size)
{ // Put sticker in an appropriate box of an appropriate size. XXX WARNING: we use this for gifs of all types, not just stickers. XXX
	if(!paintable || !GDK_IS_PIXBUF_ANIMATION(PIXBUF_PAINTABLE(paintable)->animation))
	{
		error_simple(0,"A gif is bunk. Message 2.");
		return NULL; // consider returning empty box on error. no, probably not good because then gaps in sticker chooser would occur.
	}
	const double height = gdk_pixbuf_animation_get_height(PIXBUF_PAINTABLE(paintable)->animation);
	if(height < 1)
		return NULL; // consider returning empty box on error. no, probably not good because then gaps in sticker chooser would occur.
	const double aspect_ratio = gdk_pixbuf_animation_get_width(PIXBUF_PAINTABLE(paintable)->animation) / height;
	GtkWidget *box;
	GtkWidget *sticker_image = gtk_picture_new_for_paintable(paintable); // alt: gtk_image_new_from_paintable
	if(aspect_ratio > 1)
	{ // wide sticker
		gtk_widget_set_size_request(sticker_image, square_size,(int)((double)square_size/aspect_ratio)); // x,y
		box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero); // necessary
	}
	else
	{ // tall or 1:1 sticker
		gtk_widget_set_size_request(sticker_image,(int)((double)square_size*aspect_ratio),square_size); // x,y
		box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_zero); // necessary
	}
	gtk_box_append(GTK_BOX(box), sticker_image);
	return box;
}

static void ui_sticker_chooser(GtkWidget *parent,const gpointer arg)
{ // XXX normally we make popovers global but for emoji chooser it seems unnecessary because all children are local
	const int n = vptoi(arg); // DO NOT FREE ARG
	t_main.popover_sticker = custom_popover_new(parent);
	gtk_popover_set_position (GTK_POPOVER(t_main.popover_sticker),GTK_POS_TOP); // helpful
	GtkWidget *box_popover_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box_popover_outer, "popover_inner");
	GtkWidget *scrolled_window_popover = gtk_scrolled_window_new();
	gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW (scrolled_window_popover),(size_sticker_small+size_spacing_three*2)*MAX_STICKER_COLUMNS);
	gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (scrolled_window_popover),(size_sticker_small+size_spacing_three*2)*MAX_STICKER_ROWS);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_popover),box_popover_outer);
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_sticker),scrolled_window_popover);

	GtkWidget *grid = gtk_grid_new();
//	gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
//	gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);
	int x = 0,y = 0;
	for(int s = 0; sticker[s].paintable_static; s++)
	{ // Attach stickers TODO have them only be animated while mouseover
		if(sticker[s].paintable_animated == NULL)
			continue; // was deleted
		GtkWidget *inner_box = ui_sticker_box(sticker[s].paintable_animated,size_sticker_small); // alt: sticker[s].paintable_static)
		if(inner_box)
		{ // verify sticker is valid
			sticker[s].outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
			gtk_widget_set_margin_top(inner_box, size_spacing_three);
			gtk_widget_set_margin_bottom(inner_box,size_spacing_three);
			gtk_widget_set_margin_start(inner_box,size_spacing_three);
			gtk_widget_set_margin_end(inner_box,size_spacing_three);
			if(sticker[s].data)
				gtk_widget_add_css_class(sticker[s].outer_box,"unsaved_sticker");
			gtk_box_append (GTK_BOX(sticker[s].outer_box), inner_box);

			GtkWidget *popover_substicker = custom_popover_new(sticker[s].outer_box);
			GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
			gtk_widget_add_css_class(box, "popover_inner");
			if(sticker[s].data)
				create_button(text_save,ui_sticker_save,itovp(s))
			create_button(text_delete,ui_sticker_delete,itovp(s))
			gtk_popover_set_child(GTK_POPOVER(popover_substicker),box);

			GtkGesture *long_press = gtk_gesture_long_press_new(); // NOTE: "cancelled" == single click, "pressed" == long press
			gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(long_press),LONG_PRESS_TIME);
			g_signal_connect_swapped(long_press, "cancelled", G_CALLBACK(ui_sticker_send),itovp(s)); // DO NOT FREE arg because this only gets passed ONCE.
			g_signal_connect_swapped(long_press, "pressed", G_CALLBACK(gtk_popover_popup),GTK_POPOVER(popover_substicker)); // DO NOT FREE arg because this only gets passed ONCE.

			gtk_widget_add_controller(sticker[s].outer_box, GTK_EVENT_CONTROLLER(long_press));

			gtk_grid_attach (GTK_GRID(grid),sticker[s].outer_box,x,y,1,1);
			if(x < MAX_STICKER_COLUMNS-1)
				x++;
			else
			{
				x = 0;
				y++;
			}
		}
	}
	gtk_grid_attach (GTK_GRID(grid),ui_button_generate(ENUM_BUTTON_ADD_STICKER,n),x,y,1,1); // The add button
	gtk_box_append(GTK_BOX(box_popover_outer), grid);
//	g_signal_connect_swapped(t_main.popover_sticker, "closed", G_CALLBACK (ui_clean_grid), grid); // XXX necessary or a warning occurs
	gtk_popover_popup(GTK_POPOVER(t_main.popover_sticker));
}

static inline void ui_button_determination(const int n)
{ // Must be fast as it runs on every keypress
	if(!t_main.button_send || n < 0)
		return; // Sanity check
	if(t_peer[n].buffer_write == NULL || !gtk_text_buffer_get_char_count(t_peer[n].buffer_write))
	{
		if(gtk_widget_get_visible(t_main.button_send)/* && t_peer[n].edit_n == -1*/) // TODO enabling this diverges from Flutter but hides some things that should be hidden (mic, attach, gif)
		{
			gtk_widget_set_visible(t_main.button_send,FALSE);
			if(vertical_mode)
			{
				gtk_widget_set_visible(t_main.button_keyboard_microphone,TRUE);
				gtk_widget_set_visible(t_main.button_sticker,TRUE);
				gtk_widget_set_visible(t_main.button_attach,TRUE);
			}
		}
	}
	else
	{
		if(!gtk_widget_get_visible(t_main.button_send))
		{
			gtk_widget_set_visible(t_main.button_send,TRUE);
			if(vertical_mode)
			{
				gtk_widget_set_visible(t_main.button_keyboard_microphone,FALSE);
				gtk_widget_set_visible(t_main.button_sticker,FALSE);
				gtk_widget_set_visible(t_main.button_attach,FALSE);
			}
		}
	}
}

static void ui_emoji_picked(const GtkEmojiChooser *chooser,const char *text,GtkText *self)
{
	(void) chooser;
	(void) self;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	if(!gtk_text_buffer_insert_interactive_at_cursor (buffer, text, -1,TRUE))
		gtk_widget_error_bell (GTK_WIDGET (t_main.write_message));
	ui_button_determination(global_n);
}

static void ui_choose_emoji(GtkWidget *parent,const gpointer arg)
{ // XXX normally we make popovers global but for emoji chooser it seems unnecessary because all children are local
	GtkWidget *chooser = gtk_emoji_chooser_new();
	gtk_widget_set_parent (chooser,parent);
	g_signal_connect(chooser, "emoji-picked", G_CALLBACK (ui_emoji_picked), arg); // DO NOT FREE arg because this only gets passed ONCE.
//	gtk_popover_set_position (GTK_POPOVER(chooser),GTK_POS_TOP); // helpful
	gtk_popover_set_autohide(GTK_POPOVER(chooser),TRUE);
//	g_signal_connect(chooser,"notify::destroy", G_CALLBACK(ui_kill_children), NULL);
	g_signal_connect(chooser, "closed", G_CALLBACK (gtk_widget_unparent), chooser); // XXX necessary or a warning occurs
	gtk_popover_popup(GTK_POPOVER(chooser));
}

static void ui_toggle_file(GtkGestureLongPress* self,gpointer data)
{ // Toggle File transfer (accept, pause/cancel)
	(void) self;
	if(!data)
		return;
	const int n = vptoii_n(data);
	const int f = vptoii_i(data);
	const int is_complete = file_is_complete(n,f);
	char *file_path = getter_string(NULL,n,INT_MIN,f,offsetof(struct file_list,file_path));
	if(is_complete)
	{ // NOTE: we have gtk_file_launcher_open_containing_folder in two places
		GFile *file = g_file_new_for_path(file_path);
		GtkFileLauncher *launcher = gtk_file_launcher_new (file);
	//	gtk_file_launcher_set_always_ask (launcher,TRUE); // doesn't do shit
	//	gtk_file_launcher_launch (launcher,GTK_WINDOW(t_main.main_window),NULL,NULL,NULL); // not good, it opens in default without asking
		gtk_file_launcher_open_containing_folder (launcher,GTK_WINDOW(t_main.main_window),NULL,NULL,NULL);
	}
	else if(file_path == NULL)
	{
		pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
		const char *download_dir_local = download_dir;
		pthread_rwlock_unlock(&mutex_global_variable); // 🟩
		if(file_path == NULL && download_dir_local == NULL)
		{ // this should only be for received files that are not started, otherwise should accept/pause/cancel via file_accept
			GtkFileDialog *dialog = gtk_file_dialog_new();
			gtk_file_dialog_set_modal(dialog,TRUE); // block interaction with UI
			gtk_file_dialog_set_title(dialog,text_choose_folder); // also: gtk_file_dialog_set_filters
			gtk_file_dialog_select_folder (dialog,GTK_WINDOW(t_main.main_window),NULL,(GAsyncReadyCallback)ui_on_choose_folder,iitovp(n,f));
		}
		else
		{
			error_printf(3,"Accepting file without asking for folder. To: %s",file_path);
			file_accept(n,f);
		}
	}
	torx_free((void*)&file_path);
}

static void ui_set_image_logging(GtkWidget *button,const int n)
{
	const int8_t log_messages = getter_int8(n,INT_MIN,-1,offsetof(struct peer_list,log_messages));
	if(log_messages == -1)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(logging_inactive)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(logging_inactive_light)));
		gtk_widget_set_tooltip_text(button,text_tooltip_logging_disabled);
	}
	else if(log_messages == 0)
	{
		if(threadsafe_read_uint8(&mutex_global_variable,&global_log_messages) == 1)
		{
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(globe_active)));
			gtk_widget_set_tooltip_text(button,text_tooltip_logging_global_on);
		}
		else if(global_theme == DARK_THEME)
		{
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(globe_inactive)));
			gtk_widget_set_tooltip_text(button,text_tooltip_logging_global_off);
		}
		else
		{
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(globe_inactive_light)));
			gtk_widget_set_tooltip_text(button,text_tooltip_logging_global_off);
		}
	}
	else if(log_messages == 1)
	{
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(logging_active)));
		gtk_widget_set_tooltip_text(button,text_tooltip_logging_enabled);
	}
}

static void ui_set_image_mute(GtkWidget *button,const int n)
{
	if(t_peer[n].mute == 1)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(bell_inactive)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(bell_inactive_light)));
		gtk_widget_set_tooltip_text(button,text_tooltip_notifications_off);
	}
	else
	{
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(bell_active)));
		gtk_widget_set_tooltip_text(button,text_tooltip_notifications_on);
	}
}

static void ui_set_image_block(GtkWidget *button,const int n)
{
	const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
	if(status == ENUM_STATUS_BLOCKED)
	{
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(block_active)));
		gtk_widget_set_tooltip_text(button,text_tooltip_blocked_on);
	}
	else if(global_theme == DARK_THEME)
	{
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(block_inactive)));
		gtk_widget_set_tooltip_text(button,text_tooltip_blocked_off);
	}
	else
	{
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(block_inactive_light)));
		gtk_widget_set_tooltip_text(button,text_tooltip_blocked_off);
	}
}

static void ui_go_back(void *arg)
{ // go back
	treeview_n = -1;
	const int n = vptoi(arg);
//	vertical_mode = 1;
	if(n > -1 && t_peer[n].unread > 0)
	{ // NOTE: we already do this when we select_changed, but we are doing it again in case main_window wasn't visible at that time. TODO Consider having this done when group_n > -1 when the window becomes re-visible
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
		if (owner == ENUM_OWNER_GROUP_CTRL)
			totalUnreadGroup -= t_peer[n].unread;
		else
			totalUnreadPeer -= t_peer[n].unread;
      		t_peer[n].unread = 0; // reset unread count for specific peer
	}
	if(n > -1 && t_peer[n].buffer_write != NULL)
	{ // Draft exists
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_GROUP_CTRL)
			ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
		else
		{
			const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
			ui_populate_peers(itovp(status));
		}
	}
	if(vertical_mode)
		t_main.window = window_auth; // not sure if this is ideal. we don't have an enum for chatlist
	ui_decorate_panel_left(-1); // calls ui_determine_orientation()
	gtk_widget_set_visible(t_main.panel_left,TRUE);
	if(vertical_mode)
		gtk_widget_set_visible(t_main.panel_right,FALSE);
	else
	{
		ui_show_home();
		gtk_widget_set_visible(t_main.panel_right,TRUE);
	}
}

static void ui_toggle_logging(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	const int peer_index = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,peer_index));
	int8_t log_messages = getter_int8(n,INT_MIN,-1,offsetof(struct peer_list,log_messages));
	// Update Setting
	if(log_messages == -1 || log_messages == 0)
	{
		log_messages++;
		setter(n,INT_MIN,-1,offsetof(struct peer_list,log_messages),&log_messages,sizeof(log_messages));
	}
	else if(log_messages == 1)
	{
		log_messages = -1;
		setter(n,INT_MIN,-1,offsetof(struct peer_list,log_messages),&log_messages,sizeof(log_messages));
	}
	ui_set_image_logging(button,n);
	// Save Setting
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",log_messages);
	sql_setting(0,peer_index,"logging",p1,strlen(p1));
}

static void ui_toggle_mute(const gpointer data)
{
	const int n = vptoi(data);// DO NOT FREE ARG
	const int peer_index = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,peer_index));
	// Update Setting
	toggle_int8(&t_peer[n].mute); // safe usage
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_group_peerlist));
	// Save Setting
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",t_peer[n].mute);
	sql_setting(0,peer_index,"mute",p1,strlen(p1));
}

static void ui_toggle_mute_button(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	ui_toggle_mute(data);
	ui_set_image_mute(button,n);
}

static void call_start(const gpointer data)
{
	const int call_n = vptoi(data); // DO NOT FREE ARG
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_group_peerlist));

	const uint8_t owner = getter_uint8(call_n,INT_MIN,-1,offsetof(struct peer_list,owner));
	const uint8_t sendfd_connected = getter_uint8(call_n,INT_MIN,-1,offsetof(struct peer_list,sendfd_connected));
	const uint8_t recvfd_connected = getter_uint8(call_n,INT_MIN,-1,offsetof(struct peer_list,recvfd_connected));
	const uint8_t online = recvfd_connected + sendfd_connected;

	if(owner == ENUM_OWNER_GROUP_CTRL || online) // TODO remove 1
	{
		time_t time = 0;
		time_t nstime = 0;
		set_time(&time,&nstime);
		// printf("Checkpoint time=$time nstime=$nstime ${DateFormat('yyyy/MM/dd kk:mm:ss').format(DateTime.fromMillisecondsSinceEpoch((time as int) * 1000, isUtc: false))}");
		const int c = set_c(call_n,time,nstime);
		if(c > -1) // Necessary check in GTK
			call_join(iitovp(call_n,c));
	}
}

static void ui_call_start_button(GtkWidget *button,const gpointer data)
{
//	const int n = vptoi(data); // DO NOT FREE ARG
	(void)button;
	(void)data;
	int n = global_n;
	if(t_peer[global_n].pm_n > -1)
		n = t_peer[global_n].pm_n;
	call_start(itovp(n));
}

static void ui_toggle_block(const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	block_peer(n);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_group_peerlist));
	if(owner != ENUM_OWNER_GROUP_PEER)
	{
		ui_set_image_lock(n);
		ui_populate_peers(itovp(0));
	}
}

static void ui_toggle_block_button(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	ui_toggle_block(data);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner != ENUM_OWNER_GROUP_PEER)
		ui_set_image_block(button,n);
}

static void ui_toggle_kill(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	if(n != -1) // Not global kill
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(kill_active)));
	kill_code(n,NULL);
	if(t_main.popover_more && GTK_IS_WIDGET(t_main.popover_more))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_more));
//	error_simple("Need to add additional logic here to make this image properly togglable"); // no, currently can't unkill. Kill is kill.
//	ui_go_back(data); // do not do here, already doing in onion_deleted_idle
}

static void ui_toggle_delete(GtkWidget *button,const gpointer data)
{ // should not need to ui_populate_peers() because onion_deleted_cb should run
	const int n = vptoi(data); // DO NOT FREE ARG
	const int peer_index = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,peer_index));
	gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_active)));
	takedown_onion(peer_index,1);
	if(t_main.popover_more && GTK_IS_WIDGET(t_main.popover_more))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_more));
//	ui_go_back(data); // do not do here, already doing in onion_deleted_idle
}

static void ui_delete_log(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(clear_all_active)));
	delete_log(n);
}

static void ui_load_more_messages(const GtkScrolledWindow *scrolled_window,const GtkPositionType pos,const gpointer data)
{
	(void) scrolled_window;
	if((INVERSION_TEST && pos == GTK_POS_TOP) || (!INVERSION_TEST && pos == GTK_POS_BOTTOM)) // GTK_POS_BOTTOM
		return;
	const int n = vptoi(data); // DO NOT FREE ARG
	message_load_more(n);
}

static void ui_input_new(GtkWidget *entry)
{ // New input has occured (do not call this, it should only be called from key-released in ui_input_bad)
	if(!entry || !GTK_IS_EDITABLE(entry))
		return;
	gtk_widget_remove_css_class(entry,"bad_input");
}

static void ui_input_bad(GtkWidget *entry)
{ // Make entry/Editable red until new input occurs
	if(!entry || !GTK_IS_EDITABLE(entry))
		return;
	gtk_widget_add_css_class(entry,"bad_input");
	GtkEventController *return_controller = gtk_event_controller_key_new();
	g_signal_connect_swapped(return_controller, "key-released", G_CALLBACK(ui_input_new),entry); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(entry,  GTK_EVENT_CONTROLLER(return_controller));
}

static void ui_enter_mouse(void)
{
	gtk_widget_set_visible(t_main.headerbar_buttons_box_enter,FALSE);
	gtk_widget_set_visible(t_main.headerbar_buttons_box_leave,TRUE);
}

static void ui_leave_mouse(void)
{
	if(!t_main.headerbar_buttons_box_leave)
		return;
	gtk_widget_set_visible(t_main.headerbar_buttons_box_leave,FALSE);
	gtk_widget_set_visible(t_main.headerbar_buttons_box_enter,TRUE);

	GtkEventController *ev_enter = gtk_event_controller_motion_new();
	g_signal_connect_after(ev_enter, "enter", G_CALLBACK(ui_enter_mouse), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.headerbar_buttons_box_enter,  GTK_EVENT_CONTROLLER(ev_enter));
}

static void ui_minimize(void)
{
	if(minimize_to_tray && appindicator_functioning)
		gtk_widget_set_visible(t_main.main_window,FALSE);
	else
		gtk_window_minimize(GTK_WINDOW(t_main.main_window));
	ui_leave_mouse(); // necessary
}

static void ui_toggle_maximize(void)
{
	if(gtk_window_is_maximized(GTK_WINDOW(t_main.main_window)))
		gtk_window_unmaximize(GTK_WINDOW(t_main.main_window));
	else
		gtk_window_maximize(GTK_WINDOW(t_main.main_window));
}

static void ui_determine_orientation(void)
{ // Sets vertical_mode TODO consider also aspect ratios and DPI settings to determine whether this is a mobile device or not. If mobile, should be vertical regardless of width.
// XXX TODO For zoom mode, ie for mobile devices of high DPI, https://docs.gtk.org/gsk4/method.Transform.scale.html ---> https://docs.gtk.org/gtk4/method.Widget.allocate.html
// also mobile devices should maximize by default
	const int width = gtk_widget_get_width(t_main.main_window);
//	int height = gtk_widget_get_height(t_main.main_window);
	if(vertical_mode)
	{
		if(width >= size_horizontal_mode_minimum_width)
			vertical_mode = 0;
	}
	else // 0
	{
		if(width < size_horizontal_mode_minimum_width)
		{
			if(t_main.window == window_auth)
				vertical_mode = 1;
			else
				vertical_mode = 2;
		}
	}
}

static int cleanup_idle(void *arg)
{
	const int sig_num = vptoi(arg);
	if(!sig_num && close_to_tray && appindicator_functioning)
	{
		gtk_widget_set_visible(t_main.main_window,FALSE);
		return 0;
	}
	char p1[21];
	int size;
	if((size = gtk_widget_get_width(t_main.main_window)) != size_window_default_width)
	{ // default h/w is loaded_size, so there is no wasted disk IO. fancy.
		snprintf(p1,sizeof(p1),"%d",size);
		sql_setting(1,-1,"gtk4-width",p1,strlen(p1));
	}
	if((size = gtk_widget_get_height(t_main.main_window)) != size_window_default_height)
	{ // TODO does gtk_widget_get_height need to be on the main thread? hope not
		snprintf(p1,sizeof(p1),"%d",size);
		sql_setting(1,-1,"gtk4-height",p1,strlen(p1));
	}
	/* Log Unread Message Count in the same manner that we store last_seen */
	if(log_unread == 1)
	{
		for(int peer_index,n = 0 ; (peer_index = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,peer_index))) > -1 || getter_byte(n,INT_MIN,-1,offsetof(struct peer_list,onion)) != 0 ; n++)
		{ // storing last_seen time to .key file. NOTE: this will execute more frequently than we might want if logging is disabled. however there is little we can do.
			if(peer_index < 0)
				continue;
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
			if(owner == ENUM_OWNER_CTRL || owner == ENUM_OWNER_GROUP_CTRL)
			{ // for private messages, will need to be more complicated than just adding GROUP_PEER here
				snprintf(p1,sizeof(p1),"%zu",t_peer[n].unread);
				sql_setting(0,peer_index,"unread",p1,strlen(p1));
			}
		}
	}
	const int8_t local_debug = torx_debug_level(-1);
	if(local_debug > 2)
	{
		fprintf(stderr,"Reducing debug level from %d to %d for safety.\n",local_debug,2);
		torx_debug_level(2); // For safety, to avoid sensitive things being written to stderr by error_cb
	}
	t_main.window = none; // must be first, maybe even earlier
	cleanup_lib(sig_num);
	gtk_window_destroy(GTK_WINDOW (t_main.main_window));
	exit(sig_num); // 2024/02/06 better than returning, which sometimes lead to gtk criticals
//	return 0;
}

void cleanup_cb_ui(const int sig_num)
{ // callback to UI to inform it that we are closing and it should save settings
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,cleanup_idle,itovp(sig_num),NULL);
}

static void ui_decorate_headerbar(void)
{
	/* Build HeaderBar (inactive) */
	GtkWidget *headerbar = gtk_header_bar_new();
	gtk_header_bar_set_show_title_buttons (GTK_HEADER_BAR(headerbar), FALSE);
	t_main.headerbar_buttons_box_enter = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_eight);

	// Build Close Button (inactive)
	GtkGesture *click_close = gtk_gesture_click_new();
	GtkWidget *headerbar_button1 = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_headerbar_button1));

	// Build Minimize Button (inactive)
	GtkGesture *click_minimize = gtk_gesture_click_new();
	GtkWidget *headerbar_button2 = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_headerbar_button2));

	// Build Maximize Button (inactive)
	GtkGesture *click_maximize = gtk_gesture_click_new();
	GtkWidget *headerbar_button3 = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_headerbar_button3));

	// Assemble HeaderBar (inactive)
	gtk_box_append(GTK_BOX(t_main.headerbar_buttons_box_enter), headerbar_button1);
	gtk_box_append(GTK_BOX(t_main.headerbar_buttons_box_enter), headerbar_button2);
	gtk_box_append(GTK_BOX(t_main.headerbar_buttons_box_enter), headerbar_button3);

	// Build HeaderBar (active)
	t_main.headerbar_buttons_box_leave = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_eight);

	// Build Close Button (active)
	GtkWidget *headerbar_button_leave1 = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_headerbar_button_leave1));
	g_signal_connect_swapped(click_close, "pressed", G_CALLBACK(cleanup_idle), itovp(0)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(headerbar_button_leave1, GTK_EVENT_CONTROLLER(click_close));

	// Build Minimize Button (active)
	GtkWidget *headerbar_button_leave2 = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_headerbar_button_leave2));
	g_signal_connect_swapped(click_minimize, "pressed", G_CALLBACK(ui_minimize), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(headerbar_button_leave2, GTK_EVENT_CONTROLLER(click_minimize));

	// Build Maximize Button (active)
	GtkWidget *headerbar_button_leave3 = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_headerbar_button_leave3));
	g_signal_connect_swapped(click_maximize, "pressed", G_CALLBACK(ui_toggle_maximize), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(headerbar_button_leave3, GTK_EVENT_CONTROLLER(click_maximize));

	// Assemble HeaderBar (active)
	gtk_box_append(GTK_BOX(t_main.headerbar_buttons_box_leave), headerbar_button_leave1);
	gtk_box_append(GTK_BOX(t_main.headerbar_buttons_box_leave), headerbar_button_leave2);
	gtk_box_append(GTK_BOX(t_main.headerbar_buttons_box_leave), headerbar_button_leave3);

	// Hide active by default
	gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), t_main.headerbar_buttons_box_enter);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), t_main.headerbar_buttons_box_leave);
	gtk_widget_set_visible(t_main.headerbar_buttons_box_leave,FALSE);    
	gtk_widget_add_css_class(headerbar,"headerbar");
	gtk_widget_add_css_class(headerbar_button1,"close");
	gtk_widget_add_css_class(headerbar_button2,"minimize");
	gtk_widget_add_css_class(headerbar_button3,"maximize");
	gtk_widget_add_css_class(headerbar_button_leave1,"close");
	gtk_widget_add_css_class(headerbar_button_leave2,"minimize");
	gtk_widget_add_css_class(headerbar_button_leave3,"maximize");
	gtk_window_set_titlebar (GTK_WINDOW(t_main.main_window),headerbar);

	GtkEventController *ev_enter = gtk_event_controller_motion_new();
	GtkEventController *ev_leave = gtk_event_controller_motion_new();

	g_signal_connect_after(ev_enter, "enter", G_CALLBACK(ui_enter_mouse), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect_after(ev_leave, "leave", G_CALLBACK(ui_leave_mouse), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.headerbar_buttons_box_enter,  GTK_EVENT_CONTROLLER(ev_enter));
	gtk_widget_add_controller(t_main.headerbar_buttons_box_leave,  GTK_EVENT_CONTROLLER(ev_leave));
}

static void ui_custom_switch(GtkToggleButton *button,void *arg)
{ // Action
	(void) button;
	struct stack_change *stack_change = (struct stack_change*) arg; // Casting passed struct
	char p1[21];
	snprintf(p1,sizeof(p1),"%zu",stack_change->iter);
	gtk_stack_set_visible_child_name (stack_change->stack,p1);
}

static GtkWidget *gtk_custom_switcher_new(GtkStack* stack,int orientation,uint8_t type)
{ // Alternative to gtk_stack_switcher_new
	if(!stack || (orientation != GTK_ORIENTATION_VERTICAL && orientation != GTK_ORIENTATION_HORIZONTAL) || (type != 0 && type != 1))
	{
		error_simple(0,"gtk_custom_switcher_new failed sanity check");
		return NULL;
	}
	t_main.stack_current = gtk_stack_get_visible_child_name(GTK_STACK(stack));
	GtkWidget *box;
	if(orientation == GTK_ORIENTATION_VERTICAL)
		box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	else /*if(orientation == GTK_ORIENTATION_HORIZONTAL)*/
		box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	size_t start,end;
	if(type == 0)
	{ // Peer list
		start = 0;
		end = 3;
	}
	else /*if(type == 1)*/
	{ // Home
		start = 3;
		end = sizeof(options)/sizeof(char*);
	}
	GtkWidget *first_button = NULL; // must initialize as null
	for(size_t iter = start; iter < end ; iter++)
	{
		size_t overlay_count;
		if(iter == 0)
			overlay_count = totalUnreadPeer;
		else if(iter == 1)
			overlay_count = totalUnreadGroup;
		else
			overlay_count = 0;
		GtkWidget *button = gtk_toggle_button_new_with_label (*options[iter]);
		gtk_widget_set_hexpand(button, TRUE);
		char p1[21];
		snprintf(p1,sizeof(p1),"%zu",iter);
		if(!strncmp(t_main.stack_current,p1,1))
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),TRUE);
		if(first_button) // first button already exists
			gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),GTK_TOGGLE_BUTTON(first_button));
		else // we ARE the first button
			first_button = button;
		if(overlay_count)
		{
			GtkWidget *overlay = gtk_overlay_new();
			gtk_overlay_set_child(GTK_OVERLAY(overlay),button);

			GtkWidget *overlay2 = gtk_overlay_new();
			gtk_widget_set_size_request(overlay2,(int)((double)size_peerlist_icon_size/1.5),(int)((double)size_peerlist_icon_size/1.5));
			gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
			gtk_widget_set_valign(overlay2, GTK_ALIGN_END);
			GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
			gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

			char unread_count[21];
			snprintf(unread_count,sizeof(unread_count),"%zu",overlay_count);
			GtkWidget *unread_counter = gtk_label_new(unread_count);
			gtk_widget_add_css_class(unread_counter, "unread_count");
			gtk_widget_set_halign(unread_counter, GTK_ALIGN_CENTER);
			gtk_widget_set_valign(unread_counter, GTK_ALIGN_CENTER);
			gtk_overlay_add_overlay(GTK_OVERLAY(overlay2),unread_counter);
			gtk_overlay_add_overlay(GTK_OVERLAY(overlay),overlay2);

			gtk_box_append (GTK_BOX (box), overlay);
		}
		else
			gtk_box_append (GTK_BOX (box), button);
		struct stack_change *stack_change = torx_insecure_malloc(sizeof(struct stack_change));
		stack_change->stack = stack;
		stack_change->iter = iter;
		g_signal_connect(button, "toggled", G_CALLBACK (ui_custom_switch),stack_change); // DO NOT FREE arg because this only gets passed ONCE.
	}
	return box;
}

static void ui_decorate_panel_left_top(void)
{
	if(!t_main.search_panel)
		return;
	// Set Add / Generate Icon
	if(t_main.add != NULL)
		gtk_box_remove(GTK_BOX(t_main.search_panel), t_main.add);
	if(t_main.window == window_main || t_main.window == window_group_generate)
		t_main.add = gtk_image_new_from_paintable(GDK_PAINTABLE(add_active));
	else if(global_theme == DARK_THEME)
		t_main.add = gtk_image_new_from_paintable(GDK_PAINTABLE(add_inactive));
	else
		t_main.add = gtk_image_new_from_paintable(GDK_PAINTABLE(add_inactive_light));
	gtk_widget_set_size_request(t_main.add,size_icon_top_left,size_icon_top_left);
	if(t_main.window != window_main && t_main.window != window_group_generate)
	{
		GtkGesture *click_show_generate = gtk_gesture_click_new();
		g_signal_connect_swapped(click_show_generate, "pressed", G_CALLBACK(ui_show_generate), NULL); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(t_main.add, GTK_EVENT_CONTROLLER(click_show_generate));
	}
	gtk_box_append(GTK_BOX(t_main.search_panel), t_main.add);

	// Set Home Icon
	if(t_main.home != NULL)
		gtk_box_remove(GTK_BOX(t_main.search_panel), t_main.home);
	GtkWidget *button_home;
	if(t_main.window == window_home)
		button_home = gtk_image_new_from_paintable(GDK_PAINTABLE(home_active));
	else if(global_theme == DARK_THEME)
		button_home = gtk_image_new_from_paintable(GDK_PAINTABLE(home_inactive));
	else
		button_home = gtk_image_new_from_paintable(GDK_PAINTABLE(home_inactive_light));
	gtk_widget_set_size_request(button_home,size_icon_top_left,size_icon_top_left);
	t_main.home = gtk_overlay_new();
	gtk_overlay_set_child(GTK_OVERLAY(t_main.home),button_home);
	if(t_main.window != window_home)
	{
		GtkGesture *click_home = gtk_gesture_click_new();
		g_signal_connect_swapped(click_home, "pressed", G_CALLBACK(ui_show_home), NULL); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(t_main.home, GTK_EVENT_CONTROLLER(click_home));
	}
	gtk_box_append(GTK_BOX(t_main.search_panel), t_main.home);
	if(totalIncoming > 0)
	{ // Badge on home button (Note: some of it starts before)
		GtkWidget *overlay2 = gtk_overlay_new();
		gtk_widget_set_size_request(overlay2,(int)((double)size_peerlist_icon_size/1.5),(int)((double)size_peerlist_icon_size/1.5));
		gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
		gtk_widget_set_valign(overlay2, GTK_ALIGN_START);
		GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
		gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

		char incoming_count[21];
		snprintf(incoming_count,sizeof(incoming_count),"%zu",totalIncoming);
		GtkWidget *unread_counter = gtk_label_new(incoming_count);
		gtk_widget_add_css_class(unread_counter, "unread_count");
		gtk_widget_set_halign(unread_counter, GTK_ALIGN_CENTER);
		gtk_widget_set_valign(unread_counter, GTK_ALIGN_CENTER);
		gtk_overlay_add_overlay(GTK_OVERLAY(overlay2),unread_counter);
		gtk_overlay_add_overlay(GTK_OVERLAY(t_main.home),overlay2);
	}

	// Set Active Settings Icon
	if(t_main.settings != NULL)
		gtk_box_remove(GTK_BOX(t_main.search_panel), t_main.settings);
	if(t_main.window == window_settings) // TODO use gtk_image_set_from_paintable instead of removing?
		t_main.settings = gtk_image_new_from_paintable(GDK_PAINTABLE(settings_active));
	else if(global_theme == DARK_THEME)
		t_main.settings = gtk_image_new_from_paintable(GDK_PAINTABLE(settings_inactive));
	else
		t_main.settings = gtk_image_new_from_paintable(GDK_PAINTABLE(settings_inactive_light));
	gtk_widget_set_size_request(t_main.settings,size_icon_top_left,size_icon_top_left);
	if(t_main.window != window_settings)
	{
		GtkGesture *click_settings = gtk_gesture_click_new();
		g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(ui_show_settings), NULL); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(t_main.settings, GTK_EVENT_CONTROLLER(click_settings));
	}
	gtk_box_append(GTK_BOX(t_main.search_panel), t_main.settings);

	// Build Stack Switcher
	if(t_main.custom_switcher_box)
	{ // necessary
		gtk_box_remove_all(t_main.custom_switcher_box);
		GtkWidget *custom_switcher = gtk_custom_switcher_new(GTK_STACK (t_main.panel_left_stack),GTK_ORIENTATION_HORIZONTAL,0);
		gtk_box_append(GTK_BOX(t_main.custom_switcher_box),custom_switcher);
	}
}

static void ui_decorate_panel_left(const int n)
{ /* This doesn't do initial build of left panel (only the things that might change), and destroys right panel */
 // TODO lots of this is redundant. better to just gtk_image_set_ rather than remove and rebuild everything (these things are already built in show_main_screen() TODO
	if(t_main.window != window_peer) // *** coundition could be || (n > -1), if we have more peer specific windows than 4 or other reasons as necessary
		global_n = -1; // this is critical
	show_keyboard = 1; // this is critical
	ui_determine_orientation(); // check for changes since last click-about
	ui_decorate_panel_left_top();

	/* Destroy and rebuild right panel */
	t_main.panel_right = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_eight);
	gtk_widget_set_halign(t_main.panel_right, GTK_ALIGN_FILL);
	gtk_widget_set_valign(t_main.panel_right, GTK_ALIGN_FILL);
	gtk_widget_add_css_class(t_main.panel_right,"panel_right");
	gtk_paned_set_end_child (GTK_PANED (t_main.paned), t_main.panel_right);
	if(vertical_mode == 1)
		gtk_widget_set_visible(t_main.panel_right,FALSE);
	else if(vertical_mode == 2)
		gtk_widget_set_visible(t_main.panel_left,FALSE);

	/* Build Header Bar */
	t_main.chat_headerbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_zero);
	gtk_widget_add_css_class(t_main.chat_headerbar,"chat_headerbar");

	/* Build left part of header bar */
	t_main.chat_headerbar_left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_margin_top(t_main.chat_headerbar_left, size_margin_fifteen);
	gtk_widget_set_margin_start(t_main.chat_headerbar_left, size_margin_ten);
	gtk_widget_set_margin_bottom(t_main.chat_headerbar_left, size_margin_fifteen);
	gtk_widget_set_hexpand(t_main.chat_headerbar_left, TRUE);
	gtk_widget_set_halign(t_main.chat_headerbar_left, GTK_ALIGN_START);
	gtk_widget_set_valign(t_main.chat_headerbar_left, GTK_ALIGN_CENTER);

	gtk_box_append(GTK_BOX(t_main.chat_headerbar),t_main.chat_headerbar_left);

	/* Build Image Area of Header Bar */
	t_main.image_header = gtk_image_new();
	if(t_main.window == window_main || t_main.window == window_group_generate) // (add, generate)
		t_main.image_header = gtk_image_new_from_paintable(GDK_PAINTABLE(add_active));
	else if(t_main.window == window_home || t_main.window == window_global_killcode || t_main.window == window_log_tor || t_main.window == window_log_torx) // Home (friend requests, outgoing requests, active sing, active mult)
		gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(home_active));
	else if(t_main.window == window_settings || t_main.window == window_change_password || t_main.window == window_edit_torrc) // Settings
		gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(settings_active));
	else if(t_main.window == window_peer) // Chat window
		ui_set_image_lock(n);
	gtk_widget_set_size_request(t_main.image_header, size_image_left_headerbar, size_image_left_headerbar);
	gtk_widget_set_valign(t_main.image_header, GTK_ALIGN_CENTER);

	GtkGesture *click_image_header = gtk_gesture_click_new();
	g_signal_connect_swapped(click_image_header, "pressed", G_CALLBACK(ui_go_back),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.image_header, GTK_EVENT_CONTROLLER(click_image_header));
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), t_main.image_header);

	/* This goes last, to attach headerbar to panel */
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.chat_headerbar);

	/* Build basic scroll box */
	t_main.scrolled_window_right = gtk_scrolled_window_new(); // XXX penguin XXX NOT HERE
	t_main.scroll_box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_message_spacing);
	gtk_widget_set_valign(t_main.scroll_box_right, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(t_main.scroll_box_right, TRUE);	// works, do not remove
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_right), t_main.scroll_box_right);

	/* Build right part of header bar */
	t_main.chat_headerbar_right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(t_main.chat_headerbar_right, GTK_ALIGN_END);
	gtk_widget_set_valign(t_main.chat_headerbar_right, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_end(t_main.chat_headerbar_right, size_margin_ten);
	gtk_box_append(GTK_BOX(t_main.chat_headerbar),t_main.chat_headerbar_right);
}

static void ui_override_save_torrc(GtkWidget *button,char *torrc_content_local)
{ // do we need button? could use _swapped
	(void) button;
	if(t_main.popover_torrc_errors && GTK_IS_WIDGET(t_main.popover_torrc_errors))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_torrc_errors));
	torrc_save(torrc_content_local);
	sodium_memzero(torrc_content_local,strlen(torrc_content_local));
	g_free(torrc_content_local);
	torrc_content_local = NULL;

}

static void ui_save_torrc(GtkWidget *button, gpointer arg)
{
	(void) arg;
	GtkTextIter start,end = {0};
	gtk_text_buffer_get_bounds(t_main.textbuffer_torrc, &start, &end);
	char *torrc_content_local = gtk_text_buffer_get_text(t_main.textbuffer_torrc,&start,&end,FALSE); // TODO have it throw an error if the result is too long
	char *torrc_errors = torrc_verify(torrc_content_local);
	if(torrc_errors == NULL)
		ui_override_save_torrc(NULL,torrc_content_local);
	else
	{
		t_main.popover_torrc_errors = custom_popover_new(button);
		GtkWidget *label = gtk_label_new(torrc_errors);
		gtk_widget_add_css_class(label, "text");

		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_five);
		gtk_widget_add_css_class(box, "popover_inner");
		GtkWidget *button_override = gtk_button_new_with_label (text_override);
		gtk_box_append (GTK_BOX (box), label);
		gtk_box_append (GTK_BOX (box), button_override);
		g_signal_connect(button_override, "clicked", G_CALLBACK (ui_override_save_torrc), torrc_content_local); // XXX NOTE: this gets freed which is OK because only called once

		gtk_popover_set_child(GTK_POPOVER(t_main.popover_torrc_errors),box);
		gtk_popover_popup(GTK_POPOVER(t_main.popover_torrc_errors));
		torx_free((void*)&torrc_errors);
	}
}

static void ui_show_edit_torrc(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_edit_torrc;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_edit_torrc);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */
	GtkWidget *label_save = gtk_label_new (text_saving_will_restart_tor);
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), label_save);
	GtkWidget *button_save_torrc = gtk_button_new_with_label (text_save);
	g_signal_connect(button_save_torrc, "clicked", G_CALLBACK (ui_save_torrc), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right),button_save_torrc);
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	gtk_text_buffer_set_text(t_main.textbuffer_torrc,torrc_content,-1);
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	GtkWidget *view = gtk_text_view_new_with_buffer (t_main.textbuffer_torrc);
	gtk_widget_set_hexpand(view, TRUE);	// redundant/default
	gtk_widget_set_vexpand(view, TRUE);	// required
	gtk_text_view_set_editable (GTK_TEXT_VIEW (view), TRUE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (view), TRUE);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), view);
}

static void ui_change_password(GtkWidget *button,const char *arg)
{
	const int action = vptoi(arg);
	gtk_widget_set_opacity(button,0.5);
	if(action == 1)
	{
		char *password_old_ptr = gtk_editable_get_chars(GTK_EDITABLE(t_main.pass_old),0,-1);
		char *password_new_ptr = gtk_editable_get_chars(GTK_EDITABLE(t_main.pass_new),0,-1);
		char *password_verify_ptr = gtk_editable_get_chars(GTK_EDITABLE(t_main.pass_verify),0,-1);
		change_password_start(password_old_ptr,password_new_ptr,password_verify_ptr);
		sodium_memzero(password_old_ptr,strlen(password_old_ptr));
		sodium_memzero(password_new_ptr,strlen(password_new_ptr));
		sodium_memzero(password_verify_ptr,strlen(password_verify_ptr));
		g_free(password_old_ptr);
		g_free(password_new_ptr);
		g_free(password_verify_ptr);
		password_old_ptr = password_new_ptr = password_verify_ptr = NULL;
	}
	else if(action == 2)
	{
		char *password_old_ptr = gtk_editable_get_chars(GTK_EDITABLE(t_main.pass_old),0,-1);
		change_password_start(password_old_ptr,NULL,NULL);
		sodium_memzero(password_old_ptr,strlen(password_old_ptr));
		g_free(password_old_ptr);
		password_old_ptr = NULL;
	}
	else
		error_simple(0,"UI Coding error 28120120.");
}

static void ui_show_change_password(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_change_password;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_change_password);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */
	// TODO as of 2023/01/24, there is a property  GtkPasswordEntry:placeholder-text, but there is no setter, so we cant use it.
	GtkWidget *label_pass_old = gtk_label_new(text_old_password);
	GtkWidget *label_pass_new = gtk_label_new(text_new_password);
	GtkWidget *label_pass_verify = gtk_label_new(text_new_password_again);
	t_main.pass_old = gtk_password_entry_new();
	t_main.pass_new = gtk_password_entry_new();
	t_main.pass_verify = gtk_password_entry_new();
	gtk_editable_set_alignment(GTK_EDITABLE(t_main.pass_old),0.5);
	gtk_editable_set_alignment(GTK_EDITABLE(t_main.pass_new),0.5);
	gtk_editable_set_alignment(GTK_EDITABLE(t_main.pass_verify),0.5);
	gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(t_main.pass_old),TRUE);
	gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(t_main.pass_new),TRUE);
	gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(t_main.pass_verify),TRUE);

	gtk_box_append(GTK_BOX(t_main.scroll_box_right),label_pass_old);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),t_main.pass_old);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),label_pass_new);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),t_main.pass_new);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),label_pass_verify);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),t_main.pass_verify);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
//	t_main.button_change_password_continue = gtk_button_new_with_label (text_resume_interupted);
	t_main.button_change_password = gtk_button_new_with_label (text_change_password);
//	g_signal_connect(t_main.button_change_password_continue, "clicked", G_CALLBACK (ui_change_password), itovp(2)); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(t_main.button_change_password, "clicked", G_CALLBACK (ui_change_password), itovp(1)); // DO NOT FREE arg because this only gets passed ONCE.
//	gtk_box_append (GTK_BOX(box), t_main.button_change_password_continue);
	gtk_box_append (GTK_BOX(box), t_main.button_change_password);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right), box);
}

const char* languages_available_name[] = {"English","中文",NULL};
const char* languages_available_code[] = {"en_US","zh_CN",NULL};

static void ui_initialize_language(GtkWidget *combobox)
{
	if(combobox != NULL) // from settings page
	{
		const unsigned int selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox));
		snprintf(language,sizeof(language),"%s",languages_available_code[selected]);
		sql_setting(1,-1,"language",language,strlen(language));
	}
	if(language[0] == '\0' || !strncmp(language,"en_US",5))
	{
		text_title = "TorX";
		text_welcome = "Welcome to TorX";
		text_transfer_completed = "Transfer Completed";
		text_online = "Has come online";
		text_new_friend = "Has a new friend request";
		text_accepted_file = "Accepted a file";
		text_spoiled = "A single-use onion was spoiled";
		text_placeholder_privkey = "Onion Private Key (base64, 88 characters including trailing ==, or select from file)";
		text_placeholder_identifier = "Peer Nickname or Mult Identifier";
		text_placeholder_add_identifier = "Peer Nickname";
		text_placeholder_add_onion = "Peer TorX-ID or OnionID (provided by peer)";
		text_placeholder_add_group_identifier = "Group Nickname";
		text_placeholder_add_group_id = "Public Group ID (provided by peer)";
		text_placeholder_search = "Search";
		text_dark = "Dark";
		text_light = "Light";
		text_minimize_to_tray = "Minimize to tray";
		text_generate_onionid = "Generate OnionID";
		text_generate_torxid = "Generate TorX-ID";
		text_disable = "Disable";
		text_enable = "Enable";
		text_no = "No";
		text_yes = "Yes";
		text_leave_after = "Leave After";
		text_delete_after = "Delete After";
		text_select = "Select";
		text_save_sing = "Save as SING";
		text_save_mult = "Save as MULT";
		text_emit_global_kill = "Emit Global Kill Code";
		text_vertical_emit_global_kill = "Emit Global\nKill Code";
		text_peer = "Peer";
		text_group = "Group";
		text_group_offer = "Group Offer";
		text_audio_message = "Audio Message";
		text_audio_call = "Audio Call";
		text_sticker = "Sticker";
		text_current_members = "Current Members";
		text_group_private = "Private Group";
		text_group_public = "Public Group";
		text_block = "Block";
		text_unblock = "Unblock";
		text_ignore = "Ignore";
		text_unignore = "Unignore";
		text_edit = "Edit";
		text_incoming = "Incoming Requests";
		text_outgoing = "Outgoing Requests";
		text_active_mult = "Active Multi-Use IDs";
		text_active_sing = "Active Single-Use IDs";
		text_you = "You";
		text_queued = "Queued";
		text_draft = "Draft";
		text_accept = "Accept";
		text_reject = "Reject";
		text_copy = "Copy";
		text_resend = "Resend";
		text_start = "Start";
		text_pause = "Pause";
		text_choose_file = "Choose File";
		text_choose_files = "Choose Files";
		text_choose_folder = "Choose Folder";
		text_open_folder = "Open Folder";
		text_cancel = "Cancel";
		text_transfer_paused = "Transfer Paused";
		text_transfer_cancelled = "Transfer Cancelled";
		text_transfer_rejected = "Transfer Rejected";
		text_show_qr = "Show QR";
		text_save_qr = "Save QR";
		text_copy_qr = "Copy QR";
		text_delete = "Delete";
		text_delete_log = "Clear Log";
		text_hold_to_talk = "Hold to Talk";
		text_cancel_editing = "Cancel editing";
		text_private_messaging = "Private Messaging";
		text_rename = "Rename";	
		text_button_add = "Send\nFriend\nRequest";
		text_button_join = "Attempt\nTo\nJoin";
		text_button_sing = "Generate Single-Use ID";
		text_button_mult = "Generate Multi-Use ID";
		text_button_generate_invite = "Generate Invite-Only\nGroup";
		text_button_generate_public = "Generate Public\nGroup";
		text_wait = "Please Wait";
		text_tooltip_image_header_0 = "V3Auth Enabled. Peer Blocked.";
		text_tooltip_image_header_1 = "V3Auth Enabled. Peer Connected.";
		text_tooltip_image_header_2 = "V3Auth Enabled. Outbound Connected (50%).";
		text_tooltip_image_header_3 = "V3Auth Enabled. Inbound Connected (50%).";
		text_tooltip_image_header_4 = "V3Auth Enabled. Peer Disconnected.";
		text_tooltip_image_header_5 = "V3Auth Disabled. Peer Blocked.";
		text_tooltip_image_header_6 = "V3Auth Disabled. Peer Connected.";
		text_tooltip_image_header_7 = "V3Auth Disabled. Outbound Connected (50%).";
		text_tooltip_image_header_8 = "V3Auth Disabled. Inbound Connected (50%).";
		text_tooltip_image_header_9 = "V3Auth Disabled. Peer Disconnected.";
		text_tooltip_logging_disabled = "Logging Disabled";
		text_tooltip_logging_enabled = "Logging Enabled";
		text_tooltip_logging_global_on = "Using Global Logging Setting (On)";
		text_tooltip_logging_global_off = "Using Global Logging Setting (Off)";
		text_tooltip_notifications_off = "Notifications off";
		text_tooltip_notifications_on = "Notifications on";
		text_tooltip_blocked_on = "Peer Blocked";
		text_tooltip_blocked_off = "Peer Not Blocked";
		text_tooltip_button_select_custom = "Select hs_ed25519_secret_key file";
		text_tooltip_button_custom_sing = "Save as Single-Use OnionID";
		text_tooltip_button_custom_mult = "Save as Multiple-Use OnionID";
		text_tooltip_group_or_user_name = "Click to modify";
		text_tooltip_button_kill = "DANGER:\nInstruct Peer to Delete Keys and Message History\nThen Delete Keys and Message History";
		text_tooltip_button_delete = "DANGER:\nDelete Keys and Message History";
		text_tooltip_button_delete_log = "DANGER:\nDelete Message History";
		text_status_online = "Currently online";
		text_of = "of";
		text_status_last_seen = "Last seen";
		text_status_never = "Never";
		text_edit_torrc = "Edit Torrc";
		text_saving_will_restart_tor = "Saving will restart Tor";
		text_save = "Save";
		text_override = "Override / Ignore Error";
		text_change_password = "Change Password";
	//	text_resume_interupted = "Resume Interrupted\nPassword Change";
		text_old_password = "Old Password";
		text_new_password = "New Password";
		text_new_password_again = "New Password Again";
		text_settings = "Settings";
//		text_advanced_settings = "Advanced Settings";
		text_set_select_theme = "Select color scheme";
		text_set_select_language = "Select language";
		text_set_onionid_or_torxid = "TorX-ID (<=52 char) or OnionID (56 char with checksum)";
		text_set_global_log = "Message Logging (Global Default)";
		text_set_auto_resume_inbound = "Auto-Resume Inbound Transfers";
		text_set_save_all_stickers = "Save All Stickers";
		text_set_download_directory = "Select Download Directory";
		text_tor = "Tor"; // part B
		text_snowflake = "Snowflake"; // part B
		text_lyrebird = "Lyrebird"; // part B
		text_conjure = "Conjure"; // part B
		text_binary_location = "binary location (effective immediately)"; // part C
		text_set_cpu = "Maximum CPU threads for TorX-ID generation";
		text_set_suffix = "Minimum Suffix Length for TorX-ID generation";
		text_set_validity_sing = "Single-Use TorX-ID expiration time (days)";
		text_set_validity_mult = "Multiple-Use TorX-ID expiration time (days)";
		text_set_auto_mult = "Automatically Accept Incoming Mult Requests";
		text_set_tor_port_socks = "Tor SOCKS5 Port";
		text_set_tor_port_ctrl = "Tor Control Port";
		text_set_tor_password = "Tor Control Password";
		text_set_externally_generated = "Enter a externally generated vanity OnionID or TorX-ID (Advanced)";
		text_tor_log = "Tor Log";
		text_torx_log = "TorX Log";
		text_global_kill = "Global Kill Code";
		text_global_kill_warning = "WARNING:\n\nKill Code is intended to be \
used if your private keys are suspected to have been compromised and are being used to impersonate you. Emiting a Global Kill Code involves \
sending a Kill Code to each of your peers.\n\nEmitting a kill code is irrevocable for peers who are \
online and currently irrevocable also for peers who are offline. When a peer receives a kill code, they are instructed that your keys \
are compromised and their client is automatically requested to delete your contact and potentially all chat history also. After \
successfully sending the kill code, your client should also delete the peer and its associated chat history. \n\nDO NOT CLICK WITHOUT KNOWING WHAT YOU ARE DOING.\n\n\
After activating, you should keep your client active until all of your peers are automatically removed from your peer list, which occurs immediately \
after each comes online and receives the code.";
		text_home = "Home";
		text_add_generate = "Add Peer or Generate ID";
		text_add_peer_by = "Add a peer by their TorX-ID/OnionID";
		text_add_group_by = "Join a Public Group";
		text_generate_for = "Generate a TorX-ID/OnionID for sharing";
		text_generate_group_for = "Generate a Group for sharing";
		text_qr_code = "QR Code";
		text_enter_password = "Enter Password for Decryption";
		text_enter = "Enter";
		text_incorrect = "Incorrect Password";
		text_debug_level = "Current Debug Level:";
		text_debug_level_notice = "Debug level resets for safety on restart.";
		text_fatal_error = "Fatal Error";
		text_active =  "Active";
		text_identifier = "Identifier";
		text_onionid = "OnionID";
		text_torxid = "TorX-ID";
		text_invitor = "Invitor";
		text_groupid = "GroupID";
		text_successfully_created_group = "Successfully created group";
		text_error_creating_group = "Error creating group";
		text_censored_region = "Censored Region";
		text_invite_friend = "Invite Friend";  // unused in GTK
		text_group_peers = "Group Peers"; // unused in GTK
		text_incoming_call = "Incoming Call";
	}
	else if(!strncmp(language,"zh_CN",5))
	{
		//text_chats = "聊天";
		//text_add_generate_bottom = "添加 / 生成";
		//text_dismiss = "关闭";
		//text_enter_password_first_run = "输入密码";
		text_show_qr = "显示二维码";
		//text_scan_qr = "扫描二维码";
		//text_share_qr = "分享二维码";
		//text_log_always = "始终记录";
		//text_log_never = "从不记录";
		//text_log_global_on = "全域开启";
		//text_log_global_off = "全域关闭";
		//text_blocked = "已屏蔽";
		//text_unblocked = "解除屏蔽";
		//text_kill = "终止";
		//text_mute_on = "静音开启";
		//text_mute_off = "静音关闭";
		//text_keyboard_privacy = "键盘隐私";
		//text_placeholder_privkey_flutter = "Base64编码，88个字符（含末尾==）";
		//text_tap_return = "点击返回";
		//text_vertical_change_password = "修改密码";
		//text_quit = "退出";
		//text_reply = "回复";
		//text_warning = "警告";
		text_queued = "已排队";
		text_draft = "草稿";
		//text_select_sticker = "选择表情包";
		text_title = "TorX";
		text_welcome = "欢迎使用TorX";
		text_transfer_completed = "已发送";
		text_online = "在线";
		text_new_friend = "有新的好友请求";
		text_accepted_file = "已接收文件";
		text_spoiled = "一次性洋葱已失效";
		text_placeholder_privkey = "洋葱私钥（base64编码，88个字符含末尾==，或从文件选择）";
		text_placeholder_identifier = "好友昵称或多次标识符";
		text_placeholder_add_identifier = "好友昵称";
		text_placeholder_add_onion = "好友TorX-ID或洋葱ID（由好友提供）";
		text_placeholder_add_group_identifier = "群聊昵称";
		text_placeholder_add_group_id = "公共群聊ID（由好友提供）";
		text_placeholder_search = "搜索";
		text_dark = "深色";
		text_light = "浅色";
		text_minimize_to_tray = "最小化至托盘";
		text_generate_onionid = "生成洋葱ID";
		text_generate_torxid = "生成TorX-ID";
		text_disable = "禁用";
		text_enable = "启用";
		text_no = "否";
		text_yes = "是";
		text_leave_after = "离开时间";
		text_delete_after = "删除时间";
		text_select = "选择";
		text_save_sing = "保存为一次性";
		text_save_mult = "保存为多次";
		text_emit_global_kill = "发送全域销毁代码";
		text_vertical_emit_global_kill = "发送全域\n销毁代码";
		text_peer = "好友";
		text_group = "群聊";
		text_group_offer = "群聊邀请";
		text_audio_message = "语音消息";
		text_audio_call = "语音通话";
		text_sticker = "表情包";
		text_current_members = "当前成员";
		text_group_private = "私密群聊";
		text_group_public = "公开群聊";
		text_block = "屏蔽";
		text_unblock = "取消屏蔽";
		text_ignore = "消息免打扰";
		text_unignore = "取消消息免打扰";
		text_edit = "编辑";
		text_incoming = " 收到的请求";
		text_outgoing = " 发出的请求";
		text_active_mult = "活跃多次IDs";
		text_active_sing = "活跃一次性IDs";
		text_you = "你";
		text_accept = "接受";
		text_reject = "拒绝";
		text_copy = "复制";
		//text_copy_all = "全选复制";
		text_start = "开始";
		text_pause = "暂停";
		text_cancel = "取消";
		text_save_qr = "保存二维码";
		text_copy_qr = "复制二维码";
		text_delete = "删除";
		text_delete_log = "清除日志";
		text_hold_to_talk = "按住说话";
		text_cancel_editing = "取消编辑";
		text_private_messaging = "私密消息";
		text_rename = "重命名";
		text_button_add = "发送好友请求";
		text_button_join = "尝试加入";
		text_button_sing = "生成一次性ID";
		text_button_mult = "生成多次ID";
		text_button_generate_invite = "生成邀请制群组";
		text_button_generate_public = "生成公开群组";
		text_wait = "请稍候";
		text_tooltip_image_header_0 = "V3Auth已启用，好友已阻止";
		text_tooltip_image_header_1 = "V3Auth已启用，好友已连接";
		text_tooltip_image_header_2 = "V3Auth已启用，出站连接（50%）";
		text_tooltip_image_header_3 = "V3Auth已启用，入站连接（50%）";
		text_tooltip_image_header_4 = "V3Auth已启用，好友已断开";
		text_tooltip_image_header_5 = "V3Auth已禁用，好友已阻止";
		text_tooltip_image_header_6 = "V3Auth已禁用，好友已连接";
		text_tooltip_image_header_7 = "V3Auth已禁用，出站连接（50%）";
		text_tooltip_image_header_8 = "V3Auth已禁用，入站连接（50%）";
		text_tooltip_image_header_9 = "V3Auth已禁用，好友已断开";
		text_tooltip_logging_disabled = "日志记录已禁用";
		text_tooltip_logging_enabled = "日志记录已启用";
		text_tooltip_logging_global_on = "使用全域日志设置（开启）";
		text_tooltip_logging_global_off = "使用全域日志设置（关闭）";
		text_tooltip_notifications_off = "通知已关闭";
		text_tooltip_notifications_on = "通知已开启";
		text_tooltip_blocked_on = "好友已阻止";
		text_tooltip_blocked_off = "好友未阻止";
		text_tooltip_button_select_custom = "选择hs_ed25519_secret_key文件";
		text_tooltip_button_custom_sing = "保存为一次性洋葱ID";
		text_tooltip_button_custom_mult = "保存为多次洋葱ID";
		text_tooltip_group_or_user_name = "点击修改";
		text_tooltip_button_kill = "危险：\n指示对等方删除密钥和消息历史\n然后删除本地密钥和历史消息记录";
		text_tooltip_button_delete = "危险：\n删除密钥和消息历史";
		text_status_online = "当前在线";
		text_of = "来自";
		text_status_last_seen = "上次在线";
		text_status_never = "从未";
		text_edit_torrc = "编辑Torrc";
		text_saving_will_restart_tor = "保存将重启Tor服务";
		//text_save_torrc = "保存Torrc";
		text_change_password = "修改密码";
		//text_resume_interupted = "恢复中断的\n密码修改";
		text_old_password = "旧密码";
		text_new_password = "新密码";
		text_new_password_again = "再次输入新密码";
		text_settings = "设置";
		text_set_select_theme = "选择主题色";
		text_set_select_language = "选择语言";
		text_set_onionid_or_torxid = "TorX-ID（≤52字符）或洋葱ID（含校验和56字符）";
		text_set_global_log = "消息日志（全域默认）";
		text_set_auto_resume_inbound = "自动恢复入站传输";
		text_set_save_all_stickers = "保存所有表情包";
		text_set_download_directory = "选择下载目录";
		//text_set_tor = "选择自定义Tor二进制文件路径（立即生效）";
		text_tor = "Tor"; // part B
		text_snowflake = "Snowflake"; // part B
		text_lyrebird = "Lyrebird"; // part B
		text_conjure = "Conjure"; // part B
		text_binary_location = "二进制地址(即刻生效)"; // part C
		text_set_cpu = "TorX-ID生成的最大CPU线程数";
		text_set_suffix = "TorX-ID生成的最小后缀长度";
		text_set_validity_sing = "一次性TorX-ID有效期（天）";
		text_set_validity_mult = "多次TorX-ID有效期（天）";
		text_set_auto_mult = "自动接受收到的多次请求";
		text_set_tor_port_socks = "Tor SOCKS5 端口";
		text_set_tor_port_ctrl = "Tor Control 端口";
		text_set_tor_password = "Tor Control 暗语";
		text_set_externally_generated = "输入外部生成的自定义洋葱ID或TorX-ID";
		text_tor_log = "Tor日志";
		text_torx_log = "TorX日志";
		text_global_kill = "全域销毁代码";
		text_global_kill_warning = "销毁代码用于怀疑私钥泄露并被用于冒充你时。发送全局销毁代码会向每个好友发送指令：\n\n在线好友会立即删除你的密钥和聊天记录，离线好友上线后也会执行相同操作。发送成功后，本地客户端也会删除该好友及其聊天记录。\n\n操作前请确保完全理解风险！\n\n激活后请保持客户端运行，直到所有好友从列表中自动移除（对方上线并接收代码后立即生效）。";
		text_home = "主页";
		text_add_generate = "添加对等方或生成ID";
		text_add_peer_by = "通过TorX-ID/洋葱ID添加好友";
		text_add_group_by = "加入公开群组";
		text_generate_for = "生成TorX-ID/洋葱ID或二维码用于分享";
		text_generate_group_for = "生成可分享的群组";
		text_qr_code = "二维码";
		text_enter_password = "输入解密密码";
		text_enter = "确认";
		text_incorrect = "密码错误";
		text_debug_level = "调试级别：";
		text_debug_level_notice = "调试级别重启后自动重置以确保安全。";
		text_fatal_error = "致命错误";
		text_active = "活动中";
		text_identifier = "标识符";
		text_onionid = "洋葱ID";
		text_torxid = "TorX-ID";
		text_invitor = "邀请者";
		text_groupid = "群组ID";
		text_successfully_created_group = "群组创建成功";
		text_error_creating_group = "创建群组失败";
		text_censored_region = "审查区域";
		text_invite_friend = "邀请好友";
		text_group_peers = "群组成员";
		text_save = "保存";
		text_override = "覆盖 / 忽略错误";
		text_incoming_call = "来电";
	}
}

static void ui_change_theme(const gpointer combobox)
{ // Set and save a custom UI Setting in unencrypted form
	const int local_setting = (int)gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox));
	threadsafe_write(&mutex_global_variable,&global_theme,&local_setting,sizeof(local_setting));
	ui_theme(local_setting);
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(1,-1,"theme",p1,strlen(p1));
}

static void ui_change_id_type(const gpointer combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&shorten_torxids,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"shorten_torxids",p1,strlen(p1));
}

static void ui_change_global_logging(const gpointer combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&global_log_messages,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"global_log_messages",p1,strlen(p1));
}

static void ui_change_resume(const gpointer combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&auto_resume_inbound,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"auto_resume_inbound",p1,strlen(p1));
}

static void ui_change_save_all_stickers(const gpointer combobox)
{ // Setting
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		save_all_stickers = 1; // NOTE: is in UI thread already, no need to use locks
	else
		save_all_stickers = 0; // NOTE: is in UI thread already, no need to use locks
	char p1[21];
	snprintf(p1,sizeof(p1),"%u",save_all_stickers);
	sql_setting(0,-1,"save_all_stickers",p1,strlen(p1));
}

static void ui_change_daemonize(const gpointer combobox)
{ // Setting
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		minimize_to_tray = 1; // NOTE: is in UI thread already, no need to use locks
	else
		minimize_to_tray = 0; // NOTE: is in UI thread already, no need to use locks
	char p1[21];
	snprintf(p1,sizeof(p1),"%u",minimize_to_tray);
	sql_setting(0,-1,"minimize_to_tray",p1,strlen(p1));
}

static void ui_change_auto_accept_mult(const gpointer combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&auto_accept_mult,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"auto_accept_mult",p1,strlen(p1));
}

static void ui_change_destroy_input(GtkWidget *combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&destroy_input,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"destroy_input",p1,strlen(p1));
}

static void ui_spin_change(GtkWidget *spinbutton, gpointer data)
{ // Setting
	const int spin = vptoi(data);
	double value_current; // double is returned by gtk_spin_button_get_value
	int value_original;
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	if(spin == ENUM_SPIN_DEBUG)
		value_original = (int)torx_debug_level(-1);
	else if(spin == ENUM_SPIN_CPU_THREADS)
		value_original = (int)global_threads;
	else if(spin == ENUM_SPIN_SUFFIX_LENGTH)
		value_original = (int)suffix_length;
	else if(spin == ENUM_SPIN_SING_EXPIRATION)
		value_original = (int)sing_expiration_days;
	else if(spin == ENUM_SPIN_MULT_EXPIRATION)
		value_original = (int)mult_expiration_days;
	else if(spin == ENUM_SPIN_TOR_PORT_SOCKS)
		value_original = (int)tor_socks_port;
	else if(spin == ENUM_SPIN_TOR_PORT_CONTROL)
		value_original = (int)tor_ctrl_port;
	else
	{
		pthread_rwlock_unlock(&mutex_global_variable); // 🟩
		return; // unknown spinner, coding error
	}
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	if((int)(value_current = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton))) != value_original)
	{
		char p1[21];
		pthread_rwlock_wrlock(&mutex_global_variable); // 🟥
		if(spin == ENUM_SPIN_DEBUG)
		{
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			torx_debug_level((int8_t)value_current); // Does not save to file because safety
		}
		else if(spin == ENUM_SPIN_CPU_THREADS)
		{
			global_threads = (uint32_t)value_current;
			snprintf(p1,sizeof(p1),"%u",global_threads);
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			sql_setting(0,-1,"global_threads",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_SUFFIX_LENGTH)
		{
			suffix_length = (uint8_t)value_current;
			snprintf(p1,sizeof(p1),"%u",suffix_length);
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			sql_setting(0,-1,"suffix_length",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_SING_EXPIRATION)
		{
			sing_expiration_days = (uint32_t)value_current;
			snprintf(p1,sizeof(p1),"%u",sing_expiration_days);
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			sql_setting(0,-1,"sing_expiration_days",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_MULT_EXPIRATION)
		{
			mult_expiration_days = (uint32_t)value_current;
			snprintf(p1,sizeof(p1),"%u",mult_expiration_days);
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			sql_setting(0,-1,"mult_expiration_days",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_TOR_PORT_SOCKS)
		{
			const uint16_t tor_socks_port_local = tor_socks_port = (uint16_t)value_current;
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			printf("Checkpoint UI socks port change and restarting tor!!!\n");
			start_tor();
			if(tor_socks_port_local)
			{
				snprintf(p1,sizeof(p1),"%u",tor_socks_port_local);
				sql_setting(0,-1,"tor_socks_port",p1,strlen(p1));
			}
			else // if 0, delete setting
				sql_delete_setting(0,-1,"tor_socks_port");
		}
		else if(spin == ENUM_SPIN_TOR_PORT_CONTROL)
		{
			const uint16_t tor_ctrl_port_local = tor_ctrl_port = (uint16_t)value_current;
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			printf("Checkpoint UI ctrl port change and restarting tor!!!\n");
			start_tor();
			if(tor_ctrl_port_local)
			{
				snprintf(p1,sizeof(p1),"%u",tor_ctrl_port_local);
				sql_setting(0,-1,"tor_ctrl_port",p1,strlen(p1));
			}
			else // if 0, delete setting
				sql_delete_setting(0,-1,"tor_ctrl_port");
		}
		else
		{
			pthread_rwlock_unlock(&mutex_global_variable); // 🟩
			error_simple(0,"Unknown spinner. Coding error. Report this.");
		}
	}
}

static void ui_submit_custom_input(const void *arg)
{
	const uint8_t owner = (uint8_t)vptoi(arg); // DO NOT FREE ARG
	const char *identifier_ptr = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_custom_identifier)));
	const char *privkey_ptr = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_custom_privkey)));
	char identifier[56+1];
	char privkey[88+1];
	snprintf(identifier,sizeof(identifier),"%s",identifier_ptr);
	snprintf(privkey,sizeof(privkey),"%s",privkey_ptr);
	const int n = custom_input(owner,identifier,privkey);
	sodium_memzero(identifier,sizeof(identifier));
	sodium_memzero(privkey,sizeof(privkey));
	if(n < 0)
	{
		ui_input_bad(t_main.entry_custom_privkey);
		return;
	}
	char torxid[52+1];
	getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,offsetof(struct peer_list,torxid));
	if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		gtk_text_buffer_set_text(t_main.textbuffer_custom_onion,torxid,-1);
	else
	{
		char onion[56+1];
		getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,onion));
		gtk_text_buffer_set_text(t_main.textbuffer_custom_onion,onion,-1);
		sodium_memzero(onion,sizeof(onion));
	}
	sodium_memzero(torxid,sizeof(torxid));
	gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_custom_identifier)),0,-1);
	gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_custom_privkey)),0,-1);
	if(destroy_input == 1)
		destroy_file(t_main.custom_input_location);
}

static GtkWidget *ui_combobox(const char *description,void (*callback)(void *const),const guint active_iter,...)
{ // Usage: = ui_combobox(description,&callback,active_iter,string1,string2,NULL);
	GtkWidget *outer_box = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label = gtk_label_new(description);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label, TRUE);	// works, do not remove

	GtkWidget *inner_box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box, GTK_ALIGN_END);

	va_list va_args;
	va_start(va_args,active_iter);
	GtkStringList* list_store = gtk_string_list_new(NULL);
	while(1)
	{
		const char *string;
		if((string = va_arg(va_args,char*)) == NULL) // Must be null terminated
			break; // End of list
		gtk_string_list_append(list_store,string);
	}
	va_end(va_args);
	GtkWidget *combo_box = gtk_drop_down_new(G_LIST_MODEL(list_store), NULL);
	gtk_box_append (GTK_BOX (inner_box), combo_box);
	gtk_drop_down_set_selected(GTK_DROP_DOWN(combo_box),active_iter);
	g_signal_connect(combo_box, "notify::selected-item", G_CALLBACK(callback), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (outer_box), label);
	gtk_box_append (GTK_BOX (outer_box), inner_box);
	return outer_box;
}

static GtkWidget *ui_spinbutton(const char *description,const int spinner,const int value,const int min, const int max)
{
	GtkWidget *outer_box = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label = gtk_label_new(description);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label, TRUE);	// works, do not remove
	GtkWidget *spinbutton = gtk_spin_button_new_with_range (min,max,1);
	GtkWidget *inner_box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box, GTK_ALIGN_END);
	gtk_box_append (GTK_BOX (inner_box), spinbutton);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton),value);
	g_signal_connect(spinbutton, "unrealize", G_CALLBACK(ui_spin_change),itovp(spinner)); // TODO see: jo0f2ofjo  // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (outer_box), label);
	gtk_box_append (GTK_BOX (outer_box), inner_box);
	return outer_box;
}

static void ui_choose_folder(GtkWidget *button,const gpointer arg)
{
	(void) arg;
	GtkFileDialog *dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_modal(dialog,TRUE); // block interaction with UI
	gtk_file_dialog_set_title(dialog,text_choose_folder); // also: gtk_file_dialog_set_filters
	gtk_file_dialog_select_folder (dialog,GTK_WINDOW(t_main.main_window),NULL,(GAsyncReadyCallback)ui_on_choose_download_dir,button);
}

static void ui_choose_file(GtkWidget *button,const gpointer arg)
{ /* Opens a dialog for choosing which file to send */
	GtkFileDialog *dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_modal(dialog,TRUE); // block interaction with UI
	int n = global_n;
	if(t_peer[global_n].pm_n > -1)
		n = t_peer[global_n].pm_n;
	const int int_arg = vptoi(arg);
	if(int_arg == 2) // For adding stickers only
	{ // TODO could support jpeg/png for static stickers... would have to limit size... and metadata is a bigger concern. Probably not a good idea.
		if(t_main.popover_sticker && GTK_IS_WIDGET(t_main.popover_sticker))
			gtk_popover_popdown(GTK_POPOVER(t_main.popover_sticker));
		GtkFileFilter *filter = gtk_file_filter_new();
		gtk_file_filter_add_mime_type(filter,"image/gif"); // works!
	//	gtk_file_filter_add_suffix (filter,".gif"); // works!
		GListStore *filters = g_list_store_new (GTK_TYPE_FILE_FILTER);
		g_list_store_append (filters, filter);
		gtk_file_dialog_set_filters(dialog,G_LIST_MODEL(filters));
		gtk_file_dialog_set_default_filter (dialog,filter);
	}
	if(int_arg == 1) // For file_send only
	{  // For file_send only, select multiple
		gtk_file_dialog_set_title(dialog,text_choose_files); // also: gtk_file_dialog_set_filters
		gtk_file_dialog_open_multiple (dialog,GTK_WINDOW(t_main.main_window),NULL,(GAsyncReadyCallback)ui_on_choose_files,itovp(n));
	}
	else
	{  // For selecting a file for reasons other than file sharing
		gtk_file_dialog_set_title(dialog,text_choose_file); // also: gtk_file_dialog_set_filters
		gtk_file_dialog_open (dialog,GTK_WINDOW(t_main.main_window),NULL,(GAsyncReadyCallback)ui_on_choose_file,button);
	}
}

GtkWidget *ui_choose_binary(char **location,const char *widget_name,const char *name)
{
	if(!location || !widget_name || !name)
		return NULL;
	GtkWidget *box = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	char label_text[512]; // size is arbitrary
	snprintf(label_text,sizeof(label_text),"%s %s %s",text_select,name,text_binary_location);
	GtkWidget *label = gtk_label_new(label_text);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label, TRUE); // works, do not remove
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	GtkWidget *button = gtk_button_new_with_label (*location);
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	GtkWidget *inner_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box, GTK_ALIGN_END);
	gtk_box_append (GTK_BOX (inner_box), button);
	gtk_widget_set_name(button,widget_name);
	g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_file), itovp(0));
	gtk_box_append (GTK_BOX (box), label);
	gtk_box_append (GTK_BOX (box), inner_box);
	return box;
}

GtkWidget *ui_setting_entry(void (*callback)(GtkWidget *,void*), const char *description,const char *existing)
{
	if(!description || !callback)
		return NULL; // This will cause failure, bad.
	GtkWidget *box = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label = gtk_label_new(description);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label, TRUE);	// works, do not remove
	gtk_box_append (GTK_BOX (box), label);
	GtkWidget *entry = gtk_entry_new();
	if(existing) // must null check
		gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(entry)),existing,-1);
	g_signal_connect(entry, "unrealize", G_CALLBACK(callback),NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (box), entry);
	return box;
}

static void ui_tor_control_password_change(GtkWidget *entry, gpointer data)
{ // Setting
	(void)data;
	const char *text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(entry)));
	const size_t text_len = text ? strlen(text) : 0;
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	const size_t current_len = control_password_clear ? strlen(control_password_clear) : 0;
	uint8_t changed = 0;
	if((text_len || current_len) && (current_len != text_len || strcmp(text,control_password_clear)))
		changed = 1;
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	if(!changed)
		return;
	pthread_rwlock_wrlock(&mutex_global_variable); // 🟥
	torx_free((void*)&control_password_clear);
	if(text_len)
	{
		control_password_clear = torx_secure_malloc(text_len+1);
		memcpy(control_password_clear,text,text_len+1);
	}
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	if(text_len)
		sql_setting(0,-1,"control_password_clear",text,text_len);
	else
		sql_delete_setting(0,-1,"control_password_clear");
	printf("Checkpoint UI password change and restarting tor!!!\n");
	start_tor();
}

static void ui_show_settings(void)
{ // Do not permit debug level as an option. Could make life easier but could also be very dangerous for people.
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_settings;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_settings);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */

	// Headerbar right buttons
	GtkWidget *button_torrc = gtk_button_new_with_label (text_edit_torrc);
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), button_torrc);
	g_signal_connect(button_torrc, "clicked", G_CALLBACK (ui_show_edit_torrc), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	GtkWidget *button_change_pass = gtk_button_new_with_label (text_change_password);
	g_signal_connect(button_change_pass, "clicked", G_CALLBACK (ui_show_change_password), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), button_change_pass);

	// Language Combobox
	GtkWidget *box_language = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label_language = gtk_label_new(text_set_select_language);
	gtk_widget_set_halign(label_language, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label_language, TRUE);	// works, do not remove

	GtkWidget *inner_box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box, GTK_ALIGN_END);

	GtkWidget *combo_box_language = gtk_drop_down_new_from_strings(languages_available_name);
	gtk_box_append (GTK_BOX (inner_box), combo_box_language);
	if(language[0] == '\0' || !strncmp(language,languages_available_code[0],5))
		gtk_drop_down_set_selected(GTK_DROP_DOWN(combo_box_language),0);
	else if(!strncmp(language,languages_available_code[1],5))
		gtk_drop_down_set_selected(GTK_DROP_DOWN(combo_box_language),1);
	g_signal_connect(combo_box_language, "notify::selected-item", G_CALLBACK(ui_initialize_language), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (box_language), label_language);
	gtk_box_append (GTK_BOX (box_language), inner_box);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box_language);

	// Theme Combobox
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_set_select_theme,&ui_change_theme,(guint)global_theme,text_dark,text_light,NULL));

	// Close to daemon Combobox
	if(appindicator_functioning)
		gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_minimize_to_tray,&ui_change_daemonize,(guint)minimize_to_tray,text_disable,text_enable,NULL));

	// TorX-ID or OnionID Combobox
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_set_onionid_or_torxid,&ui_change_id_type,threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids),text_generate_onionid,text_generate_torxid,NULL));

	// Global Logging Default (is overridden by user settings)
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_set_global_log,&ui_change_global_logging,threadsafe_read_uint8(&mutex_global_variable,&global_log_messages),text_disable,text_enable,NULL));

	// Automatically resume inbound transfers (toggle)
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_set_auto_resume_inbound,&ui_change_resume,threadsafe_read_uint8(&mutex_global_variable,&auto_resume_inbound),text_disable,text_enable,NULL));

	// Save all stickers (toggle)
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_set_save_all_stickers,&ui_change_save_all_stickers,save_all_stickers,text_disable,text_enable,NULL));

	// Downloads Folder
	GtkWidget *box0 = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label0 = gtk_label_new(text_set_download_directory);
	gtk_widget_set_halign(label0, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label0, TRUE);	// works, do not remove
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	GtkWidget *button_download_dir = gtk_button_new_with_label (download_dir);
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	GtkWidget *inner_box1  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box1, GTK_ALIGN_END);
	gtk_box_append (GTK_BOX (inner_box1), button_download_dir);
	g_signal_connect(button_download_dir, "clicked", G_CALLBACK(ui_choose_folder), NULL);
	gtk_box_append (GTK_BOX (box0), label0);
	gtk_box_append (GTK_BOX (box0 ), inner_box1);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box0);

	// Tor binary location
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_choose_binary(&tor_location,"tor_location",text_tor));

	// Snowflake binary location
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_choose_binary(&snowflake_location,"snowflake_location",text_snowflake));

	// Lyrebird binary location
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_choose_binary(&lyrebird_location,"lyrebird_location",text_lyrebird));

	// Conjure binary location
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_choose_binary(&conjure_location,"conjure_location",text_conjure));

	// Maximum CPU threads
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_spinbutton(text_set_cpu,ENUM_SPIN_CPU_THREADS,(int)threadsafe_read_uint32(&mutex_global_variable,&global_threads),1,(int)threadsafe_read_uint32(&mutex_global_variable,&threads_max)));

	// Suffix Length
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_spinbutton(text_set_suffix,ENUM_SPIN_SUFFIX_LENGTH,threadsafe_read_uint8(&mutex_global_variable,&suffix_length),0,9));

	// Sing Validity
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_spinbutton(text_set_validity_sing,ENUM_SPIN_SING_EXPIRATION,(int)threadsafe_read_uint32(&mutex_global_variable,&sing_expiration_days),0,5475)); // setting reasonable max of 15 years because it it over-runs 2038 we gonna have issues

	// Mult Validity
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_spinbutton(text_set_validity_mult,ENUM_SPIN_MULT_EXPIRATION,(int)threadsafe_read_uint32(&mutex_global_variable,&mult_expiration_days),0,5475)); // setting reasonable max of 15 years because it it over-runs 2038 we gonna have issues

	// Automatically Accept Mult
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_combobox(text_set_auto_mult,&ui_change_auto_accept_mult,auto_accept_mult,text_no,text_yes,NULL));

	// Tor SOCKS5 Port
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_spinbutton(text_set_tor_port_socks,ENUM_SPIN_TOR_PORT_SOCKS,(int)threadsafe_read_uint16(&mutex_global_variable,&tor_socks_port),0,65536));

	// Tor Control Port
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_spinbutton(text_set_tor_port_ctrl,ENUM_SPIN_TOR_PORT_CONTROL,(int)threadsafe_read_uint16(&mutex_global_variable,&tor_ctrl_port),0,65536));

	// Tor Control Password
	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	char *control_password_clear_local = torx_copy(NULL,control_password_clear);
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), ui_setting_entry(ui_tor_control_password_change,text_set_tor_password,control_password_clear_local));
	torx_free((void*)&control_password_clear_local);

	// Custom Input
	GtkWidget *box8 = gtk_box_new(vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	GtkWidget *label8 = gtk_label_new(text_set_externally_generated);
	gtk_widget_set_halign(label8, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label8, TRUE);	// works, do not remove

	const char* strings2[] = {text_leave_after, text_delete_after, NULL};
	GtkWidget *combo_destroy_input = gtk_drop_down_new_from_strings(strings2);

	gtk_drop_down_set_selected(GTK_DROP_DOWN(combo_destroy_input),destroy_input);
	g_signal_connect(combo_destroy_input, "notify::selected-item", G_CALLBACK(ui_change_destroy_input), NULL); // DO NOT FREE arg because this only gets passed ONCE.

	GtkWidget *button_select_custom = gtk_button_new_with_label (text_select);
	gtk_widget_set_tooltip_text(button_select_custom,text_tooltip_button_select_custom);
	gtk_widget_set_name(button_select_custom,"custom_input_location");
	g_signal_connect(button_select_custom, "clicked", G_CALLBACK(ui_choose_file), itovp(0));

	GtkWidget *inner_box3  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box3, GTK_ALIGN_END);
	gtk_box_append (GTK_BOX (inner_box3), button_select_custom); // XXX
	gtk_box_append (GTK_BOX (inner_box3), combo_destroy_input);

	gtk_box_append (GTK_BOX (box8), label8);
	gtk_box_append (GTK_BOX (box8), inner_box3);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box8);

	// Custom Privkey
	t_main.entry_custom_privkey = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_custom_privkey),text_placeholder_privkey);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), t_main.entry_custom_privkey);

	// Custom Identifier
	GtkWidget *box10 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	t_main.entry_custom_identifier = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_custom_identifier),text_placeholder_identifier);
	GtkWidget *button_custom_sing = gtk_button_new_with_label (text_save_sing);
	gtk_widget_set_tooltip_text(button_custom_sing,text_tooltip_button_custom_sing);
	g_signal_connect_swapped(button_custom_sing, "clicked", G_CALLBACK(ui_submit_custom_input),itovp(ENUM_OWNER_SING)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_set_hexpand(t_main.entry_custom_identifier, TRUE);	// works, do not remove
	gtk_box_append (GTK_BOX (box10), t_main.entry_custom_identifier);
	gtk_box_append (GTK_BOX (box10), button_custom_sing);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box10);

	// Custom Onion (output)
	GtkWidget *box11 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	t_main.textbuffer_custom_onion = gtk_text_buffer_new(NULL);
	GtkWidget *textview11 = gtk_text_view_new_with_buffer(t_main.textbuffer_custom_onion);
	GtkWidget *button_custom_mult = gtk_button_new_with_label (text_save_mult);
	gtk_widget_set_tooltip_text(button_custom_mult,text_tooltip_button_custom_mult);
	g_signal_connect_swapped(button_custom_mult, "clicked", G_CALLBACK(ui_submit_custom_input),itovp(ENUM_OWNER_MULT)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_set_hexpand(textview11, TRUE);	// works, do not remove
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview11), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview11), FALSE);
	gtk_box_append (GTK_BOX (box11), textview11);
	gtk_box_append (GTK_BOX (box11), button_custom_mult);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box11);
}

static int scroll_func_idle(void *arg)
{ // This SHOULD NOT need to run in a g_idle_add... but it do. Can we set it to lowest priority? helps only a minor amount
// TODO https://discourse.gnome.org/t/solved-how-to-auto-scroll-the-scrollbar-to-bottom-when-window-get-minimized/1337/2
//	printf("Checkpoint Not scrolling because we're testing gtk_list_view_scroll_to\n");
//	return 0; // TODO remove
	GtkWidget *window = arg;
	if(!window || !GTK_IS_SCROLLED_WINDOW(window))
	{
		error_simple(0,"Not scrolled window. Report this."); // triggered on 2023/10/31
		breakpoint();
		return 0; // sanity check
	}
	GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(window));
	const double offset = gtk_adjustment_get_upper(GTK_ADJUSTMENT(adj));
//	fprintf(stderr,"Checkpoint Scroll Upper: %e\n",upper);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), offset);
	gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW(window),GTK_ADJUSTMENT(adj));
//TODO	printf("Checkpoint scrolling\n");
	return 0;
}

static int scroll_func_idle_inverted(void *arg)
{ // This SHOULD NOT need to run in a g_idle_add... but it do. Can we set it to lowest priority? helps only a minor amount
	GtkWidget *window = arg;
	if(!window || !GTK_IS_SCROLLED_WINDOW(window))
	{
		error_simple(0,"Not scrolled window. Report this."); // triggered on 2023/10/31
		breakpoint();
		return 0; // sanity check
	}
	GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(window));
	const double offset = gtk_adjustment_get_lower(GTK_ADJUSTMENT(adj));
//	fprintf(stderr,"Checkpoint Scroll Upper: %e\n",upper);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), offset);
	gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW(window),GTK_ADJUSTMENT(adj));
//	printf("Checkpoint scrolling\n");
	return 0;
}

static int tor_log_idle(void *data)
{
	if(data != NULL)
	{
		const size_t len = strlen(data);
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(t_main.tor_log_buffer,&end);
		gtk_text_buffer_insert(t_main.tor_log_buffer,&end,data,(int)len);
		if(t_main.window == window_log_tor)
			g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
		printf(WHITE"CHECKPOINT TOR LOG: %s\n"RESET,(char*)data);
		torx_free((void*)&data);
	}
/*	if(t_main.window == window_log_tor)
		return 1; // continue
	else
		return 0; // stop*/
	return 0;
}

/* static void tor_call_async_cb_ui(char *message)
{ // Do not delete
	printf("Checkpoint async_cb: %s\n",message);
	torx_free((void*)&message);
} */

void tor_log_cb_ui(char *message)
{
	if(!message)
		return;
/*	const char msg1[] = "GETINFO network-liveness\r\n";
	const char msg2[] = "GETINFO dormant\r\n";
	const char msg3[] = "GETINFO status/bootstrap-phase\r\n";
	tor_call_async(&tor_call_async_cb_ui,msg1);
	tor_call_async(&tor_call_async_cb_ui,msg2);
	tor_call_async(&tor_call_async_cb_ui,msg3);	*/
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,tor_log_idle,message,NULL); // frees pointer*
}


static gboolean play_callback(GstBus *bus, GstMessage *msg, gpointer arg)
{
	(void)bus;
	struct play_info *play_info = (struct play_info *)arg;
	gchar *debug_info = NULL;
	GError *error = NULL;
	switch (GST_MESSAGE_TYPE(msg))
	{ // Options: https://gstreamer.freedesktop.org/documentation/gstreamer/gstmessage.html?gi-language=c#enumerations
		case GST_MESSAGE_EOS:
			if(play_info->loop)
			{ // Ex: Ring
				printf("Checkpoint ring 2: play_callback\n");
				if(play_info->n > -1) // This should never trigger because loop and n should never both be set
					error_simple(0,"Loop and n should never be both set. Coding error. Report this to UI devs."); // t_peer[play_info->n].t_cache_info.playing = 1;
				playback_start(play_info);
			}
			else if(play_info->n > -1 && torx_allocation_len(t_peer[play_info->n].t_cache_info.audio_cache)/sizeof(unsigned char *))
			{ // Ex: Streaming audio
				torx_free((void*)&play_info->data); // necessary to free
				t_peer[play_info->n].t_cache_info.playing = 0; // necessary or cache_play will not function
				cache_play(play_info->n);
			}
			else
			{ // Ex: Voice message
				if(play_info->n > -1)
					t_peer[play_info->n].t_cache_info.playing = 0;
				playback_stop(play_info);
			}
			break;
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &error, &debug_info);
			g_free(debug_info);
			error_printf(0,"GstMessage: %s",error->message);
			g_error_free(error);
			break;
		default:
			break;
	}
//	gst_message_unref(msg); // NO, causes errors, unnecessary here
	return TRUE;
}

void playback_async(struct play_info *play_info,const uint8_t loop,const unsigned char *data, const size_t data_len)
{ // Play audio, without checking whether there is already audio playing.
	if(!play_info || !data || !data_len || play_info->pipeline || play_info->appsrc)
	{
		error_simple(0,"Sanity check failed in playback_async. Coding error. Report this.");
		return;
	}
	play_info->data = torx_secure_malloc(data_len); // will be free'd by bus_callback
	memcpy(play_info->data,data,data_len);
	play_info->loop = loop;
	play_info->n = -1;
	play_info->callback = play_callback;
printf("Checkpoint ring 0: playback_async\n");
	playback_start(play_info);
}

void cache_play(const int n)
{ // For streaming audio.
	uint32_t count;
	if(!t_peer[n].t_cache_info.playing && (count = torx_allocation_len(t_peer[n].t_cache_info.audio_cache)/sizeof(unsigned char *)))
	{
		t_peer[n].t_cache_info.last_played_time = t_peer[n].t_cache_info.audio_time[0]; // Important
		t_peer[n].t_cache_info.last_played_nstime = t_peer[n].t_cache_info.audio_nstime[0]; // Important

		struct play_info play_info = {0}; // will be free'd by bus_callback
		play_info.data = t_peer[n].t_cache_info.audio_cache[0]; // will be free'd by bus_callback
		t_peer[n].t_cache_info.audio_cache[0] = NULL; // highly necessary!!!

		play_info.pipeline = t_peer[n].t_cache_info.stream_pipeline;
		play_info.loop = 0;
		play_info.n = n;
		play_info.callback = play_callback;

		t_peer[n].t_cache_info.audio_cache = torx_realloc_shift(t_peer[n].t_cache_info.audio_cache,(count - 1) * sizeof(unsigned char *),1); // torx_realloc(
		t_peer[n].t_cache_info.audio_time = torx_realloc_shift(t_peer[n].t_cache_info.audio_time,(count - 1) * sizeof(time_t),1);
		t_peer[n].t_cache_info.audio_nstime = torx_realloc_shift(t_peer[n].t_cache_info.audio_nstime,(count - 1) * sizeof(time_t),1);

		t_peer[n].t_cache_info.playing = 1;
		playback_start(&play_info);
	}
}

static void playback_message(void* arg)
{
	if(!arg)
		error_simple(0,"Playback_message passed null. Coding error. Report this.");
	const int n = vptoii_n(arg);
	const int i = vptoii_i(arg);
	if(current_play_pausable.pipeline)
	{
		playback_stop(&current_play_pausable);
		if(last_played_n == n && last_played_i == i)
			return;
	}
	last_played_n = n;
	last_played_i = i;
	uint32_t len;
	char *message = getter_string(&len,n,i,-1,offsetof(struct message_list,message));
	playback_async(&current_play_pausable,0,(unsigned char *)&message[4],len-4);
	torx_free((void*)&message);
	if(t_peer[n].t_message[i].unheard && getter_uint8(n,i,-1,offsetof(struct message_list,stat)) == ENUM_MESSAGE_RECV)
	{
		t_peer[n].t_message[i].unheard = 0;
		message_extra(n,i,&t_peer[n].t_message[i].unheard,sizeof(t_peer[n].t_message[i].unheard));
		ui_print_message(n,i,2);
	}
}

void ring_start(void)
{ // Start ringing
	if(current_play_ringtone.pipeline)
		return; // Already ringing
	if(current_play_pausable.pipeline)
	{
		error_simple(0,"Not ringing because other audio is playing.");
		return; // Don't ring if something else is playing (such as playback_message)
	}
	GBytes *bytes = g_resources_lookup_data("/org/torx/gtk4/other/beep.wav",G_RESOURCE_LOOKUP_FLAGS_NONE,NULL);
	size_t size = 0;
	const void *data = g_bytes_get_data(bytes,&size);
	playback_async(&current_play_ringtone,1,data,size);
}

void ring_stop(void)
{ // Stop ringing
	if(current_play_ringtone.pipeline)
		playback_stop(&current_play_ringtone);
}

static void beep(void)
{ // This function currently does not need to run in the UI thread, which is why it is not ui_prefixed
	if(current_play_beep.pipeline)
		return; // Already beeping
	GBytes *bytes = g_resources_lookup_data("/org/torx/gtk4/other/beep.wav",G_RESOURCE_LOOKUP_FLAGS_NONE,NULL);
	size_t size = 0;
	const void *data = g_bytes_get_data(bytes,&size);
	playback_async(&current_play_beep,0,data,size);
/*	#ifdef WIN32
	{ // call PlaySound asyncronously to prevent having to CreateProcess
		PlaySound(FILENAME_BEEP, NULL, SND_FILENAME | SND_ASYNC);
	}
	#else
	{
		pid_t pid;
		if((pid = fork()) == -1)
			error_simple(-1,"fork");
		if(pid == 0)
		{ // Alternatively, gresource can probably provide the audio to stdin on *nix but probably not on windows
			if(execlp("paplay","paplay",FILENAME_BEEP,NULL))
				if(execlp("aplay","aplay","-q",FILENAME_BEEP,NULL))
					if(execlp("afplay","afplay","-q",FILENAME_BEEP,NULL)) // OSX, untested
						error_printf(0,"Error finding paplay, aplay, or afplay, or beep file missing: %s",FILENAME_BEEP); // DO NOT MAKE error_ll, as its forked
			exit(0); // TODO wait() or waitpid() to clean up
		}
	}
	#endif
*/
}

static int error_idle(void *arg)
{
	char *error_message = (char *) arg;
	if(t_main.window != none && GTK_IS_WIDGET(t_main.main_window)) // main_window // TODO GTK_IS_WIDGET can lead to invalid reads, including here on shutdown
	{
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(t_main.torx_log_buffer,&end);
		gtk_text_buffer_insert(t_main.torx_log_buffer,&end,error_message,-1);
		if(t_main.window == window_log_torx)
			g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
	}
	torx_free(&arg);
	return 0;
}

void error_cb_ui(char *error_message)
{ // Prints to internal TorX Log if available, otherwise stderr
	if(!error_message)
		return;
	if(ALPHA_VERSION)
		fprintf(stderr,"Err: %s",error_message); // TODO remove prior to release
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,error_idle,error_message,NULL); // frees pointer*
}

void fatal_cb_ui(char *error_message)
{
	ui_notify(text_fatal_error,error_message);
	fprintf(stderr,"%s: %s",text_fatal_error,error_message);
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,error_idle,error_message,NULL); // frees pointer*
}

static void ui_show_log_tor(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_log_tor;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_tor_log);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */
	GtkWidget *view = gtk_text_view_new_with_buffer (t_main.tor_log_buffer);
	gtk_widget_set_hexpand(view, TRUE);	// redundant/default
	gtk_widget_set_vexpand(view, TRUE);	// required
	gtk_text_view_set_editable (GTK_TEXT_VIEW (view), FALSE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (view), TRUE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), view);
//	g_timeout_add(100,tor_log_idle,NULL); // continues until returns 0. this is good code.
}

static void ui_show_log_torx(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_log_torx;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_torx_log);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */
	if(!vertical_mode)
	{
		GtkWidget *label_log_torx_notice = gtk_label_new (text_debug_level_notice);
		gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), label_log_torx_notice);
	}
	GtkWidget *label_log_torx = gtk_label_new (text_debug_level);
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), label_log_torx);
	GtkWidget *spinbutton_debug = gtk_spin_button_new_with_range (0,5,1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton_debug),torx_debug_level(-1));
	g_signal_connect(spinbutton_debug, "unrealize", G_CALLBACK(ui_spin_change),itovp(ENUM_SPIN_DEBUG)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), spinbutton_debug);

//	GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
//	gtk_text_buffer_set_text(buffer,"scrolling contents of error() should be here",-1);
	GtkWidget *view = gtk_text_view_new_with_buffer (t_main.torx_log_buffer);
	gtk_widget_set_hexpand(view, TRUE);	// redundant/default
	gtk_widget_set_vexpand(view, TRUE);	// required
	gtk_text_view_set_editable (GTK_TEXT_VIEW (view), FALSE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (view), TRUE);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), view);
//	g_timeout_add(100,torx_log_idle,NULL);
}

static void ui_show_global_killcode(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_global_killcode;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_global_kill);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */
	GtkWidget *label_emit_global_killcode_warning = gtk_label_new(text_global_kill_warning);
	gtk_label_set_wrap(GTK_LABEL(label_emit_global_killcode_warning), TRUE);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), label_emit_global_killcode_warning);

	GtkWidget *button_emit_global_killcode = button_emit_global_killcode = gtk_button_new_with_label (text_emit_global_kill);
	g_signal_connect(button_emit_global_killcode, "clicked", G_CALLBACK (ui_toggle_kill),itovp(-1)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), button_emit_global_killcode);
}

static void ui_stack_switched(GtkWidget *stack)
{
	treeview_n = -1;
	t_main.stack_current = gtk_stack_get_visible_child_name(GTK_STACK(stack));
	gtk_widget_set_visible(t_main.button_accept,FALSE);
	gtk_widget_set_visible(t_main.button_reject,FALSE);
	gtk_widget_set_visible(t_main.button_copy,FALSE);
	gtk_widget_set_visible(t_main.button_show_qr,FALSE);
	gtk_widget_set_visible(t_main.button_delete,FALSE);
//printf("Checkpoint stack_switched %s\n",t_main.stack_current);
	if(!strncmp(t_main.stack_current,"3",1))
		ui_populate_list(itovp(ENUM_OWNER_CTRL));
	else if(!strncmp(t_main.stack_current,"4",1))
		ui_populate_list(itovp(ENUM_OWNER_PEER));
	else if(!strncmp(t_main.stack_current,"5",4))
		ui_populate_list(itovp(ENUM_OWNER_SING));
	else if(!strncmp(t_main.stack_current,"6",1))
		ui_populate_list(itovp(ENUM_OWNER_MULT));
	else
	{
		error_simple(0,"Unknown stack switched. UI Bug. Report this.");
		breakpoint();
	}
}

static void ui_n_from_treeview(gpointer data)
{ // need to roll through peer[n].onion to set treeview_n
	const int n = vptoi(data);
	if(!t_main.stack_current)
	{
		error_simple(0,"ui_n_from_treeview null stack. Coding error. Report this.");
		breakpoint();
		treeview_n = -1;
		return;
	}
	else if(!strncmp(t_main.stack_current,"3",1))
	{
		gtk_widget_set_visible(t_main.button_accept,TRUE);
		gtk_widget_set_visible(t_main.button_reject,TRUE);
	}
	else if(!strncmp(t_main.stack_current,"4",1))
	{
		gtk_widget_set_visible(t_main.button_delete,TRUE);
		gtk_widget_set_visible(t_main.button_copy,TRUE);
	}
	else if(!strncmp(t_main.stack_current,"5",1))
	{
		gtk_widget_set_visible(t_main.button_delete,TRUE);
		gtk_widget_set_visible(t_main.button_copy,TRUE);
		gtk_widget_set_visible(t_main.button_show_qr,TRUE);
	}
	else if(!strncmp(t_main.stack_current,"6",1))
	{
		gtk_widget_set_visible(t_main.button_delete,TRUE);
		gtk_widget_set_visible(t_main.button_copy,TRUE);
		gtk_widget_set_visible(t_main.button_show_qr,TRUE);
	}
	else
	{
		error_simple(0,"GTK issue, unexpected stack. Report this.");
		breakpoint();
		treeview_n = -1; // should not occur. could remove this check and else, for brevity
		return;
	}
	char byte = '\0';
	getter_array(&byte,1,n,INT_MIN,-1,offsetof(struct peer_list,onion));
	if(byte != '\0')
		treeview_n = n;
	else
		treeview_n = -1; // should not occur. could remove this check and else, for brevity
}

static void ui_rename(GtkCellEditable *self,GParamSpec *pspec,gpointer data)
{ // Make iter editable, and have a signal for after it gets changed
	(void) pspec;
	const int n = vptoi(data);
	if(!gtk_editable_label_get_editing(GTK_EDITABLE_LABEL(self)))
	{
		char *p = gtk_editable_get_chars(GTK_EDITABLE(self),0,56);
		if(p && strlen(p) && p[0] != ' ')
		{
			change_nick(n,p);
			g_free(p);
			p = NULL;
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
			ui_populate_list(itovp(owner));
		}
	}
}

static void ui_accept(void)
{
	if(treeview_n < 0)
		return;
	totalIncoming--;
	ui_decorate_panel_left_top();
	peer_accept(treeview_n);
	ui_populate_list(itovp(ENUM_OWNER_CTRL));
	ui_populate_peers(itovp(ENUM_STATUS_FRIEND));
}

static void ui_delete(void)
{
	if(treeview_n < 0)
		return;
	gtk_widget_set_visible(t_main.button_accept,FALSE);
	gtk_widget_set_visible(t_main.button_reject,FALSE);
	gtk_widget_set_visible(t_main.button_copy,FALSE);
	gtk_widget_set_visible(t_main.button_show_qr,FALSE);
	gtk_widget_set_visible(t_main.button_delete,FALSE);
	const uint8_t owner = getter_uint8(treeview_n,INT_MIN,-1,offsetof(struct peer_list,owner)); // in case of delete
	const int peer_index = getter_int(treeview_n,INT_MIN,-1,offsetof(struct peer_list,peer_index));
	if(owner == ENUM_OWNER_CTRL)
	{
		totalIncoming--;
		ui_decorate_panel_left_top();
	}
	takedown_onion(peer_index,1);
	ui_populate_list(itovp(owner));
}

static void ui_copy(GtkWidget *button,const gpointer data)
{
	(void) button;
	int n;
	if(data)
		n = vptoi(data);
	else
		n = treeview_n;
	if(n < 0)
	{
		gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),"");
		return;
	}
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL);
		const uint8_t g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
		if(!g_invite_required)
		{ // Public gropu, copy group_id
			unsigned char id[GROUP_ID_SIZE];
			pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
			memcpy(id,group[g].id,sizeof(id));
			pthread_rwlock_unlock(&mutex_expand_group); // 🟩
			char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
			sodium_memzero(id,sizeof(id));
			gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),group_id_encoded);
			torx_free((void*)&group_id_encoded);
		} // else { Coding error. Copying a private group ID or viewing QR code serves no purpose. }
	}
	else
	{
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		{
			char torxid[52+1];
			getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,offsetof(struct peer_list,torxid));
			gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),torxid);
			sodium_memzero(torxid,sizeof(torxid));
		}
		else
		{
			char onion[56+1];
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,onion));
			gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),onion);
			sodium_memzero(onion,sizeof(onion));
		}
	}
}

static void ui_show_qr(void)
{
	if(treeview_n < 0)
		return;
	char torxid[52+1];
	getter_array(&torxid,sizeof(torxid),treeview_n,INT_MIN,-1,offsetof(struct peer_list,torxid));

	GtkWidget *generated_qr_code;
	struct qr_data *qr_data;
	if(QR_IS_PNG)
	{
		generated_qr_code = gtk_image_new();
		qr_data = qr_bool(torxid,8);
		size_t png_size = 0;
		void* png_data = return_png(&png_size,qr_data);
		GBytes *bytes = g_bytes_new(png_data,png_size);
		GdkTexture *texture = gdk_texture_new_from_bytes(bytes,NULL);
		gtk_image_set_from_paintable(GTK_IMAGE(generated_qr_code),GDK_PAINTABLE(texture));
		torx_free((void*)&png_data);
	}
	else
	{
		qr_data = qr_bool(torxid,1);
		char *qr = qr_utf8(qr_data);
		if(qr == NULL)
			return; // bug
		generated_qr_code = gtk_label_new(qr);
		torx_free((void*)&qr);
	}
	gtk_widget_set_size_request(generated_qr_code,size_box_qr_code,size_box_qr_code);

	t_main.popover_qr = custom_popover_new(t_main.button_show_qr);
	GtkWidget *button_copy_qr = gtk_button_new_with_label (text_copy_qr);
	g_signal_connect_swapped(button_copy_qr, "clicked", G_CALLBACK(ui_copy_qr),itovp(treeview_n)); // DO NOT FREE arg because this only gets passed ONCE.
	GtkWidget *button_save_qr = gtk_button_new_with_label (text_save_qr);
	g_signal_connect_swapped(button_save_qr, "clicked", G_CALLBACK(ui_save_qr),itovp(treeview_n)); // DO NOT FREE arg because this only gets passed ONCE.
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_ten);
	gtk_widget_add_css_class(box, "popover_inner");
	gtk_box_append(GTK_BOX(box),generated_qr_code);
	gtk_box_append(GTK_BOX(box),button_copy_qr);
	gtk_box_append(GTK_BOX(box),button_save_qr);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_qr),box);
	gtk_popover_popup(GTK_POPOVER(t_main.popover_qr));
//	sodium_memzero(qr,qr_data->size_allocated);
	sodium_memzero(torxid,sizeof(torxid));
	torx_free((void*)&qr_data->data);
	torx_free((void*)&qr_data);
}

static gboolean ui_treeview_toggled(GtkSwitch *self,gboolean state,gpointer data)
{
	(void) self;
	(void) state; // TRUE or FALSE
	(void) data;
	const int n = vptoi(data);
//	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	char byte = '\0';
	getter_array(&byte,1,n,INT_MIN,-1,offsetof(struct peer_list,onion));
	if(byte != '\0')
	{
		treeview_n = n;
		block_peer(n);
	//why?	ui_populate_list(itovp(owner));
	}
	else
		treeview_n = -1; // should not occur. could remove this check and else, for brevity
	return FALSE;
}

static void item_builder(GtkListItemFactory *factory, GtkListItem *list_item, gpointer data)
{ // This is for Home page list items
	(void) factory;
	const int column_number = vptoi(data);
	IntPair *pair = gtk_list_item_get_item(list_item);
	if(pair)
	{
		const int n = pair->first;
		const int list = pair->second;
		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
		if(column_number == 0)
		{
			char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			GtkWidget *label = gtk_editable_label_new(peernick);
			g_signal_connect(label, "notify::editing", G_CALLBACK(ui_rename),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
			torx_free((void*)&peernick);
			gtk_box_append(GTK_BOX(box), label);
		}
		if(column_number == 1)
		{
			char onion[56+1];
			if(list == ENUM_OWNER_PEER)
				getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,peeronion));
			else
				getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,onion));
			GtkWidget *label = gtk_label_new(onion);
			sodium_memzero(onion,sizeof(onion));
			gtk_box_append(GTK_BOX(box), label);
		}
		else if(column_number == 2)
		{
			char torxid[52+1];
			getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,offsetof(struct peer_list,torxid));
			GtkWidget *label = gtk_label_new(torxid);
			sodium_memzero(torxid,sizeof(torxid));
			gtk_box_append(GTK_BOX(box), label);
		}
		else if(column_number == 3)
		{
			const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
			GtkWidget *toggle = gtk_switch_new();
			if(status == ENUM_STATUS_FRIEND)
				gtk_switch_set_active(GTK_SWITCH(toggle),TRUE);
			else
				gtk_switch_set_active(GTK_SWITCH(toggle),FALSE);
			g_signal_connect(toggle, "state-set", G_CALLBACK(ui_treeview_toggled), itovp(n));
			gtk_box_append(GTK_BOX(box), toggle);
		}
		else if(column_number)
		{
			error_simple(0,"Item_builder. Coding error. Report this.");
			return; // bug
		}
		GtkGesture *long_press = gtk_gesture_long_press_new(); // NOTE: "cancelled" == single click, "pressed" == long press
		gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(long_press),LONG_PRESS_TIME);
		g_signal_connect_swapped(long_press, "cancelled", G_CALLBACK(ui_n_from_treeview),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		g_signal_connect_swapped(long_press, "pressed", G_CALLBACK(ui_n_from_treeview),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(box, GTK_EVENT_CONTROLLER(long_press));

		gtk_list_item_set_child(list_item, box);
	}
}

static void ui_show_home(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_home;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_home);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

//	treeview_n = -1;

	/* Window content, below here, should be appended to t_main.scroll_box_right */

	GtkWidget *button_log_tor = gtk_button_new_with_label (text_tor_log);
	g_signal_connect(button_log_tor, "clicked", G_CALLBACK (ui_show_log_tor), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), button_log_tor);

	GtkWidget *button_log_torx = gtk_button_new_with_label (text_torx_log);
	g_signal_connect(button_log_torx, "clicked", G_CALLBACK (ui_show_log_torx), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), button_log_torx);

	GtkWidget *button_kill_global = gtk_button_new_with_label (vertical_mode ? text_vertical_emit_global_kill : text_emit_global_kill);
	g_signal_connect(button_kill_global, "clicked", G_CALLBACK (ui_show_global_killcode), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), button_kill_global);

	/* Add viewport to each scrolled window */

	GtkListItemFactory* cellrenderer_home_text_identifier_incoming = gtk_signal_list_item_factory_new();
	GtkListItemFactory* cellrenderer_home_text_identifier_outgoing = gtk_signal_list_item_factory_new();
	GtkListItemFactory* cellrenderer_home_text_identifier_mult = gtk_signal_list_item_factory_new();
	GtkListItemFactory* cellrenderer_home_text_identifier_sing = gtk_signal_list_item_factory_new();
	GtkListItemFactory* cellrenderer_home_text_onion = gtk_signal_list_item_factory_new();
	GtkListItemFactory* cellrenderer_home_toggle_mult = gtk_signal_list_item_factory_new();
	GtkListItemFactory* cellrenderer_home_toggle_sing = gtk_signal_list_item_factory_new();

	g_signal_connect(cellrenderer_home_text_identifier_incoming, "bind", G_CALLBACK(item_builder), itovp(0));
	g_signal_connect(cellrenderer_home_text_identifier_outgoing, "bind", G_CALLBACK(item_builder), itovp(0));
	g_signal_connect(cellrenderer_home_text_identifier_mult, "bind", G_CALLBACK(item_builder), itovp(0));
	g_signal_connect(cellrenderer_home_text_identifier_sing, "bind", G_CALLBACK(item_builder), itovp(0));
	if(shorten_torxids)
		g_signal_connect(cellrenderer_home_text_onion, "bind", G_CALLBACK(item_builder), itovp(2));
	else
		g_signal_connect(cellrenderer_home_text_onion, "bind", G_CALLBACK(item_builder), itovp(1));
	g_signal_connect(cellrenderer_home_toggle_mult, "bind", G_CALLBACK(item_builder), itovp(3));
	g_signal_connect(cellrenderer_home_toggle_sing, "bind", G_CALLBACK(item_builder), itovp(3));

	GtkColumnViewColumn *column_incoming_identifier = gtk_column_view_column_new(text_identifier,cellrenderer_home_text_identifier_incoming);
	GtkColumnViewColumn *column_outgoing_identifier = gtk_column_view_column_new(text_identifier,cellrenderer_home_text_identifier_outgoing);
	GtkColumnViewColumn *column_mult_identifier = gtk_column_view_column_new(text_identifier,cellrenderer_home_text_identifier_mult);
	GtkColumnViewColumn *column_sing_identifier = gtk_column_view_column_new(text_identifier,cellrenderer_home_text_identifier_sing);
	GtkColumnViewColumn *column_mult_active = gtk_column_view_column_new(text_active,cellrenderer_home_toggle_mult); // XXX toggle
	GtkColumnViewColumn *column_sing_active = gtk_column_view_column_new(text_active,cellrenderer_home_toggle_sing); // XXX toggle

	GtkColumnViewColumn *column_incoming_onion;
	GtkColumnViewColumn *column_outgoing_onion;
	GtkColumnViewColumn *column_mult_onion;
	GtkColumnViewColumn *column_sing_onion;

	if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
	{
		column_incoming_onion = gtk_column_view_column_new(text_torxid,cellrenderer_home_text_onion);
		column_outgoing_onion = gtk_column_view_column_new(text_torxid,cellrenderer_home_text_onion);
		column_mult_onion = gtk_column_view_column_new(text_torxid,cellrenderer_home_text_onion);
		column_sing_onion = gtk_column_view_column_new(text_torxid,cellrenderer_home_text_onion);
	}
	else
	{
		column_incoming_onion = gtk_column_view_column_new(text_onionid,cellrenderer_home_text_onion);
		column_outgoing_onion = gtk_column_view_column_new(text_onionid,cellrenderer_home_text_onion);
		column_mult_onion = gtk_column_view_column_new(text_onionid,cellrenderer_home_text_onion);
		column_sing_onion = gtk_column_view_column_new(text_onionid,cellrenderer_home_text_onion);
	}

	t_main.treestore_incoming = g_list_store_new(int_pair_get_type());
	t_main.treestore_outgoing = g_list_store_new(int_pair_get_type());
	t_main.treestore_mult = g_list_store_new(int_pair_get_type());
	t_main.treestore_sing = g_list_store_new(int_pair_get_type());

	ui_populate_list(itovp(ENUM_OWNER_CTRL)); // only need the default one. the rest will be handled when switching stacks. EDIT: nope, not currently.
	ui_populate_list(itovp(ENUM_OWNER_PEER));
	ui_populate_list(itovp(ENUM_OWNER_MULT));
	ui_populate_list(itovp(ENUM_OWNER_SING));

	t_main.treeview_incoming = gtk_column_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(t_main.treestore_incoming))));
	t_main.treeview_outgoing = gtk_column_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(t_main.treestore_outgoing))));
	t_main.treeview_mult = gtk_column_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(t_main.treestore_mult))));
	t_main.treeview_sing = gtk_column_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(t_main.treestore_sing))));

	gtk_column_view_set_single_click_activate(GTK_COLUMN_VIEW(t_main.treeview_incoming),FALSE);
	gtk_column_view_set_single_click_activate(GTK_COLUMN_VIEW(t_main.treeview_outgoing),FALSE);
	gtk_column_view_set_single_click_activate(GTK_COLUMN_VIEW(t_main.treeview_mult),FALSE);
	gtk_column_view_set_single_click_activate(GTK_COLUMN_VIEW(t_main.treeview_sing),FALSE);

	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_incoming),column_incoming_identifier);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_incoming),column_incoming_onion);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_outgoing),column_outgoing_identifier);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_outgoing),column_outgoing_onion);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_mult),column_mult_active);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_mult),column_mult_identifier);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_mult),column_mult_onion);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_sing),column_sing_active);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_sing),column_sing_identifier);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(t_main.treeview_sing),column_sing_onion);

	gtk_widget_set_vexpand(t_main.treeview_incoming, TRUE);
	gtk_widget_set_vexpand(t_main.treeview_outgoing, TRUE);
	gtk_widget_set_vexpand(t_main.treeview_mult, TRUE);
	gtk_widget_set_vexpand(t_main.treeview_sing, TRUE);

	/* Add treeview to each viewport */
	GtkWidget *viewport_incoming = gtk_viewport_new (NULL,NULL);
	GtkWidget *viewport_outgoing = gtk_viewport_new (NULL,NULL);
	GtkWidget *viewport_mult = gtk_viewport_new (NULL,NULL);
	GtkWidget *viewport_sing = gtk_viewport_new (NULL,NULL);

	gtk_viewport_set_child (GTK_VIEWPORT(viewport_incoming),t_main.treeview_incoming);
	gtk_viewport_set_child (GTK_VIEWPORT(viewport_outgoing),t_main.treeview_outgoing);
	gtk_viewport_set_child (GTK_VIEWPORT(viewport_mult),t_main.treeview_mult);
	gtk_viewport_set_child (GTK_VIEWPORT(viewport_sing),t_main.treeview_sing);

	/* Put viewports each in a scrolled window */
	GtkWidget *scrolled_window_incoming = gtk_scrolled_window_new();
	GtkWidget *scrolled_window_outgoing = gtk_scrolled_window_new();
	GtkWidget *scrolled_window_mult = gtk_scrolled_window_new();
	GtkWidget *scrolled_window_sing = gtk_scrolled_window_new();

        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_incoming),viewport_incoming);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_outgoing),viewport_outgoing);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_mult),viewport_mult);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_sing),viewport_sing);

	/* Build stack switcher and stack */
	GtkWidget *stack = gtk_stack_new();
	gtk_stack_add_titled (GTK_STACK (stack), scrolled_window_incoming, "3",text_incoming);
	gtk_stack_add_titled (GTK_STACK (stack), scrolled_window_outgoing, "4",text_outgoing);
	gtk_stack_add_titled (GTK_STACK (stack), scrolled_window_mult, "5",text_active_mult);
	gtk_stack_add_titled (GTK_STACK (stack), scrolled_window_sing, "6",text_active_sing);
	t_main.stack_current = gtk_stack_get_visible_child_name(GTK_STACK(stack)); // set a default
//	GtkWidget* stack_switcher = gtk_stack_switcher_new();
	GtkWidget *custom_switcher = gtk_custom_switcher_new(GTK_STACK (stack),vertical_mode ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL,1);
//	gtk_stack_switcher_set_stack (GTK_STACK_SWITCHER(stack_switcher),GTK_STACK (stack));
	g_signal_connect(stack, "notify::visible-child", G_CALLBACK(ui_stack_switched), NULL); // DO NOT FREE arg because this only gets passed ONCE.

	gtk_box_append(GTK_BOX(t_main.scroll_box_right),custom_switcher);
//	gtk_box_append(GTK_BOX(t_main.scroll_box_right),stack_switcher);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),stack);

	/* Bottom buttons */
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	t_main.button_accept = gtk_button_new_with_label (text_accept);
	t_main.button_reject = gtk_button_new_with_label (text_reject);
	t_main.button_copy = gtk_button_new_with_label (text_copy);
	t_main.button_show_qr = gtk_button_new_with_label (text_show_qr);
	t_main.button_delete = gtk_button_new_with_label (text_delete);

	gtk_widget_set_visible(t_main.button_accept,FALSE);
	gtk_widget_set_visible(t_main.button_reject,FALSE);
	gtk_widget_set_visible(t_main.button_copy,FALSE);
	gtk_widget_set_visible(t_main.button_show_qr,FALSE);
	gtk_widget_set_visible(t_main.button_delete,FALSE);

	g_signal_connect(t_main.button_accept, "clicked", G_CALLBACK(ui_accept), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(t_main.button_reject, "clicked", G_CALLBACK(ui_delete), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(t_main.button_delete, "clicked", G_CALLBACK(ui_delete), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(t_main.button_copy, "clicked", G_CALLBACK(ui_copy), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(t_main.button_show_qr, "clicked", G_CALLBACK(ui_show_qr), NULL); // DO NOT FREE arg because this only gets passed ONCE.

	gtk_box_append(GTK_BOX(box),t_main.button_accept);
	gtk_box_append(GTK_BOX(box),t_main.button_reject);
	gtk_box_append(GTK_BOX(box),t_main.button_delete);
	gtk_box_append(GTK_BOX(box),t_main.button_copy);
	gtk_box_append(GTK_BOX(box),t_main.button_show_qr);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(t_main.scroll_box_right),box);
}

static void ui_group_join(void* arg)
{
	int ret;
	if(arg)
	{ // Responding to invite
		const int inviter_n = vptoii_n(arg);
		const int inviter_i = vptoii_i(arg);
		ret = group_join_from_i(inviter_n,inviter_i);
	}
	else
	{ // Joining public group
		const char *encoded_id = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_group_id)));
		unsigned char id[GROUP_ID_SIZE];
		if(b64_decode(id,sizeof(id),encoded_id) != GROUP_ID_SIZE)
		{
			ui_input_bad(t_main.entry_add_group_id);
			error_simple(0,"Failed to attempt to join public group.");
			return;
		}
		const char *name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_group_identifier)));
		ret = group_join(-1,id,name,NULL,NULL);
		sodium_memzero(id,sizeof(id));
	}
	if(arg && ret > -1)
	{}
	else if(ret > -1)
	{ // clear entries (public group)
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_group_id)),0,-1);
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_group_identifier)),0,-1);
	}
	else
		error_simple(0,"Failed to attempt to join a group of some type.");
	// TODO should probably indicate to the user what is wrong (probably empty group name or invalid length b64)
	ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
}

static void ui_group_generate(const void *arg)
{
	const uint8_t invite_required = (uint8_t)vptoi(arg);
	const char *name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_generate_group)));
	const int g = group_generate(invite_required,name);
	if(g > -1)
	{
		const int group_n = getter_group_int(g,offsetof(struct group_list,n));
		const int g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_generate_group)),0,-1);
		if(g_invite_required)
		{
			gtk_text_buffer_set_text(t_main.textbuffer_generated_group,text_successfully_created_group,-1);
			gtk_widget_set_visible(t_main.generated_qr_group,FALSE);
			gtk_widget_set_visible(t_main.button_copy_generated_qr,FALSE);
			gtk_widget_set_visible(t_main.button_save_generated_qr,FALSE);
		}
		else
		{ // public group, show ID and QR code
			unsigned char id[GROUP_ID_SIZE];
			pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
			memcpy(id,group[g].id,sizeof(id));
			pthread_rwlock_unlock(&mutex_expand_group); // 🟩
			char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
			sodium_memzero(id,sizeof(id));
			gtk_text_buffer_set_text(t_main.textbuffer_generated_group,group_id_encoded,-1);
			struct qr_data *qr_data;
			if(QR_IS_PNG)
			{
				qr_data = qr_bool(group_id_encoded,8);
				size_t png_size = 0;
				void* png_data = return_png(&png_size,qr_data);
				GBytes *bytes = g_bytes_new(png_data,png_size);
				GdkTexture *texture = gdk_texture_new_from_bytes(bytes,NULL);
				gtk_image_set_from_paintable(GTK_IMAGE(t_main.generated_qr_group),GDK_PAINTABLE(texture));
				torx_free((void*)&png_data);
			}
			else
			{
				qr_data = qr_bool(group_id_encoded,1);
				char *qr = qr_utf8(qr_data);
				gtk_label_set_text(GTK_LABEL(t_main.generated_qr_group),qr);
				torx_free((void*)&qr);
			}
			torx_free((void*)&group_id_encoded);
			torx_free((void*)&qr_data->data);
			torx_free((void*)&qr_data);
			g_signal_connect_swapped(t_main.button_copy_generated_qr, "clicked", G_CALLBACK(ui_copy_qr),itovp(group_n));
			g_signal_connect_swapped(t_main.button_save_generated_qr, "clicked", G_CALLBACK(ui_save_qr),itovp(group_n)); // DO NOT FREE arg because this only gets passed ONCE.
			gtk_widget_set_visible(t_main.generated_qr_group,TRUE);
			gtk_widget_set_visible(t_main.button_copy_generated_qr,TRUE);
			gtk_widget_set_visible(t_main.button_save_generated_qr,TRUE);
		}
		ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
	}
	else
	{
		if(!name || !strlen(name))
			ui_input_bad(t_main.entry_generate_group);
		gtk_text_buffer_set_text(t_main.textbuffer_generated_group,text_error_creating_group,-1);
		gtk_widget_set_visible(t_main.generated_qr_group,FALSE);
		gtk_widget_set_visible(t_main.button_copy_generated_qr,FALSE);
		gtk_widget_set_visible(t_main.button_save_generated_qr,FALSE);
	}
}

static void ui_show_group_generate(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_group_generate;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_group);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */

	// Add Public Group
	GtkWidget *subtitle_add = gtk_label_new(text_add_group_by);
	gtk_widget_add_css_class(subtitle_add, "page_sub_title");
	gtk_widget_set_margin_top(subtitle_add, size_margin_fifteen);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), subtitle_add);
	GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_eight);
	t_main.entry_add_group_identifier = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_add_group_identifier),text_placeholder_add_group_identifier);
	gtk_widget_set_hexpand(t_main.entry_add_group_identifier, TRUE);	// works, do not remove
	t_main.entry_add_group_id = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_add_group_id),text_placeholder_add_group_id);
	gtk_widget_set_hexpand(t_main.entry_add_group_id, TRUE);	// works, do not remove 
	GtkWidget *button_add = gtk_button_new_with_label (text_button_join);
	g_signal_connect_swapped(button_add, "clicked", G_CALLBACK (ui_group_join),NULL); // DO NOT FREE arg because this only gets passed ONCE.

	gtk_box_append (GTK_BOX (box2), t_main.entry_add_group_identifier);
	gtk_box_append (GTK_BOX (box2), t_main.entry_add_group_id);
	gtk_box_append (GTK_BOX (box1), box2);
	gtk_box_append (GTK_BOX (box1), button_add);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box1);

	// Generate
	GtkWidget *subtitle_generate = gtk_label_new(text_generate_group_for);
	gtk_widget_add_css_class(subtitle_generate, "page_sub_title");
	gtk_widget_set_margin_top(subtitle_generate, size_margin_fifteen);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), subtitle_generate);
	GtkWidget *box_4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_ten);
	GtkWidget *box_5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);

	t_main.entry_generate_group = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_generate_group),text_placeholder_add_group_identifier);
	gtk_widget_set_hexpand(t_main.entry_generate_group, TRUE);	// works, do not remove
	gtk_box_append (GTK_BOX (box_4), t_main.entry_generate_group);

	GtkWidget *button_invite = gtk_button_new_with_label (text_button_generate_invite);
	GtkWidget *button_public = gtk_button_new_with_label (text_button_generate_public);
	g_signal_connect_swapped(button_invite, "clicked", G_CALLBACK (ui_group_generate),itovp(1)); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect_swapped(button_public, "clicked", G_CALLBACK (ui_group_generate),itovp(0)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (box_5), button_invite);
	gtk_box_append (GTK_BOX (box_5), button_public);
	gtk_widget_set_halign(box_5, GTK_ALIGN_CENTER);
	gtk_box_append (GTK_BOX (box_4), box_5);

	t_main.textbuffer_generated_group = gtk_text_buffer_new(NULL);

	GtkWidget *textview_output = gtk_text_view_new_with_buffer(t_main.textbuffer_generated_group);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(textview_output),TRUE);
	gtk_widget_set_size_request(textview_output,size_zero,size_textbuffer_single_line_height); // necessary or it doesnt show up
	gtk_widget_set_hexpand(textview_output, TRUE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_output), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview_output), FALSE);
	gtk_box_append (GTK_BOX (box_4), textview_output);

	// QR Code in a Label
	t_main.button_copy_generated_qr = gtk_button_new_with_label (text_copy_qr);
	t_main.button_save_generated_qr = gtk_button_new_with_label (text_save_qr);

	if(QR_IS_PNG)
		t_main.generated_qr_group = gtk_image_new();
	else
	{
		t_main.generated_qr_group = gtk_label_new(text_qr_code);
		gtk_widget_add_css_class(t_main.generated_qr_group, "textview_qr");
		gtk_label_set_selectable(GTK_LABEL(t_main.generated_qr_group), TRUE);
	}
	gtk_widget_set_visible(t_main.generated_qr_group,FALSE);
	gtk_widget_set_visible(t_main.button_copy_generated_qr,FALSE);
	gtk_widget_set_visible(t_main.button_save_generated_qr,FALSE);
	gtk_widget_set_size_request(t_main.generated_qr_group,size_box_qr_code,size_box_qr_code); // 29 *6font
	gtk_widget_set_hexpand(t_main.generated_qr_group, FALSE);	// works, do not remove
	gtk_widget_set_vexpand(t_main.generated_qr_group, FALSE);	// works, do not remove
	gtk_box_append (GTK_BOX (box_4), t_main.generated_qr_group);
	gtk_box_append (GTK_BOX (box_4), t_main.button_copy_generated_qr);
	gtk_box_append (GTK_BOX (box_4), t_main.button_save_generated_qr);

	// Finally add generate box
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box_4);
}

static void ui_generate(const void *arg)
{ // DO NOT FREE arg because this only gets passed ONCE.
	const uint8_t owner = (uint8_t)vptoi(arg);
	generate_onion(owner,NULL,gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_generate_peernick))));
}

static void ui_add_peer(void)
{
	const char *peernick = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_peernick)));
	const char *peeronion = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_peeronion)));
	if(peer_save(peeronion,peernick) == 0) // send friend request
	{
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_peernick)),0,-1); // Clear peernick
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_add_peeronion)),0,-1); // Clear peernick
	}
	else
	{
		if(!peernick || !strlen(peernick))
			ui_input_bad(t_main.entry_add_peernick);
		if(!peeronion || !strlen(peeronion))
			ui_input_bad(t_main.entry_add_peeronion);
	}
}

static void ui_show_generate(void)
{
	if(vertical_mode > 0)
		vertical_mode = 2;
	t_main.window = window_main;
	ui_decorate_panel_left(-1);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);
	GtkWidget *title = gtk_label_new(text_add_generate);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), title);

	/* Window content, below here, should be appended to t_main.scroll_box_right */
	GtkWidget *button_group = gtk_button_new_with_label (text_group);
	g_signal_connect(button_group, "clicked", G_CALLBACK (ui_show_group_generate), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (t_main.chat_headerbar_right), button_group);

	// Add peer
	GtkWidget *subtitle_add = gtk_label_new(text_add_peer_by);
	gtk_widget_add_css_class(subtitle_add, "page_sub_title");
	gtk_widget_set_margin_top(subtitle_add, size_margin_fifteen);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), subtitle_add);
	GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_eight);
	t_main.entry_add_peernick = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_add_peernick),text_placeholder_add_identifier);
	gtk_widget_set_hexpand(t_main.entry_add_peernick, TRUE);	// works, do not remove
	t_main.entry_add_peeronion = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_add_peeronion),text_placeholder_add_onion);
	gtk_widget_set_hexpand(t_main.entry_add_peeronion, TRUE);	// works, do not remove 
	GtkWidget *button_add = gtk_button_new_with_label (text_button_add);
	g_signal_connect_swapped(button_add, "clicked", G_CALLBACK (ui_add_peer),NULL); // DO NOT FREE arg because this only gets passed ONCE.

	gtk_box_append (GTK_BOX (box2), t_main.entry_add_peernick);
	gtk_box_append (GTK_BOX (box2), t_main.entry_add_peeronion);
	gtk_box_append (GTK_BOX (box1), box2);
	gtk_box_append (GTK_BOX (box1), button_add);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box1);

	// Generate
	GtkWidget *subtitle_generate = gtk_label_new(text_generate_for);
	gtk_widget_add_css_class(subtitle_generate, "page_sub_title");
	gtk_widget_set_margin_top(subtitle_generate, size_margin_fifteen);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), subtitle_generate);
	GtkWidget *box_4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_ten);
	GtkWidget *box_5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);

	t_main.entry_generate_peernick = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_generate_peernick),text_placeholder_add_identifier);
	gtk_widget_set_hexpand(t_main.entry_generate_peernick, TRUE);	// works, do not remove
	gtk_box_append (GTK_BOX (box_4), t_main.entry_generate_peernick);

	GtkWidget *button_sing = gtk_button_new_with_label (text_button_sing);
	GtkWidget *button_mult = gtk_button_new_with_label (text_button_mult);
	g_signal_connect_swapped(button_sing, "clicked", G_CALLBACK (ui_generate),itovp(ENUM_OWNER_SING)); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect_swapped(button_mult, "clicked", G_CALLBACK (ui_generate),itovp(ENUM_OWNER_MULT)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append (GTK_BOX (box_5), button_sing);
	gtk_box_append (GTK_BOX (box_5), button_mult);
	gtk_widget_set_halign(box_5, GTK_ALIGN_CENTER);
	gtk_box_append (GTK_BOX (box_4), box_5);

	t_main.textbuffer_generated_onion = gtk_text_buffer_new(NULL);
	char torxid[52+1];
	if(generated_n > -1)
	{
		getter_array(&torxid,sizeof(torxid),generated_n,INT_MIN,-1,offsetof(struct peer_list,torxid));
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,torxid,-1);
		else
		{
			char onion[56+1];
			getter_array(&onion,sizeof(onion),generated_n,INT_MIN,-1,offsetof(struct peer_list,onion));
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,onion,-1);
			sodium_memzero(onion,sizeof(onion));
		}
	}
	GtkWidget *textview_output = gtk_text_view_new_with_buffer(t_main.textbuffer_generated_onion);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(textview_output),TRUE);
	gtk_widget_set_size_request(textview_output,size_zero,size_textbuffer_single_line_height); // necessary or it doesnt show up
	gtk_widget_set_hexpand(textview_output, TRUE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_output), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview_output), FALSE);
	gtk_box_append (GTK_BOX (box_4), textview_output);

	// QR Code in a Label
	t_main.button_copy_generated_qr = gtk_button_new_with_label (text_copy_qr);
	t_main.button_save_generated_qr = gtk_button_new_with_label (text_save_qr);

	if(generated_n > -1)
	{
		struct qr_data *qr_data;
		if(QR_IS_PNG)
		{
			t_main.generated_qr_onion = gtk_image_new();
			qr_data = qr_bool(torxid,8);
			size_t png_size = 0;
			void* png_data = return_png(&png_size,qr_data);
			GBytes *bytes = g_bytes_new(png_data,png_size);
			GdkTexture *texture = gdk_texture_new_from_bytes(bytes,NULL);
			gtk_image_set_from_paintable(GTK_IMAGE(t_main.generated_qr_onion),GDK_PAINTABLE(texture));
			torx_free((void*)&png_data);
		}
		else
		{
			qr_data = qr_bool(torxid,1);
			char *qr = qr_utf8(qr_data);
			t_main.generated_qr_onion = gtk_label_new(qr);
			torx_free((void*)&qr);
		}
		sodium_memzero(torxid,sizeof(torxid));
		torx_free((void*)&qr_data->data);
		torx_free((void*)&qr_data);
		g_signal_connect_swapped(t_main.button_copy_generated_qr, "clicked", G_CALLBACK(ui_copy_qr),itovp(generated_n));
		g_signal_connect_swapped(t_main.button_save_generated_qr, "clicked", G_CALLBACK(ui_save_qr),itovp(generated_n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_set_visible(t_main.generated_qr_onion,TRUE);
		gtk_widget_set_visible(t_main.button_copy_generated_qr,TRUE);
		gtk_widget_set_visible(t_main.button_save_generated_qr,TRUE);
	}
	else
	{
		if(QR_IS_PNG)
			t_main.generated_qr_onion = gtk_image_new();
		else
			t_main.generated_qr_onion = gtk_label_new(text_qr_code);
		gtk_widget_set_visible(t_main.generated_qr_onion,FALSE);
		gtk_widget_set_visible(t_main.button_copy_generated_qr,FALSE);
		gtk_widget_set_visible(t_main.button_save_generated_qr,FALSE);
	}
	gtk_widget_set_size_request(t_main.generated_qr_onion,size_box_qr_code,size_box_qr_code); // 29 *6font
	if(!QR_IS_PNG)
	{
		gtk_widget_add_css_class(t_main.generated_qr_onion, "textview_qr");
		gtk_label_set_selectable(GTK_LABEL(t_main.generated_qr_onion), TRUE);
	}
	gtk_widget_set_hexpand(t_main.generated_qr_onion, FALSE);	// works, do not remove
	gtk_widget_set_vexpand(t_main.generated_qr_onion, FALSE);	// works, do not remove
	gtk_box_append (GTK_BOX (box_4), t_main.generated_qr_onion);
	gtk_box_append (GTK_BOX (box_4), t_main.button_copy_generated_qr);
	gtk_box_append (GTK_BOX (box_4), t_main.button_save_generated_qr);

	// Finally add generate box
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box_4);
}

static void ui_theme(const int theme_local)
{
	if(theme_local == LIGHT_THEME)
		gtk_css_provider_load_from_resource(t_main.provider,RESOURCE_CSS_LIGHT);
	else
		gtk_css_provider_load_from_resource(t_main.provider,RESOURCE_CSS_DARK);
	gtk_style_context_add_provider_for_display(gdk_display_get_default(),GTK_STYLE_PROVIDER(t_main.provider),GTK_STYLE_PROVIDER_PRIORITY_USER);
	ui_decorate_panel_left_top();
}

static int compare(const char *suffix,const char *name,const size_t name_len) 
{
	if(!suffix || !name || !name_len)
		return 0;
	const size_t suffix_len = strlen(suffix);
	if(name_len < suffix_len)
		return 0;
	return !strncasecmp(&name[name_len-suffix_len],suffix,suffix_len);
}

static GtkWidget *ui_get_icon_from_filename(const char *name)
{ // This method is better than strcasestr because it can't accidentally trigger jpg on a .jpg.exe
	GtkWidget *file_icon = {0};
	if(name == NULL)
	{
		error_simple(2,"File name passed to get_icon_from_filename() is null.");
		file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file));
	}
	else
	{
		const size_t name_len = strlen(name);
		if(compare(".txt",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_txt));
		else if(compare(".jpeg",name,name_len) || compare(".jpg",name,name_len) || compare(".png",name,name_len) || compare(".bmp",name,name_len) || compare(".gif",name,name_len) || compare(".tiff",name,name_len) || compare(".svg",name,name_len) || compare(".psd",name,name_len) || compare(".raw",name,name_len) || compare(".webp",name,name_len) || compare(".ico",name,name_len) || compare(".dng",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_image));
		else if(compare(".mp3",name,name_len) || compare(".wav",name,name_len) || compare(".flac",name,name_len) || compare(".m4a",name,name_len) || compare(".ogg",name,name_len) || compare(".aac",name,name_len) || compare(".wma",name,name_len) || compare(".mid",name,name_len) || compare(".pcm",name,name_len) || compare(".mka",name,name_len) || compare(".opus",name,name_len) || compare(".ape",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_music));
		else if(compare(".zip",name,name_len) || compare(".rar",name,name_len) || compare(".tar",name,name_len) || compare(".deb",name,name_len) || compare(".iso",name,name_len) || compare(".7z",name,name_len) || compare(".rpm",name,name_len) || compare(".bz2",name,name_len) || compare(".gz",name,name_len) || compare(".xz",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_zip));
		else if(compare(".mkv",name,name_len) || compare(".mp4",name,name_len) || compare(".avi",name,name_len) || compare(".mov",name,name_len) || compare(".wmv",name,name_len) || compare(".flv",name,name_len) || compare(".mpg",name,name_len) || compare(".webm",name,name_len) || compare(".3gp",name,name_len) || compare(".mpv",name,name_len) || compare(".mpeg",name,name_len) || compare(".rm",name,name_len) || compare(".divx",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file)); // TODO change this, video file
		else if(compare(".pdf",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file)); // TODO change this, pdf
		else if(compare(".doc",name,name_len) || compare(".docx",name,name_len) || compare(".odt",name,name_len) || compare(".wps",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file)); // TODO change this, document
		else if(compare(".xlsx",name,name_len) || compare(".xls",name,name_len) || compare(".csv",name,name_len) || compare(".ods",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file)); // TODO change this, spreadsheet
		else if(compare(".pptx",name,name_len) || compare(".ppt",name,name_len) || compare(".odp",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file)); // TODO change this, presentation
		else if(compare(".exe",name,name_len) || compare(".msi",name,name_len) || compare(".dmg",name,name_len) || compare(".deb",name,name_len) || compare(".rpm",name,name_len) || compare(".apk",name,name_len) || compare(".jar",name,name_len) || compare(".app",name,name_len) || compare(".bin",name,name_len) || compare(".sh",name,name_len) || compare(".bat",name,name_len) || compare(".bash",name,name_len) || compare(".cmd",name,name_len) || compare(".pkg",name,name_len))
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file)); // TODO change this, executive/installer
		else
			file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(file_file));
	}
	return file_icon;
}

static int custom_setting_idle(void *arg)
{
	struct custom_setting *custom_setting = (struct custom_setting*) arg; // Casting passed struct
	const int n = custom_setting->n;
	char *setting_name = custom_setting->setting_name;
	char *setting_value = custom_setting->setting_value;
	const size_t setting_value_len = custom_setting->setting_value_len;
	const int plaintext = custom_setting->plaintext;
	if(!strncmp(setting_name,"theme",5))
	{
		global_theme = (int)strtoll(setting_value, NULL, 10);
		if(global_theme != THEME_DEFAULT)
			ui_theme(global_theme);
	}
	else if(!strncmp(setting_name,"language",8) && sizeof(language) == setting_value_len+1)
	{ // We are requiring the language to be exactly 5 characters long to be considered valid (ex: en_US)
	//	printf("Checkpoint language %s vs %s\n",language,setting_value);
		if(memcmp(language,setting_value,sizeof(language)))
		{ // Loading a different language setting. This check is to avoid unnecessarily calling ui_show_auth_screen twice.
			memcpy(language,setting_value,setting_value_len);
			language[setting_value_len] = '\0';
			ui_initialize_language(NULL);
			if(t_main.window == window_auth)
			{ // Making sure we don't try to show auth when we have already passed it
				pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
				const char *tor_location_local = tor_location;
				pthread_rwlock_unlock(&mutex_global_variable); // 🟩
				if(tor_location_local)
					ui_show_auth_screen();
				else // missing a required binary
					ui_show_missing_binaries();
			}
		}
	}
	else if(!strncmp(setting_name,"gtk4-width",10))
		size_window_default_width = (int)strtoll(setting_value, NULL, 10);
	else if(!strncmp(setting_name,"gtk4-height",11))
		size_window_default_height = (int)strtoll(setting_value, NULL, 10);
	else if(!strncmp(setting_name,"minimize_to_tray",16))
		minimize_to_tray = (uint8_t)strtoull(setting_value, NULL, 10);
	else if(!strncmp(setting_name,"save_all_stickers",17))
		save_all_stickers = (uint8_t)strtoull(setting_value, NULL, 10);
	else if(plaintext == 0 && !strncmp(setting_name,"mute",4))
		t_peer[n].mute = (int8_t)strtoll(setting_value, NULL, 10);
	else if(plaintext == 0 && !strncmp(setting_name,"unread",6))
	{
		if(log_unread != 1)
			return 0; // ignoring (potentially old)
		t_peer[n].unread = strtoull(setting_value, NULL, 10);
		if(t_peer[n].unread > 0)
		{
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
			if (owner == ENUM_OWNER_GROUP_CTRL)
				totalUnreadGroup += t_peer[n].unread;
			else
				totalUnreadPeer += t_peer[n].unread;
			ui_decorate_panel_left_top();
		}
	}
	else if(plaintext == 0 && !strncmp(setting_name,"sticker-gif-",12))
		ui_sticker_register((unsigned char *)setting_value,setting_value_len);
	else if(plaintext == 0)
		error_printf(3,"Unrecognized encrypted config option: %s",setting_name);
	else if(plaintext == 1)
		error_printf(0,"Unrecognized unencrypted config option: %s",setting_name);
	torx_free((void*)&custom_setting->setting_name);
	torx_free((void*)&custom_setting->setting_value);
	torx_free((void*)&custom_setting);
	return 0;
}

void custom_setting_cb_ui(const int n,char *setting_name,char *setting_value,const size_t setting_value_len,const int plaintext) // plaintext == 0 means it was encrypted, for safety
{ // GUI Callback from initial_keyed() and get_key() for custom UI options that are saved to our .key file (encrypted or otherwise)
	if(!setting_name || !setting_value || !setting_value_len)
	{
		error_simple(0,"Setting received by UI with either NULL name or NULL value");
		return;
	}
	struct custom_setting *custom_setting = torx_insecure_malloc(sizeof(struct custom_setting));
	custom_setting->n = n;
	custom_setting->setting_name = setting_name;
	custom_setting->setting_value = setting_value;
	custom_setting->setting_value_len = setting_value_len;
	custom_setting->plaintext = plaintext;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,custom_setting_idle,custom_setting,NULL);
}

static void ui_message_copy(const gpointer data)
{
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.4");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩
	if(null_terminated_len == 1)
	{
		char *message = getter_string(NULL,n,i,-1,offsetof(struct message_list,message));
		gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),message);
		torx_free((void*)&message);
	}
	else
		error_simple(0,"Attempting to copy a message type that hasn't yet been configured for copying");
}

static void ui_message_resend(const gpointer data)
{
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
/*	const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.7");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint16_t protocol = protocols[p_iter].protocol;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	printf("Checkpoint ui_message_resend owner==%d\n",owner);
	int target_n = n;
	if(owner == ENUM_OWNER_GROUP_PEER)
	{ // should always be true unless this is != RECV
		const int g = set_g(n,NULL);
		target_n = getter_group_int(g,offsetof(struct group_list,n));
	} */
	message_resend(n,i);
}

static void ui_message_pause(const gpointer data)
{ // file_accept alternates between accept and pause
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	int file_n;
	const int f = set_f_from_i(&file_n,n,i);
	if(f < 0)
		return;
	file_accept(file_n,f);
}
static void ui_message_cancel(const gpointer data)
{ // file_cancel() NOTE: this is for cancelling FILES not unsent messages.
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	int file_n;
	const int f = set_f_from_i(&file_n,n,i);
	if(f < 0)
		return;
	file_cancel(file_n,f);
}
static void ui_message_reject(const gpointer data)
{ // file_cancel() TODO NOTE: this should delete the related messages too.
	ui_message_cancel(data);
}

static void ui_open_folder(const gpointer data)
{ // NOTE: we have gtk_file_launcher_open_containing_folder in two places
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	int file_n;
	const int f = set_f_from_i(&file_n,n,i);
	if(f < 0)
		return;
	char *file_path = getter_string(NULL,file_n,INT_MIN,f,offsetof(struct file_list,file_path));
	if(file_path == NULL)
	{
		error_simple(0,"File path is NULL. Cannot open folder.");
		return;
	}
	GFile *file = g_file_new_for_path(file_path);
	GtkFileLauncher *launcher = gtk_file_launcher_new (file);
	gtk_file_launcher_open_containing_folder (launcher,GTK_WINDOW(t_main.main_window),NULL,NULL,NULL);
	torx_free((void*)&file_path);
}

static void ui_activity_cancel(const gpointer data)
{ // Cancel active activity
	const int n = vptoi(data);
	if(t_peer[n].edit_n == -1 && t_peer[n].edit_i == INT_MIN && t_peer[n].pm_n == -1) // make it safe to call from select_changed
		return;
//	if(t_main.write_message == NULL || t_main.button_activity == NULL)
//		return;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	gtk_text_buffer_set_text(buffer,"",0);
	if(t_peer[n].edit_n > -1 || t_peer[n].edit_i > INT_MIN)
	{ // Editing was active (of message or a GROUP_PEER peernick)
		t_peer[n].edit_n = -1;
		t_peer[n].edit_i = INT_MIN;
		if(t_peer[n].pm_n > -1)
		{ // PM was active before edit, restore it
			uint32_t len;
			char *peernick = getter_string(&len,t_peer[n].pm_n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			char cancel_message[ARBITRARY_ARRAY_SIZE];
			snprintf(cancel_message,sizeof(cancel_message),"%s %s",text_private_messaging,peernick);
			torx_free((void*)&peernick);
			gtk_button_set_label(GTK_BUTTON(t_main.button_activity),cancel_message);
			sodium_memzero(cancel_message,sizeof(cancel_message));
		}
	}
	else if(t_peer[n].pm_n > -1) // DO NOT MAKE ELSE. PM was active and editing was not
		t_peer[n].pm_n = -1;
	ui_button_determination(n);
	if(t_peer[n].edit_n == -1 && t_peer[n].edit_i == INT_MIN && t_peer[n].pm_n == -1) // DO NOT MAKE ELSE.	
	{
		gtk_widget_set_visible(t_main.button_activity,FALSE);
		if(!INVERSION_TEST)
			g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle,t_main.scrolled_window_right,NULL);
	}
}

static void ui_activity_rename(const gpointer data)
{ // Rename a GROUP_PEER via change_nick(n,nick);
	t_peer[global_n].edit_n = vptoi(data);
	gtk_button_set_label(GTK_BUTTON(t_main.button_activity),text_cancel_editing);
	g_signal_connect_swapped(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),itovp(global_n));
	gtk_widget_set_visible(t_main.button_activity,TRUE);
	if(t_main.popover_message && GTK_IS_WIDGET(t_main.popover_message))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_message));
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_group_peerlist));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	uint32_t len;
	char *peernick = getter_string(&len,t_peer[global_n].edit_n,INT_MIN,-1,offsetof(struct peer_list,peernick));
	gtk_text_buffer_set_text(buffer,peernick,(int)len - 1);
	torx_free((void*)&peernick);
	ui_button_determination(global_n);
}

static void ui_establish_pm(const int n,void *popover)
{
	if(n < 0)
	{ // If triggered, check that iitovp or itovp is used as appropriate in calling the caller function.
		error_simple(0,"Sanity check failure in ui_establish_pm. Coding error. Report this.");
		return;
	}
	if(t_peer[global_n].edit_n > -1 && t_peer[global_n].edit_i > INT_MIN)
	{
		t_peer[global_n].edit_n = -1;
		t_peer[global_n].edit_i = INT_MIN;
	}
	t_peer[global_n].pm_n = n;
	uint32_t len;
	char *peernick = getter_string(&len,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
	char cancel_message[ARBITRARY_ARRAY_SIZE];
	snprintf(cancel_message,sizeof(cancel_message),"%s %s",text_private_messaging,peernick);
	torx_free((void*)&peernick);
	gtk_button_set_label(GTK_BUTTON(t_main.button_activity),cancel_message);
	sodium_memzero(cancel_message,sizeof(cancel_message));
	g_signal_connect_swapped(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),itovp(global_n));
	gtk_widget_set_visible(t_main.button_activity,TRUE);
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_group_peerlist));
	if(popover)
		gtk_popover_popdown(GTK_POPOVER(popover));
}

static void ui_private_message(const void *data)
{ // Start PM from group peerlist. Call with itovp.
	const int n = vptoi(data);
	ui_establish_pm(n,t_main.popover_group_peerlist);
}

static void ui_activity_pm(const gpointer data)
{ // Start PM from message popover. Call with iitovp.
	const int n = vptoii_n(data);
//	const int i = vptoii_i(data);
	ui_establish_pm(n,t_main.popover_message);
}

static void ui_activity_edit(const gpointer data)
{
	t_peer[global_n].edit_n = vptoii_n(data);
	t_peer[global_n].edit_i = vptoii_i(data);
	gtk_button_set_label(GTK_BUTTON(t_main.button_activity),text_cancel_editing);
	g_signal_connect_swapped(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),itovp(global_n));
	gtk_widget_set_visible(t_main.button_activity,TRUE);
	if(t_main.popover_message && GTK_IS_WIDGET(t_main.popover_message))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_message));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	const int p_iter = getter_int(t_peer[global_n].edit_n,t_peer[global_n].edit_i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.5");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩
	if(null_terminated_len == 1)
	{
		char *message = getter_string(NULL,t_peer[global_n].edit_n,t_peer[global_n].edit_i,-1,offsetof(struct message_list,message));
		gtk_text_buffer_set_text(buffer,message,(int)strlen(message)); // strlen is to avoid null pointer and otherwise reading beyond utf8
		torx_free((void*)&message);
		ui_button_determination(global_n);
	}
	else
		error_simple(0,"Attempting to edit a message type that hasn't yet been configured for editing");
}

static void ui_message_delete(const gpointer data)
{ // Should work for public, private, etc, messages.
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	message_edit(n,i,NULL);
	if(t_main.popover_message && GTK_IS_WIDGET(t_main.popover_message))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_message));
}

static void ui_message_long_press(GtkGestureLongPress* self,gdouble x,gdouble y,const gpointer data)
{
	(void) x;
	(void) y;
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.6");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint16_t protocol = protocols[p_iter].protocol;
	const uint8_t file_checksum = protocols[p_iter].file_checksum;
	const uint8_t group_msg = protocols[p_iter].group_msg;
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	const uint32_t date_len = protocols[p_iter].date_len;
	const uint32_t signature_len = protocols[p_iter].signature_len;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box, "popover_inner");

	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	const uint8_t stat = getter_uint8(n,i,-1,offsetof(struct message_list,stat));
	char *message = getter_string(NULL,n,i,-1,offsetof(struct message_list,message));

	if(file_checksum)
	{ // files:	start/pause,reject/cancel	text_start / text_pause, text_reject (in) / text_cancel (out) // TODO rebuild int_int struct and pass f along with n,i
		int file_n = n;
		if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
		{
			const int g = set_g(n,NULL);
			file_n = getter_group_int(g,offsetof(struct group_list,n));
		}
		const int f = set_f(file_n,(const unsigned char*)message,CHECKSUM_BIN_LEN-1);
		if(f < 0)
		{
			torx_free((void*)&message);
			error_simple(0,"ui_message_long_press with f=-1. Coding error. Report this.");
			breakpoint();
			return;
		}
		const int file_status = file_status_get(file_n,f);
		torx_read(file_n) // 🟧🟧🟧
		const uint8_t file_path_exists = peer[file_n].file[f].file_path ? 1 : 0;
		torx_unlock(file_n) // 🟩🟩🟩
		if(file_path_exists)
		{
			if(file_status == ENUM_FILE_INACTIVE_ACCEPTED)
				create_button(text_start,ui_message_pause,data)
			else if(file_status == ENUM_FILE_ACTIVE_IN || file_status == ENUM_FILE_ACTIVE_OUT || file_status == ENUM_FILE_ACTIVE_IN_OUT)
				create_button(text_pause,ui_message_pause,data)
		}
		if(file_status != ENUM_FILE_INACTIVE_AWAITING_ACCEPTANCE_INBOUND && file_status != ENUM_FILE_INACTIVE_CANCELLED)
			create_button(text_open_folder,ui_open_folder,data)
		if(file_status != ENUM_FILE_INACTIVE_CANCELLED)
			create_button(stat == ENUM_MESSAGE_RECV ? text_reject : text_cancel,ui_message_reject,data)
	}
	else
	{ // messages:		copy,edit,delete	text_copy, text_edit, text_delete
		if((date_len && signature_len) || stat != ENUM_MESSAGE_RECV)
			create_button(text_resend,ui_message_resend,data)
		if(null_terminated_len == 1)
			create_button(text_copy,ui_message_copy,data)
		else if(protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED)
		{ // checking that data and data_len exist before displaying (ie, check that the sticker isn't already saved)
			const int s = ui_sticker_set((const unsigned char*)message);
			if(s > -1)
				create_button(text_save,ui_sticker_save,itovp(s))
		}
		if(null_terminated_len == 1 && (signature_len == 0 || stat != ENUM_MESSAGE_RECV))
			create_button(text_edit,ui_activity_edit,data)
		create_button(text_audio_call,call_start,data)
		if(owner == ENUM_OWNER_GROUP_PEER)
			create_button(text_private_messaging,ui_activity_pm,data)
		if(owner == ENUM_OWNER_GROUP_PEER)
			create_button(text_rename,ui_activity_rename,itovp(n))
	}
	torx_free((void*)&message);
	create_button(text_delete,ui_message_delete,data)
	t_main.popover_message = custom_popover_new(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self)));
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_message),box);
	gtk_popover_popup(GTK_POPOVER(t_main.popover_message));
}

static GtkWidget *ui_message_info(const int n,const int i)
{ // Build the message header or footer
	GtkWidget *info_message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);


	char *timebuffer = message_time_string(n,i);
	GtkWidget *message_time = gtk_label_new(timebuffer);
	gtk_widget_add_css_class(message_time, "message_info_time");
	torx_free((void*)&timebuffer);

	char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
	GtkWidget *peernick_widget = gtk_label_new(peernick);
	torx_free((void*)&peernick);
	gtk_widget_add_css_class(peernick_widget, "message_info_peernick");

	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	const uint8_t stat = getter_uint8(n,i,-1,offsetof(struct message_list,stat));
	gtk_widget_set_halign(info_message_box, stat == ENUM_MESSAGE_RECV ? GTK_ALIGN_START : GTK_ALIGN_END);
	if(owner != ENUM_OWNER_GROUP_CTRL && owner != ENUM_OWNER_CTRL)
		gtk_box_append(GTK_BOX(info_message_box),peernick_widget);
	if(owner != ENUM_OWNER_GROUP_CTRL && stat == ENUM_MESSAGE_FAIL)
		gtk_box_append(GTK_BOX(info_message_box),gtk_image_new_from_paintable(GDK_PAINTABLE(fail_ico))); //;//{} //gtk_box_append(GTK_BOX(t_peer[n].t_message[i].info_message_box),t_peer[n].t_message[i].fail_icon);
	else
		gtk_box_append(GTK_BOX(info_message_box),message_time);
	return info_message_box;
}

static inline uint8_t extension_check(const char *filename,const size_t filename_len,const char *extension)
{ // case insensitive
	size_t extension_len;
	if(!filename || !extension || (extension_len = strlen(extension)) == 0 || extension_len > filename_len)
		return 0;
	if(!strncasecmp(&filename[filename_len-extension_len],extension,extension_len))
		return 1;
	return 0;
}

static inline uint8_t is_image_file(const char *filename)
{ // does not check file validity, only suffix
	size_t filename_len;
	if(!display_images || !filename || (filename_len = strlen(filename)) == 0)
		return 0;
	const int number_of_supported = sizeof(supported_image_formats)/sizeof(char*);
	for(int iter = 0; iter < number_of_supported; iter++)
		if(extension_check(filename,filename_len,supported_image_formats[iter]))
			return 1;
	return 0;
}

static GtkWidget *ui_message_generator(const int n,const int i,const int f,int g)
{
	const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"ui_message_generator called on p_iter < 0. Coding error. Report this.");
		return gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	}
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint16_t protocol = protocols[p_iter].protocol;
	const uint8_t group_pm = protocols[p_iter].group_pm;
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	const uint8_t group_msg = protocols[p_iter].group_msg;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩
	torx_read(n) // 🟧🟧🟧
	const uint8_t owner = peer[n].owner;
	const uint8_t stat = peer[n].message[i].stat;
	torx_unlock(n) // 🟩🟩🟩
	uint32_t message_len;
	char *message = getter_string(&message_len,n,i,-1,offsetof(struct message_list,message));
	int nn = n; // XXX IMPORTANT: usage of n when relating to specific message, nn when relating to group settings or global_n. nn is GROUP_CTRL, if applicable, otherwise is n. XXX
	if(owner == ENUM_OWNER_GROUP_PEER)// && peer[n].message[i].stat == ENUM_MESSAGE_RECV)
	{
		const int group_n = getter_group_int(g,offsetof(struct group_list,n));
		if(group_n > -1)
			nn = group_n;
	}
	GtkWidget *msg = NULL; // must be null initialized
	uint8_t finished_image = 0;
	char *filename;
	char *file_path;
	if(f > -1) // File
	{
		int file_n = n;
		if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
			file_n = nn; // group_n
		filename = getter_string(NULL,file_n,INT_MIN,f,offsetof(struct file_list,filename));
		file_path = getter_string(NULL,file_n,INT_MIN,f,offsetof(struct file_list,file_path));
	//	printf("Checkpoint loader: pc=%u fic=%d fsg=%d\n",t_peer[file_n].t_file[f].previously_completed,file_is_complete(file_n,f),file_status_get(file_n,f));
		if(t_peer[file_n].t_file[f].previously_completed || file_is_complete(file_n,f))
		{
			t_peer[file_n].t_file[f].previously_completed = 1;
			finished_image = is_image_file(file_path);
		}
		if(!finished_image) // NOT else if
			msg = gtk_label_new(filename);
	}
	else
	{ // Audited 2024/02/16 // not file related
		if(null_terminated_len == 1)
			msg = gtk_label_new(message);
		else if(protocol == ENUM_PROTOCOL_GROUP_OFFER || protocol == ENUM_PROTOCOL_GROUP_OFFER_FIRST)
		{
			g = set_g(-1,message); // necessary to do this again even if we have g because it reserves. also WE MIGHT NOT ALREADY HAVE IT
			const int group_n = getter_group_int(g,offsetof(struct group_list,n));
			uint32_t peercount = be32toh(align_uint32((void*)&message[GROUP_ID_SIZE])); // XXX cannot use g_peercount because its zero
			const uint32_t g_peercount = getter_group_uint32(g,offsetof(struct group_list,peercount));
			const uint8_t g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
			if(peercount < g_peercount)
				peercount = g_peercount; // override if it has grown (when we already joined the group)
			const char *group_type;
			if(g_invite_required)
				group_type = text_group_private;
			else
				group_type = text_group_public;
			char *peernick = NULL;
			uint32_t peernick_len = 0;
			if(group_n > -1)
				peernick = getter_string(&peernick_len,group_n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			char group_message[ARBITRARY_ARRAY_SIZE];
			if(group_n > -1 && peernick && peernick_len)
				snprintf(group_message,sizeof(group_message),"%s\n%s: %u\n%s",group_type,text_current_members,peercount,peernick);
			else
			{ // We have not joined yet, so no name. Use encoded GroupID instead
				unsigned char id[GROUP_ID_SIZE];
				pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
				memcpy(id,group[g].id,sizeof(id));
				pthread_rwlock_unlock(&mutex_expand_group); // 🟩
				char *group_encoded = b64_encode(id,GROUP_ID_SIZE);
				sodium_memzero(id,sizeof(id));
				snprintf(group_message,sizeof(group_message),"%s\n%s: %u\n%s",group_type,text_current_members,peercount,group_encoded);
				torx_free((void*)&group_encoded);
			}
			torx_free((void*)&peernick);
			msg = gtk_label_new(group_message);
			sodium_memzero(group_message,sizeof(group_message));
		}
		else if(message_len >= CHECKSUM_BIN_LEN && (protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED))
		{
			const int s = ui_sticker_set((unsigned char*)message);
			if(s < 0 || !(msg = ui_sticker_box(sticker[s].paintable_animated,size_sticker_large)))
			{ // Does not exist yet
				if(stat == ENUM_MESSAGE_RECV && ENABLE_SPINNERS)
				{
					msg = gtk_spinner_new();
					gtk_spinner_start(GTK_SPINNER(msg)); // alt: gtk_spinner_set_spinning(msg,TRUE);
				}
				else
				{
					msg = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo));
					gtk_widget_set_size_request(msg,size_sticker_large,size_sticker_large);
				}
			}
		}
		else if(protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED)
		{ // Audio message TODO need message length
			uint32_t trash;
			getter_array(&trash,sizeof(trash),n,i,-1,offsetof(struct message_list,message));
			const uint32_t duration = be32toh(trash);
			char placeholder[64];
			snprintf(placeholder,sizeof(placeholder)," %u\" ",(duration+500)/1000); // NOTE: The +500 is to implement rounding
			msg = gtk_label_new(placeholder);
		}
		else
		{
			char placeholder[64];
			snprintf(placeholder,sizeof(placeholder),"Unrecognized Protocol %d",protocol); // note: or bunk length, see tmp_message_len checks
			msg = gtk_label_new(placeholder);
		}
	}
	if(msg)
		gtk_widget_set_halign(msg, GTK_ALIGN_START);
//	GtkWidget *inner_message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	GtkWidget *grid = gtk_grid_new();
	GtkWidget *outer_message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	if(stat == ENUM_MESSAGE_RECV)
	{
	//	gtk_widget_set_halign(outer_message_box, GTK_ALIGN_START);
		if(group_pm)
			gtk_widget_add_css_class(outer_message_box, "message_recv_private");
		else
			gtk_widget_add_css_class(outer_message_box, "message_recv");
	}
	else
	{
	//	gtk_widget_set_halign(outer_message_box, GTK_ALIGN_END);
		if(group_pm)
			gtk_widget_add_css_class(outer_message_box, "message_sent_private");
		else
			gtk_widget_add_css_class(outer_message_box, "message_sent");
	}

	if(msg && GTK_IS_LABEL(msg))
	{
		gtk_label_set_use_markup(GTK_LABEL(msg),FALSE); // important. perhaps we should set this after creating label but before creating text? depends how it is parsed.
		gtk_label_set_wrap(GTK_LABEL(msg), TRUE);
		gtk_label_set_natural_wrap_mode(GTK_LABEL(msg), FALSE);
		gtk_label_set_wrap_mode(GTK_LABEL(msg), PANGO_WRAP_WORD_CHAR);
	//	gtk_label_set_max_width_chars(GTK_LABEL(msg), 50); // TODO do not hard-code a max width like this? its getting ignored anyway so wtf
		gtk_label_set_selectable(GTK_LABEL(msg), FALSE); // disabled selectable messages because we are implementing a menu on longpress.
	}
	GtkGesture *long_press = NULL;
//	if(scroll != 3)
//	{
		long_press = gtk_gesture_long_press_new();
		gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(long_press),LONG_PRESS_TIME);
		g_signal_connect(long_press, "pressed", G_CALLBACK(ui_message_long_press),iitovp(n,i)); // DO NOT FREE arg because this only gets passed ONCE.
//	}
	GtkWidget *file_icon = {0};
	/* Set up text area of message */
	if(f > -1)
	{
		int file_n = n;
		if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
			file_n = nn; // group_n
		if(long_press)
			g_signal_connect(long_press, "cancelled", G_CALLBACK(ui_toggle_file),iitovp(file_n,f)); // clicked proper for buttons; this is proper for boxes  // DO NOT FREE arg because this only gets passed ONCE.
		t_peer[file_n].t_file[f].n = n;
		t_peer[file_n].t_file[f].i = i;
		if(finished_image)
		{
			t_peer[file_n].t_file[f].progress_bar = NULL; // Important to prevent segfaults in transfer_progress_idle. Do not remove.
			GBytes *bytes_file;
			if(extension_check(file_path,strlen(file_path),".gif") && (bytes_file = g_file_load_bytes(g_file_new_for_path(file_path),NULL,NULL,NULL)))
			{ // We check if bytes_file exists because otherwise there can be issues once we reach g_bytes_get_data
				size_t bytes_size = 0;
				const void *bytes = g_bytes_get_data(bytes_file,&bytes_size);
				GtkWidget *gif = ui_sticker_box(gif_animated_new_from_data(bytes,bytes_size),size_sticker_large);
				if(gif) // not bunk
					gtk_grid_attach (GTK_GRID(grid),gif,0,0,1,1);
				else
					error_simple(0,"A bunk gif occured.");
			}
			else
			{ // If file doesn't load or doesn't exist, it will show a red x
				GtkWidget *image = gtk_image_new_from_file(file_path);
				gtk_widget_set_size_request(image,size_sticker_large,size_sticker_large);
				gtk_grid_attach (GTK_GRID(grid),image,0,0,1,1);
			}
		}
		else
		{
			file_icon = ui_get_icon_from_filename(filename);
			gtk_widget_set_size_request(file_icon, size_file_icon, size_file_icon);
			char *file_size_text = file_progress_string(file_n,f);
			t_peer[file_n].t_file[f].file_size = gtk_label_new(file_size_text);
			torx_free((void*)&file_size_text);
			double fraction = 0;
			const uint64_t size = getter_uint64(file_n,INT_MIN,f,offsetof(struct file_list,size));
			const uint64_t transferred = calculate_transferred(file_n,f);
			if(!transferred && t_peer[file_n].t_file[f].previously_completed)
				fraction = 1.000;
			else if(size > 0)
				fraction = (1.000 *(double)transferred / (double)size);

			t_peer[file_n].t_file[f].progress_bar = gtk_progress_bar_new();
			gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(t_peer[file_n].t_file[f].progress_bar),TRUE);
			gtk_progress_bar_set_text(GTK_PROGRESS_BAR(t_peer[file_n].t_file[f].progress_bar),NULL);
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(t_peer[file_n].t_file[f].progress_bar),fraction);
			GtkWidget *text_area = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
			gtk_box_append(GTK_BOX(text_area),msg);
			gtk_box_append(GTK_BOX(text_area),t_peer[file_n].t_file[f].file_size);
			gtk_box_append(GTK_BOX(text_area),t_peer[file_n].t_file[f].progress_bar);
			const int file_status = file_status_get(file_n,f);
			if(file_status == ENUM_FILE_INACTIVE_AWAITING_ACCEPTANCE_INBOUND || file_status == ENUM_FILE_INACTIVE_CANCELLED)
				gtk_widget_set_visible(t_peer[file_n].t_file[f].progress_bar,FALSE);

			gtk_grid_attach (GTK_GRID(grid),file_icon,0,0,1,1);
			gtk_grid_attach (GTK_GRID(grid),text_area,1,0,1,1);
		//	gtk_box_append(GTK_BOX(inner_message_box),file_icon);
		//	gtk_box_append(GTK_BOX(inner_message_box),text_area);
		//	printf("Checkpoint ui_print_message file_n==%d f==%d\n",file_n,f);
		}
		torx_free((void*)&filename);
		torx_free((void*)&file_path);
	}
	else if(protocol == ENUM_PROTOCOL_GROUP_OFFER || protocol == ENUM_PROTOCOL_GROUP_OFFER_FIRST)
	{
		file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo));
		gtk_widget_set_size_request(file_icon, size_file_icon, size_file_icon);
		if(stat == ENUM_MESSAGE_RECV)
		{
			GtkGesture *gesture = gtk_gesture_click_new();
			g_signal_connect_swapped(gesture, "released", G_CALLBACK(ui_group_join),iitovp(n,i)); // do not free
			gtk_widget_add_controller(outer_message_box, GTK_EVENT_CONTROLLER(gesture));
		}
	//	GtkWidget *text_area = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	//	gtk_box_append(GTK_BOX(text_area),msg);
		gtk_grid_attach (GTK_GRID(grid),file_icon,0,0,1,1);
		gtk_grid_attach (GTK_GRID(grid),msg,1,0,1,1);
	//	gtk_box_append(GTK_BOX(inner_message_box),file_icon);
	//	gtk_box_append(GTK_BOX(inner_message_box),text_area);
	}
	else if(protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED)
	{
		file_icon = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo));
		gtk_widget_set_size_request(file_icon, size_file_icon/2, size_file_icon/2);

		GtkGesture *gesture = gtk_gesture_click_new(); // XXX DO NOT SET THIS TO "pressed". MUST use "released" otherwise when ui_message_build is called, which triggers splicing, segfaults will occur if the message was clicked on the edges XXX
		g_signal_connect_swapped(gesture, "released", G_CALLBACK(playback_message),iitovp(n,i)); // do not free
		gtk_widget_add_controller(outer_message_box, GTK_EVENT_CONTROLLER(gesture));

		gtk_grid_attach (GTK_GRID(grid),file_icon,0,0,1,1);
		gtk_grid_attach (GTK_GRID(grid),msg,1,0,1,1);
		if(stat == ENUM_MESSAGE_RECV && t_peer[n].t_message[i].unheard)
			gtk_grid_attach (GTK_GRID(grid),gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red)),2,0,1,1);
	}
	else
		gtk_grid_attach (GTK_GRID(grid),msg,0,0,1,1);
	//	gtk_box_append(GTK_BOX(inner_message_box),msg);

	GtkWidget *info_message_box = ui_message_info(n,i);
	gtk_box_append(GTK_BOX(outer_message_box), grid);
//	gtk_box_append(GTK_BOX(outer_message_box), inner_message_box);
	gtk_box_append(GTK_BOX(outer_message_box), info_message_box);

//	if(scroll != 3)
		gtk_widget_add_controller(outer_message_box, GTK_EVENT_CONTROLLER(long_press));
	torx_free((void*)&message);
	return outer_message_box;
}

static void message_builder(GtkListItemFactory *factory, GtkListItem *list_item, gpointer data)
{ // This is for message bubbles
	(void) factory;
	(void) data;
	gtk_list_item_set_activatable(list_item,FALSE); // XXX this changes whether hover is visible
	IntPair *pair = gtk_list_item_get_item(list_item);
	if(pair)
	{
		const int n = pair->first;
		const int i = pair->second;
		const int f = pair->third;
		const int g = pair->fourth;
		if(!t_peer[n].t_message[i].visible)
			return;
		#if GTK_FACTORY_BUG
		if(t_peer[n].t_message[i].message_box) // sanity check
		{
			if(gtk_widget_get_parent (t_peer[n].t_message[i].message_box) == NULL) // DO NOT REMOVE THIS, even though it makes this whole function potentially non-op in some circumstances
				gtk_list_item_set_child(list_item, t_peer[n].t_message[i].message_box);
		}
		else
		{
			GtkWidget *message_box;
			t_peer[n].t_message[i].message_box = message_box = ui_message_generator(n,i,f,g);
			if(INVERSION_TEST)
				gtk_widget_add_css_class(message_box,"invert-vertical");
			const uint8_t stat = getter_uint8(n,i,-1,offsetof(struct message_list,stat));
			if(stat == ENUM_MESSAGE_RECV)
				gtk_widget_set_halign(message_box, GTK_ALIGN_START);
			else
				gtk_widget_set_halign(message_box, GTK_ALIGN_END);
			gtk_list_item_set_child(list_item, message_box);
			g_object_ref(message_box);
		}
		#else
		GtkWidget *message_box = ui_message_generator(n,i,f,g);
		if(INVERSION_TEST)
			gtk_widget_add_css_class(message_box,"invert-vertical");
		const uint8_t stat = getter_uint8(n,i,-1,offsetof(struct message_list,stat));
		if(stat == ENUM_MESSAGE_RECV)
			gtk_widget_set_halign(message_box, GTK_ALIGN_START);
		else
			gtk_widget_set_halign(message_box, GTK_ALIGN_END);
		gtk_list_item_set_child(list_item, message_box);
		#endif
	}
}

static void ui_print_message(const int n,const int i,const int scroll)
{ // use _idle or _cb unless in main thread // XXX WARNING: DO NOT ACCESS .message STRUCT IF SCROLL==3
	if(n < 0 || i == INT_MIN/* || i > getter_int(n,INT_MIN,-1,offsetof(struct peer_list,max_i)) || i < getter_int(n,INT_MIN,-1,offsetof(struct peer_list,min_i))*/)
	{
		error_simple(0,"Sanity check failed in ui_print_message. Coding error. Report this.");
		breakpoint();
		return;
	}
	const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
		return;
	#if GTK_FACTORY_BUG
	if(scroll == 2 && t_peer[n].t_message[i].message_box)
	{
		g_object_unref(t_peer[n].t_message[i].message_box);
		t_peer[n].t_message[i].message_box = NULL;
	}
	#endif
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint16_t protocol = protocols[p_iter].protocol;
	const uint8_t notifiable = protocols[p_iter].notifiable;
	const uint8_t group_pm = protocols[p_iter].group_pm;
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	const uint8_t file_offer = protocols[p_iter].file_offer;
//	const uint8_t group_msg = protocols[p_iter].group_msg;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩
	uint32_t message_len;
	char *message = getter_string(&message_len,n,i,-1,offsetof(struct message_list,message));
	const uint8_t stat = getter_uint8(n,i,-1,offsetof(struct message_list,stat));
	if(scroll == 2 && stat != ENUM_MESSAGE_RECV && message_len >= CHECKSUM_BIN_LEN && (protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED))
	{ // THE FOLLOWING IS IMPORTANT TO PREVENT FINGERPRINTING BY STICKER WALLET. XXX NOTE: To allow offline stickers to work for GROUP messages, also permit scroll == 1 (prolly very bad idea thou because unlimitd peers could repeatedly request)
		const int s = ui_sticker_set((unsigned char*)message);
		if(s > -1)
		{ // Will always be true because we are sending a sticker here
			int iter = 0;
			while(iter < MAX_PEERS && sticker[s].peers[iter] != n && sticker[s].peers[iter] > -1)
				iter++;
			if(iter < MAX_PEERS && sticker[s].peers[iter] < 0) // Register a new recipient of sticker so that they can request data
				sticker[s].peers[iter] = n;
		}
	}
	if(!notifiable)
	{ // not notifiable message type or group peer is ignored. could still flash something.
		if(n == global_n && scroll == 1) // Done printing messages. Last in a list. (Such as, when select_changed() prints a bunch at once). Things that only go once go here. XXX
		{
			if(INVERSION_TEST)
				g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle_inverted,t_main.scrolled_window_right,NULL);
			else
				g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
		}
		torx_free((void*)&message);
		return; // do not display
	}
	int g = -1;
	int nn = n; // XXX IMPORTANT: usage of n when relating to specific message, nn when relating to group settings or global_n. nn is GROUP_CTRL, if applicable, otherwise is n. XXX
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_PEER)// && peer[n].message[i].stat == ENUM_MESSAGE_RECV)
	{
		if(!group_pm && stat != ENUM_MESSAGE_RECV)
		{
			torx_free((void*)&message);
			return;	// Do not print OUTBOUND messages on GROUP_PEER unless they are private
		}
		const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
		if(stat == ENUM_MESSAGE_RECV && (t_peer[n].mute || status == ENUM_STATUS_BLOCKED))
		{
			torx_free((void*)&message);
			return; // Do not print inbound messages from muted (ignored) or blocked group peers
		}
		g = set_g(n,NULL);
		const int group_n = getter_group_int(g,offsetof(struct group_list,n));
		if(group_n > -1)
			nn = group_n;
	}
	if((nn != global_n || !gtk_widget_get_visible(t_main.main_window)) && scroll == 1 && stat == ENUM_MESSAGE_RECV)
	{ /* Notify of complete Message (indicated by scroll==1), but not on screen */
		t_peer[nn].unread++;
		if (owner == ENUM_OWNER_GROUP_CTRL || owner == ENUM_OWNER_GROUP_PEER)
			totalUnreadGroup++;
		else
			totalUnreadPeer++;
		ui_decorate_panel_left_top();
		if(t_peer[nn].mute == 0 && t_peer[n].mute == 0)
		{ // Notification + beep
			char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			if(null_terminated_len == 1)
				ui_notify(peernick,message);
			else if(protocol == ENUM_PROTOCOL_FILE_OFFER || protocol == ENUM_PROTOCOL_FILE_OFFER_PRIVATE)
			{
				const int f = set_f(n,(unsigned char*)message,CHECKSUM_BIN_LEN-1);
				if(f > -1)
				{
					char *filename = getter_string(NULL,n,INT_MIN,f,offsetof(struct file_list,filename));
					ui_notify(peernick,filename);
					torx_free((void*)&filename);
				}
			}
			else if(protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP || protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP_DATE_SIGNED)
			{ // group, so use group_n
				torx_free((void*)&peernick);
				peernick = getter_string(NULL,nn,INT_MIN,-1,offsetof(struct peer_list,peernick)); // XXX
				const int f = set_f(nn,(unsigned char*)message,CHECKSUM_BIN_LEN-1);
				if(f > -1)
				{
					char *filename = getter_string(NULL,nn,INT_MIN,f,offsetof(struct file_list,filename));
					ui_notify(peernick,filename);
					torx_free((void*)&filename);
				}
			}
			else if(protocol == ENUM_PROTOCOL_FILE_REQUEST)
				ui_notify(peernick,text_accepted_file);
			else if(protocol == ENUM_PROTOCOL_GROUP_OFFER || protocol == ENUM_PROTOCOL_GROUP_OFFER_FIRST)
				ui_notify(peernick,text_group_offer);
			else if(protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED)
				ui_notify(peernick,text_audio_message);
			else if(protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED)
				ui_notify(peernick,text_sticker);
			else
				error_printf(0,"A notifiable message was received that we don't know how to print: %u",protocol);
			torx_free((void*)&peernick);
			beep();
		}
	}
	if(nn == global_n)
	{ // Print it. DO NOT make this else if (needs to occur even if window not visible)
		int f = -1;
		if(protocol == ENUM_PROTOCOL_FILE_OFFER || protocol == ENUM_PROTOCOL_FILE_OFFER_PRIVATE)
			f = set_f(n,(unsigned char*)message,CHECKSUM_BIN_LEN-1);
		else if(protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP || protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP_DATE_SIGNED)
			f = set_f(nn,(unsigned char*)message,CHECKSUM_BIN_LEN-1);
		if(protocol == ENUM_PROTOCOL_FILE_REQUEST || (file_offer && f < 0))
		{
			if(scroll == 1) // Done printing messages. Last in a list. (Such as, when select_changed() prints a bunch at once). Things that only go once go here. XXX
			{
				if(INVERSION_TEST)
					g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle_inverted,t_main.scrolled_window_right,NULL);
				else
					g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
			}
			torx_free((void*)&message);
			return; // do not display other types of files messages
		}
		// Handle :sent: TODO
		if(scroll == 2)
		{ // Update but *do not* actually scroll
		//	ui_message_info(n,i);
			IntPair *pair = int_pair_new(n,i,f,g);
		//	guint n_items = g_list_model_get_n_items(G_LIST_MODEL(t_main.list_store_chat));
			if(INVERSION_TEST)
			{
				const guint total = (guint)(t_main.global_pos_max - t_main.global_pos_min);
				g_list_store_splice (t_main.list_store_chat,total - (guint)abs(t_main.global_pos_min - t_peer[n].t_message[i].pos),1,(void**)&pair,1);
			}
			else
				g_list_store_splice (t_main.list_store_chat,(guint)abs(t_main.global_pos_min - t_peer[n].t_message[i].pos),1,(void**)&pair,1);
			goto skip_printing;
		}
		t_peer[n].t_message[i].visible = 1; // goes before g_list_store_append
		if(scroll < 0)
		{
			if(INVERSION_TEST)
			{
				g_list_store_append(t_main.list_store_chat, int_pair_new(n,i,f,g));
				t_peer[n].t_message[i].pos = t_main.global_pos_min--; // goes after g_list_store_append and only here // XXX NOTE THE DIFFERENCE, ++ after
			}
			else
			{
				g_list_store_insert(t_main.list_store_chat, 0, int_pair_new(n,i,f,g));
				t_peer[n].t_message[i].pos = --t_main.global_pos_min; // goes after g_list_store_append and only here // XXX NOTE THE DIFFERENCE, ++ before
			}
		}
		else
		{
			if(INVERSION_TEST)
			{
			/*	IntPair *pair = int_pair_new(n,i,f,g);
				g_list_store_splice(t_main.list_store_chat, 0, 0, (void**)&pair,1); */ // This is the same as g_list_store_insert
				g_list_store_insert(t_main.list_store_chat, 0, int_pair_new(n,i,f,g));
				t_peer[n].t_message[i].pos = ++t_main.global_pos_max; // goes after g_list_store_append and only here // XXX NOTE THE DIFFERENCE, ++ before
			}
			else
			{
				g_list_store_append(t_main.list_store_chat, int_pair_new(n,i,f,g));
				t_peer[n].t_message[i].pos = t_main.global_pos_max++; // goes after g_list_store_append and only here // XXX NOTE THE DIFFERENCE, ++ after
			}
		}
		if(scroll == 1) // Done printing messages. Last in a list. (Such as, when select_changed() prints a bunch at once). Things that only go once go here. XXX 2024/03/09 note: ==3 because message could change size
		{
			if(INVERSION_TEST)
				g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle_inverted,t_main.scrolled_window_right,NULL);
			else
				g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
		}
	}
	torx_free((void*)&message);
	skip_printing: {}
//printf("Checkpoint print n=%d i=%d p_iter=%d scroll=%d pos=%d msg=%s\n",n,i,p_iter,scroll,t_peer[n].t_message[i].pos,peer[n].message[i].message);
	if(scroll > 0)
	{ // == 1 or == 2
		const int max_i = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,max_i));
		if(scroll == 1 || i == max_i + 1)
		{ // populate_peers if last message or commanded (1)
			if(owner == ENUM_OWNER_GROUP_CTRL || owner == ENUM_OWNER_GROUP_PEER)
				ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL)); // Necessary for last_message and also the order
			else
			{
				const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
				ui_populate_peers(itovp(status)); // Necessary for last_message and also the order
			}
		}
	}
}

int print_message_idle(void *arg)
{ // this step is necessary
	struct printing *printing = (struct printing*) arg; // Casting passed struct
	const int n = printing->n;
	const int i = printing->i;
	const int scroll = printing->scroll;
	if(scroll == 1)
	{ // New message coming in
		const uint8_t stat = getter_uint8(n,i,-1,offsetof(struct message_list,stat));
		if(stat == ENUM_MESSAGE_RECV)
		{ // XXX Put as little here as possible before checking the protocol XXX
			const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
			if(p_iter < 0)
			{
				error_simple(0,"print_message_idle called on p_iter < 0. Coding error. Report this.");
				torx_free((void*)&printing);
				return 0;
			}
			pthread_rwlock_rdlock(&mutex_protocols); // 🟧
			const uint16_t protocol = protocols[p_iter].protocol;
			pthread_rwlock_unlock(&mutex_protocols); // 🟩
			if(protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED)
			{ // Received sticker
				uint32_t message_len;
				char *message = getter_string(&message_len,n,i,-1,offsetof(struct message_list,message));
				if(message && message_len >= CHECKSUM_BIN_LEN)
				{
					const int s = ui_sticker_set((unsigned char*)message);
					if(s < 0)
					{ // Don't have sticker, consider requesting it, if we haven't already
						int y = 0;
						while (y < MAX_STICKER_REQUESTS && memcmp(t_peer[n].stickers_requested[y], message, CHECKSUM_BIN_LEN))
							y++;
						if (y == MAX_STICKER_REQUESTS)
						{ // We haven't yet requested it.
							y = 0; // necessary
							while(y < MAX_STICKER_REQUESTS && !is_null(t_peer[n].stickers_requested[y],CHECKSUM_BIN_LEN))
								y++;
							if (y < MAX_STICKER_REQUESTS)
							{ // Found a slot
								memcpy(t_peer[n].stickers_requested[y],message,CHECKSUM_BIN_LEN);
								message_send(n,ENUM_PROTOCOL_STICKER_REQUEST,t_peer[n].stickers_requested[y],CHECKSUM_BIN_LEN);
							}
							else
								error_simple(0,"Requested MAX_STICKER_REQUESTS stickers already. Not requesting more."); // Should increase. Do not delete this error message.
						}
						else
							error_simple(0,"Requested this sticker already. Not requesting again."); // TODO delete
					}
				}
				torx_free((void*)&message);
			}
			else if(protocol == ENUM_PROTOCOL_STICKER_REQUEST && send_sticker_data)
			{ // Recieved sticker request
				uint32_t message_len;
				char *message = getter_string(&message_len,n,i,-1,offsetof(struct message_list,message));
				if(message && message_len)
				{
					const int s = ui_sticker_set((unsigned char*)message);
					if(s > -1)
					{
						int relevant_n = n; // For groups, this should be group_n
						for(int cycle = 0; cycle < 2; cycle++)
						{
							const uint8_t owner = getter_uint8(relevant_n,INT_MIN,-1,offsetof(struct peer_list,owner));
							int iter = 0;
							while(iter < MAX_PEERS && sticker[s].peers[iter] != relevant_n && sticker[s].peers[iter] > -1)
								iter++;
							if(iter < MAX_PEERS && relevant_n != sticker[s].peers[iter])
							{
							//	printf("Checkpoint TRYING s=%d owner=%u\n",s,owner); // FINGERPRINTING
								if(owner == ENUM_OWNER_GROUP_PEER)
								{ // if not on peer_n(pm), try group_n (public)
									const int g = set_g(n,NULL);
									relevant_n = getter_group_int(g,offsetof(struct group_list,n));
									continue;
								}
								else
									error_simple(0,"Peer requested a sticker they dont have access to (either they are buggy or malicious, or our MAX_PEERS is too small). Report this.");
							}
							else
							{ // Peer requested a sticker we have
								if(sticker[s].data && sticker[s].data_len) // Peer requested an unsaved sticker we have
									message_send(n,ENUM_PROTOCOL_STICKER_DATA_GIF,sticker[s].data,(uint32_t)sticker[s].data_len);
								else
								{ // Peer requested a saved sticker we have
									char *p = b64_encode(message,CHECKSUM_BIN_LEN);
									char query[256]; // somewhat arbitrary size
									snprintf(query,sizeof(query),"sticker-gif-%s",p);
									torx_free((void*)&p);
									size_t setting_value_len;
									unsigned char *setting_value = sql_retrieve(&setting_value_len,0,query);
									sodium_memzero(query,sizeof(query));
								//	printf("Checkpoint setting_value_len: %lu\n",setting_value_len);
									unsigned char *data = torx_secure_malloc(CHECKSUM_BIN_LEN + setting_value_len);
									memcpy(data,message,CHECKSUM_BIN_LEN);
									memcpy(&data[CHECKSUM_BIN_LEN],setting_value,setting_value_len);
									torx_free((void*)&setting_value);
									message_send(n,ENUM_PROTOCOL_STICKER_DATA_GIF,data,CHECKSUM_BIN_LEN + (uint32_t)setting_value_len); // TODO TODO TODO get it from sql_setting
									torx_free((void*)&data);
								}
							}
							break;
						}
					}
					else
						error_simple(0,"Peer requested sticker we do not have. Maybe we deleted it.");
				}
				torx_free((void*)&message);
			}
		}
	}
	ui_print_message(n,i,scroll);
	torx_free((void*)&printing);
	return 0;
}

void message_new_cb_ui(const int n,const int i)
{ // GUI Callback
	struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
	printing->n = n;
	printing->i = i;
	printing->scroll = 1;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,print_message_idle,printing,NULL);
}

void message_modified_cb_ui(const int n,const int i)
{ // GUI Callback
	struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
	printing->n = n;
	printing->i = i;
	printing->scroll = 2;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,print_message_idle,printing,NULL);
}

static int message_deleted_idle(void *arg)
{ // XXX WARNING: DO NOT ACCESS .message STRUCT due to shrinkage possibly having occurred
	const int n = vptoii_n(arg);
	const int i = vptoii_i(arg);
	if(n < 0 || i == INT_MIN)
	{
		error_simple(0,"Sanity check failed in message_deleted_idle. Coding error. Report this.");
		breakpoint();
		return 0;
	}
	printf("Checkpoint message_deleted_idle n=%d i=%d\n",n,i);
	#if GTK_FACTORY_BUG
	if(t_peer[n].t_message[i].message_box)
	{
		g_object_unref(t_peer[n].t_message[i].message_box);
		t_peer[n].t_message[i].message_box = NULL;
	}
	#endif
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(t_peer[n].t_message[i].visible)
	{
		t_peer[n].t_message[i].visible = 0; // We must set .visible = 0 before we g_list_store_remove_all or g_list_store_splice
		int group_n = -800; // DO NOT INITIALIZE AS -1!!!!
		if(owner == ENUM_OWNER_GROUP_PEER)
		{
			const int g = set_g(n,NULL);
			group_n = getter_group_int(g,offsetof(struct group_list,n));
		}
		if(n == global_n || group_n == global_n) // TODO cannot occur when we already wiped
		{
			IntPair *pair = int_pair_new(n,i,-1,-1);
			if(INVERSION_TEST)
			{
				const guint total = (guint)(t_main.global_pos_max - t_main.global_pos_min);
				g_list_store_splice (t_main.list_store_chat,total - (guint)abs(t_main.global_pos_min - t_peer[n].t_message[i].pos),1,(void**)&pair,1);
			}
			else
				g_list_store_splice (t_main.list_store_chat,(guint)abs(t_main.global_pos_min - t_peer[n].t_message[i].pos),1,(void**)&pair,1);
		}
	}
	t_peer[n].t_message[i].pos = 0;
	t_peer[n].t_message[i].visible = 0;
	t_peer[n].t_message[i].unheard = 1;
	const int max_i = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,max_i));
	if(i == max_i + 1)
	{ // populate_peers if last message
		if(owner == ENUM_OWNER_GROUP_CTRL || owner == ENUM_OWNER_GROUP_PEER)
			ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL)); // Necessary for last_message and also the order
		else
		{
			const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
			ui_populate_peers(itovp(status)); // Necessary for last_message and also the order
		}
	}
	return 0;
}

void message_deleted_cb_ui(const int n,const int i)
{ // GUI Callback // XXX WARNING: DO NOT ACCESS .message STRUCT due to shrinkage possibly having occurred
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,message_deleted_idle,iitovp(n,i),NULL);
}

static void cache_add(const int n,const time_t time,const time_t nstime,const char *data,const size_t data_len)
{ // Handles ENUM_PROTOCOL_AAC_AUDIO_STREAM_DATA_DATE data
	if(time < t_peer[n].t_cache_info.last_played_time || (time == t_peer[n].t_cache_info.last_played_time && nstime < t_peer[n].t_cache_info.last_played_nstime))
	{
		error_simple(0,"Received audio older than last played. Disgarding it. Carry on.");
		return;
	}
	const uint32_t current_allocation_size = torx_allocation_len(t_peer[n].t_cache_info.audio_cache);
	const size_t prior_count = current_allocation_size/sizeof(unsigned char *);
printf("Checkpoint cache_add First: %u.%u Last: %u.%u\n",(unsigned char)data[0],(unsigned char)data[1],(unsigned char)data[data_len-2],(unsigned char)data[data_len-1]);
	if(prior_count && (t_peer[n].t_cache_info.audio_time[prior_count-1] > time || (t_peer[n].t_cache_info.audio_time[prior_count-1] == time && t_peer[n].t_cache_info.audio_nstime[prior_count-1] > nstime)))
	{ // Received audio is older than something we already have in our struct, so we need to re-order it.
		unsigned char **audio_cache = torx_insecure_malloc((prior_count + 1) * sizeof(unsigned char *));
		time_t *audio_time = torx_insecure_malloc((prior_count + 1) * sizeof(time_t));
		time_t *audio_nstime = torx_insecure_malloc((prior_count + 1) * sizeof(time_t));
		uint8_t already_placed_new_data = 0; // must avoid placing more than once
		for(int old = (int)prior_count-1,new = (int)prior_count; old > -1; new--)
		{
			if(already_placed_new_data || t_peer[n].t_cache_info.audio_time[old] > time || (t_peer[n].t_cache_info.audio_time[old] == time && t_peer[n].t_cache_info.audio_nstime[old] > nstime))
			{ // Existing is newer, place it (may occur many times)
				audio_cache[new] = t_peer[n].t_cache_info.audio_cache[old];
				audio_time[new] = t_peer[n].t_cache_info.audio_time[old];
				audio_nstime[new] = t_peer[n].t_cache_info.audio_nstime[old];
				old--; // only -- when utilizing old data
			}
			else
			{ // Ours is newer, place it (must only occur once)
				audio_cache[new] = torx_secure_malloc(data_len);
				memcpy(audio_cache[new],data,data_len);
				audio_time[new] = time;
				audio_nstime[new] = nstime;
				already_placed_new_data = 1;
			}
		}
		torx_free((void*)&t_peer[n].t_cache_info.audio_cache);
		torx_free((void*)&t_peer[n].t_cache_info.audio_time);
		torx_free((void*)&t_peer[n].t_cache_info.audio_nstime);

		t_peer[n].t_cache_info.audio_cache = audio_cache;
		t_peer[n].t_cache_info.audio_time = audio_time;
		t_peer[n].t_cache_info.audio_nstime = audio_nstime;
	}
	else
	{ // Add the new data at the end because it is newest
		if(t_peer[n].t_cache_info.audio_cache)
		{ // Only checking one for efficiency
printf("Checkpoint prior_count=%zu making new size %zu\n",prior_count,(prior_count + 1) * sizeof(unsigned char *));
			t_peer[n].t_cache_info.audio_cache = torx_realloc(t_peer[n].t_cache_info.audio_cache, (prior_count + 1) * sizeof(unsigned char *));
			t_peer[n].t_cache_info.audio_time = torx_realloc(t_peer[n].t_cache_info.audio_time, (prior_count + 1) * sizeof(time_t));
			t_peer[n].t_cache_info.audio_nstime = torx_realloc(t_peer[n].t_cache_info.audio_nstime, (prior_count + 1) * sizeof(time_t));
		}
		else
		{
			t_peer[n].t_cache_info.audio_cache = torx_insecure_malloc((prior_count + 1) * sizeof(unsigned char *));
			t_peer[n].t_cache_info.audio_time = torx_insecure_malloc((prior_count + 1) * sizeof(time_t));
			t_peer[n].t_cache_info.audio_nstime = torx_insecure_malloc((prior_count + 1) * sizeof(time_t));
		}
		t_peer[n].t_cache_info.audio_cache[prior_count] = torx_secure_malloc(data_len);
		memcpy(t_peer[n].t_cache_info.audio_cache[prior_count],data,data_len);
		t_peer[n].t_cache_info.audio_time[prior_count] = time;
		t_peer[n].t_cache_info.audio_nstime[prior_count] = nstime;
	}
}

static int stream_idle(void *arg)
{
	struct stream_data *stream_data = (struct stream_data*) arg; // Casting passed struct
	const int n = stream_data->n;
	const int p_iter = stream_data->p_iter;
	char *data = stream_data->data;
	const uint32_t data_len = stream_data->data_len;
	if(data == NULL || data_len == 0 || n < 0 || p_iter < 0)
		goto end;
	pthread_rwlock_rdlock(&mutex_protocols); // 🟧
	const uint16_t protocol = protocols[p_iter].protocol;
	pthread_rwlock_unlock(&mutex_protocols); // 🟩
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
	if((owner == ENUM_OWNER_GROUP_PEER && t_peer[n].mute) || status == ENUM_STATUS_BLOCKED)
		goto end; // ignored or blocked
	if(data_len >= CHECKSUM_BIN_LEN && protocol == ENUM_PROTOCOL_STICKER_DATA_GIF)
	{
		int s = ui_sticker_set((unsigned char*)data);
		if(s > -1)
			error_simple(0,"We already have this sticker data, do not register or print again.");
		else
		{ // Fresh sticker data. Save it and print it
			unsigned char checksum[CHECKSUM_BIN_LEN];
			if(b3sum_bin(checksum,NULL,(unsigned char*)&data[CHECKSUM_BIN_LEN],0,data_len - CHECKSUM_BIN_LEN) != data_len - CHECKSUM_BIN_LEN || memcmp(checksum,data,sizeof(checksum)))
				error_simple(0,"Received bunk sticker data from peer. Checksum failed. Disgarding sticker.");
			else
			{
				s = ui_sticker_register((unsigned char*)&data[CHECKSUM_BIN_LEN],data_len - CHECKSUM_BIN_LEN);
				sticker[s].data = torx_secure_malloc(data_len - CHECKSUM_BIN_LEN);
				memcpy(sticker[s].data,(unsigned char*)&data[CHECKSUM_BIN_LEN],data_len - CHECKSUM_BIN_LEN);
				sticker[s].data_len = data_len - CHECKSUM_BIN_LEN;
				if(save_all_stickers)
					ui_sticker_save(itovp(s));
				int y = 0;
				while (y < MAX_STICKER_REQUESTS && memcmp(t_peer[n].stickers_requested[y],checksum,sizeof(checksum)))
					y++;
				if (y < MAX_STICKER_REQUESTS)
					sodium_memzero(t_peer[n].stickers_requested[y],CHECKSUM_BIN_LEN);
				torx_read(n) // 🟧🟧🟧
				for(int i = peer[n].max_i; i > peer[n].min_i - 1 ; i--)
				{ // Rebuild any messages that had this sticker
					const int p_iter_local = peer[n].message[i].p_iter;
					if(p_iter_local > -1)
					{
						pthread_rwlock_rdlock(&mutex_protocols); // 🟧
						const uint16_t protocol_local = protocols[p_iter_local].protocol;
						pthread_rwlock_unlock(&mutex_protocols); // 🟩
						if((protocol_local == ENUM_PROTOCOL_STICKER_HASH || protocol_local == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED || protocol_local == ENUM_PROTOCOL_STICKER_HASH_PRIVATE)
						&& !memcmp(peer[n].message[i].message,sticker[s].checksum,CHECKSUM_BIN_LEN))
						{ // Find the first relevant message and update it TODO this might not work in groups
							torx_unlock(n) // 🟩🟩🟩
							printf("Checkpoint should be rebuilding a sticker n=%d i=%i (might not work in groups)\n",n,i);
							ui_print_message(n,i,2);
							torx_read(n) // 🟧🟧🟧
							break;
						}
					}
				}
				torx_unlock(n) // 🟩🟩🟩
			}
			sodium_memzero(checksum,sizeof(checksum));
		}
	}
	else if(data_len >= 8 && (protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_DATA_DATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN || protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_LEAVE))
	{
		const time_t time = be32toh(align_uint32((void*)data)); // this is for the CALL, not MESSAGE
		const time_t nstime = be32toh(align_uint32((void*)&data[4])); // this is for the CALL, not MESSAGE
	//	printf("Checkpoint received host: %ld %ld\n",time,nstime);
		int call_n = n;
		int call_c = -1;
		int group_n = -1;
		for(size_t c = 0; c < sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list); c++)
			if(t_peer[call_n].t_call[c].start_time == time && t_peer[call_n].t_call[c].start_nstime == nstime)
				call_c = (int)c;
		if(call_c == -1 && owner == ENUM_OWNER_GROUP_PEER)
		{ // Try group_n instead
			const int g = set_g(n, NULL);
			group_n = getter_group_int(g, offsetof(struct group_list, n));
			call_n = group_n;
			for(size_t c = 0; c < sizeof(t_peer[call_n].t_call)/sizeof(struct t_call_list); c++)
				if(t_peer[call_n].t_call[c].start_time == time && t_peer[call_n].t_call[c].start_nstime == nstime)
					call_c = (int)c;
		}
		if(call_c == -1 && (protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN || protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE))
		{ // Received offer to join a new call
			if(protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE)
				call_n = n;
			call_c = set_c(call_n,time,nstime); // reserve
			t_peer[call_n].t_call[call_c].waiting = 1;
			call_peer_joining(call_n, call_c, n); // should call before call_update
			if((owner != ENUM_OWNER_GROUP_PEER || (group_n > -1 && t_peer[group_n].mute == 0)) && t_peer[call_n].mute == 0)
				ring_start(); // XXX start ringing
			char *peernick = getter_string(NULL,call_n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			ui_notify(text_incoming_call,peernick);
			torx_free((void*)&peernick);
		}
		else if(call_c > -1)
		{
			if(protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_DATA_DATE)
			{
				size_t iter = 0;
				for( ; iter < sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int) ; iter++)
					if(t_peer[call_n].t_call[call_c].participating[iter] == n)
						break;
				if(iter == sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int))
				{ // Sanity check. Might be fatal.
					error_simple(0,"Serious error in stream_idle caused by race condition, or more likely by a peer mistakenly sending AUDIO before joining or after leaving a call. Coding error. Report this.");
					goto end;
				}
				else if(t_peer[call_n].t_call[call_c].speaker_on && t_peer[call_n].t_call[call_c].participant_speaker[iter])
				{
					const time_t audio_time = be32toh(align_uint32((void*)&data[data_len])); // NOTE: this is intentionally reading outside data_len because that is where *message* date/time is stored by library
					const time_t audio_nstime = be32toh(align_uint32((void*)&data[data_len+sizeof(uint32_t)])); // NOTE: this is intentionally reading outside data_len because that is where *message* date/time is stored by library
					cache_add(n,audio_time,audio_nstime,&data[8],data_len-8);
					cache_play(n);
				}
				else
					printf("Checkpoint Disgarding streaming audio because speaker is off\n");
			}
			else if(protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_LEAVE)
				call_peer_leaving(call_n, call_c, n);
			else // if(protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN || protocol == ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE)
				call_peer_joining(call_n, call_c, n);
		}
		else
			error_printf(0, "Received a audio stream related message for an unknown call: %lu %lu",time,nstime); // If DATA, consider sending _LEAVE once. Otherwise it is _LEAVE, so ignore.
	}
	else
		error_printf(0,"Unknown stream data received: protocol=%u data_len=%u",protocol,data_len);
	torx_free((void*)&data);
	torx_free((void*)&arg);
	return 0;
	end: {}
	error_simple(0,"Potential issue in stream_cb."); // 2025/04/22 Occured when receiving a CALL JOIN from an ignore'd peer in a group chat
	torx_free((void*)&data);
	torx_free((void*)&arg);
	return 0;
}

void stream_cb_ui(const int n,const int p_iter,char *data,const uint32_t data_len)
{
	struct stream_data *stream_data = torx_insecure_malloc(sizeof(struct stream_data));
	stream_data->n = n;
	stream_data->p_iter = p_iter;
	stream_data->data = data;
	stream_data->data_len = data_len;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,stream_idle,stream_data,NULL);
}

static int message_extra_idle(void *arg)
{
	struct stream_data *stream_data = (struct stream_data*) arg; // Casting passed struct
	const int n = stream_data->n;
	const int i = stream_data->p_iter;
	const int p_iter = getter_int(n,i,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
		error_simple(0,"message_extra_idle called on p_iter < 0. Coding error. Report this.");
	else
	{
		pthread_rwlock_rdlock(&mutex_protocols); // 🟧
		const uint16_t protocol = protocols[p_iter].protocol;
		pthread_rwlock_unlock(&mutex_protocols); // 🟩
		unsigned char *data = (unsigned char *)stream_data->data;
		const uint32_t data_len = stream_data->data_len;
		if(protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED)
			t_peer[n].t_message[i].unheard = *((uint8_t *)data);
		else
			error_printf(0,"message_extra_cb received %u unknown bytes on protocol %u",data_len,protocol);
	}
	torx_free((void*)&stream_data->data);
	torx_free((void*)&arg);
	return 0;
}

void message_extra_cb_ui(const int n,const int i,unsigned char *data,const uint32_t data_len)
{
	struct stream_data *stream_data = torx_insecure_malloc(sizeof(struct stream_data));
	stream_data->n = n;
	stream_data->p_iter = i;
	stream_data->data = (char *)data;
	stream_data->data_len = data_len;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,message_extra_idle,stream_data,NULL);
}

static int message_more_idle(void *arg)
{
	struct int_p_p *int_p_p = (struct int_p_p*) arg; // Casting passed struct
	for(int current = 0; current < int_p_p->loaded ; current++)
		ui_print_message(int_p_p->loaded_array_n[current],int_p_p->loaded_array_i[current],-1);
	torx_free((void*)&int_p_p->loaded_array_n);
	torx_free((void*)&int_p_p->loaded_array_i);
	torx_free((void*)&int_p_p);
	return 0;
}

void message_more_cb_ui(const int loaded,int *loaded_array_n,int *loaded_array_i)
{
	struct int_p_p *int_p_p = torx_insecure_malloc(sizeof(struct int_p_p));
	int_p_p->loaded = loaded;
	int_p_p->loaded_array_n = loaded_array_n;
	int_p_p->loaded_array_i = loaded_array_i;
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,message_more_idle,int_p_p,NULL);
}

static void ui_keypress(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state,const gpointer data)
{ // Press key on message entry /* can pass NULL as arg */
	(void) controller;
	(void) keycode;
	(void) state;
	const int n = vptoi(data); // DO NOT FREE ARG
//	fprintf(stderr,"%d ",keyval); // TODO with the results of this, we can do ctrl+enter for send,etc (edit: nope, not viable. ctrl+k is same if held or not)
	if(!show_keyboard)
		error_simple(0,"Keypress occured while keyboard was hidden. Coding error. Report this to UI devs.");
	else if(!keyval || ((keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) && !(state & GDK_SHIFT_MASK))) // 0 == pressed enter
	{
		if(keyval && gtk_text_buffer_get_char_count(t_peer[n].buffer_write) == 1)
		{ // User pressed enter on an empty message box. Just clear it.
			gtk_text_buffer_set_text(t_peer[n].buffer_write,"",-1);
			return;
		}
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(t_peer[n].buffer_write, &start, &end);
		char *message = gtk_text_buffer_get_text(t_peer[n].buffer_write, &start, &end, FALSE); // false ignores hidden chars
		if(!message)
		{ // Should not occur due to earlier length check
			ui_button_determination(n);
			return; // Sanity check
		}
		size_t buf_len = strlen(message);
		if((keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) && !(state & GDK_SHIFT_MASK) && message[buf_len-1] == '\n')
		{ // Strip Trailing Newline, if applicable
			message[buf_len-1] = '\0';
			buf_len--;
		}
		else if((keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) && !(state & GDK_SHIFT_MASK))
			return; // enter press but not at end of line, do not send yet
		if(gtk_widget_get_visible(t_main.button_activity))
		{ // PM / edits require button_activity to be functional to trigger (this is really good)
			if(t_peer[n].edit_n > -1 && t_peer[n].edit_i > INT_MIN)
			{
				message_edit(t_peer[n].edit_n,t_peer[n].edit_i,message);
				ui_activity_cancel(itovp(n));
			}
			else if(t_peer[n].edit_n > -1)
			{
				change_nick(t_peer[n].edit_n,message);
				ui_activity_cancel(itovp(n));
				// TODO cycle through displayed messages and call ui_print_message on any that need to be updated
			}
			else if(t_peer[n].pm_n > -1)
			{ // send to GROUP_PEER instead of GROUP_CTRL
				message_send(t_peer[n].pm_n,ENUM_PROTOCOL_UTF8_TEXT_PRIVATE,message,(uint32_t)strlen(message));
				gtk_text_buffer_set_text(t_peer[n].buffer_write, "", 0); // this occurs in ui_activity_cancel
			}
			else
			{
				error_simple(0,"UI: Unknown current activity. Coding error. Report this.");
				breakpoint();
			}
		}
		else
		{
			int g = -1;
			uint8_t g_invite_required = 0;
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
			if(owner == ENUM_OWNER_GROUP_CTRL)
			{
				g = set_g(n,NULL);
				g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
			}
			if((force_sign == 2 && owner == ENUM_OWNER_CTRL) || (owner == ENUM_OWNER_GROUP_CTRL && g_invite_required)) // date && sign private group messages
				message_send(n,ENUM_PROTOCOL_UTF8_TEXT_DATE_SIGNED,message,(uint32_t)strlen(message));
		//	else if(force_sign == 1 && owner == ENUM_OWNER_CTRL)
		//		message_send(n,ENUM_PROTOCOL_UTF8_TEXT_SIGNED,message,(uint32_t)strlen(message));
			else // regular messages, private messages (in authenticated pipes), public messages in public groups (in authenticated pipes)
				message_send(n,ENUM_PROTOCOL_UTF8_TEXT,message,(uint32_t)strlen(message));
			gtk_text_buffer_set_text(t_peer[n].buffer_write, "", 0); // this occurs in ui_activity_cancel
		}
		sodium_memzero(message,buf_len);
		g_free(message);
		message = NULL;
	} /* else Non-enter keypresses are unhandled */
	ui_button_determination(n);
}

static void ui_editing_nick(GtkCellEditable *self,GParamSpec *pspec,gpointer data)
{
	(void) pspec;
	const int n = vptoi(data); // DO NOT FREE ARG
	if(!gtk_editable_label_get_editing(GTK_EDITABLE_LABEL(self)))
	{
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
		char *p = gtk_editable_get_chars(GTK_EDITABLE(self),0,56);
		char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
		if(p == NULL || strlen(p) == 0 || !strncmp(p," ",1))
		{
			gtk_editable_set_text(GTK_EDITABLE(self),peernick);
			torx_free((void*)&peernick);
			return;
		}
		else if(!strncmp(p,peernick,56))
		{
			torx_free((void*)&peernick);
			return;
		}
		else
		{
			torx_free((void*)&peernick);
			change_nick(n,p);
			if(owner == ENUM_OWNER_GROUP_CTRL)
				ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
			else
			{
				const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
				ui_populate_peers(itovp(status));
			}
		}
		sodium_memzero(p,strlen(p));
		g_free(p);
		p = NULL;
	}
}

static void ui_group_invite(const void *arg) //(const void *arg)
{
	const int n = vptoi(arg); // target
	const int g = global_group;
	const uint32_t g_peercount = getter_group_uint32(g,offsetof(struct group_list,peercount));
	const uint8_t g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
	if(t_main.popover_invite && GTK_IS_WIDGET(t_main.popover_invite))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_invite));
	if(g_invite_required == 1 && g_peercount == 0)
		message_send(n,ENUM_PROTOCOL_GROUP_OFFER_FIRST,itovp(global_group),GROUP_OFFER_FIRST_LEN);
	else
		message_send(n,ENUM_PROTOCOL_GROUP_OFFER,itovp(global_group),GROUP_OFFER_LEN);
}

static int ui_populate_peers(void *arg)
{ /* Search Letters Entered */ // Takes 0 or ENUM_STATUS_BLOCKED / ENUM_STATUS_FRIEND as argument
// TODO should probably save the scroll point, or not scroll if not at bottom, since this will be triggered on every message in/out
	if(threadsafe_read_uint8(&mutex_global_variable,&lockout))
		return 0;
	else if(!t_main.entry_search)
		return 0;
	const uint8_t list = (uint8_t)vptoi(arg);
	const char *entry_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (t_main.entry_search)));
	uint8_t status;
	if(list == 0) // XXX = means do both blocked and friend lists
		status = ENUM_STATUS_BLOCKED;
	else
		status = list;
	while(1)
	{
		int len = 0;
		int *array;
		if(status == ENUM_STATUS_GROUP_CTRL)
			array = refined_list(&len,ENUM_OWNER_GROUP_CTRL,ENUM_STATUS_FRIEND,entry_text);
		else
			array = refined_list(&len,ENUM_OWNER_CTRL,status,entry_text);
		GtkWidget *scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
		gtk_widget_set_vexpand(scroll_box, TRUE);
		if(len)
		{
			GListStore *list_store = g_list_store_new(int_pair_get_type());
			GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (list_store));
			GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
			g_signal_connect(factory, "bind", G_CALLBACK(chat_builder),(void*)(uint64_t)&ui_select_changed);
			GtkWidget *list_view = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory); // TODO NOT A direct child of a scrolled window, could have issues with > 205 widgets
			gtk_widget_add_css_class(list_view, "invisible"); // XXX important
			gtk_box_append (GTK_BOX(scroll_box), list_view);
			for(int pos = 0 ; pos < len ; pos++) // or len if starting from other direction, then count down instead of up
			{
				const int n = array[pos];
				const uint8_t status_local = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
				if(status == ENUM_STATUS_GROUP_CTRL)
					g_list_store_append(list_store, int_pair_new(array[pos],-1,-1,0));
				else if(status == ENUM_STATUS_BLOCKED && status_local == ENUM_STATUS_BLOCKED)
					g_list_store_append(list_store, int_pair_new(array[pos],-1,-1,0));
				else if(status == ENUM_STATUS_FRIEND && status_local == ENUM_STATUS_FRIEND)
					g_list_store_append(list_store, int_pair_new(array[pos],-1,-1,0));
			}
			torx_free((void*)&array);
		}
		if(t_main.scrolled_window_peer && GTK_IS_SCROLLED_WINDOW(t_main.scrolled_window_peer))
		{
			if(status == ENUM_STATUS_FRIEND)
				gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_peer),scroll_box);
			else if(status == ENUM_STATUS_BLOCKED)
				gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_block),scroll_box);
			else if(status == ENUM_STATUS_GROUP_CTRL)
				gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_group),scroll_box);
		}
		if(list == 0 && status == ENUM_STATUS_BLOCKED)
			status = ENUM_STATUS_FRIEND; // repeat once if 0 is passed
		else
			break;
	}
	return 0;
}

static void ui_populate_group_peerlist_popover(GtkWidget *entry_search_popover,GParamSpec *pspec,GtkWidget *box_popover)
{
	(void) pspec;
	gtk_box_remove_all(box_popover);
	const char *entry_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (entry_search_popover)));
	int len = 0;
	int *array = refined_list(&len,ENUM_OWNER_GROUP_PEER,global_group,entry_text);
	if(len)
	{
		GListStore *list_store = g_list_store_new(int_pair_get_type());
		GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (list_store));
		for(int pos = 0 ; pos < len ; pos++) // or len if starting from other direction, then count down instead of up
			g_list_store_append(list_store, int_pair_new(array[pos],-1,-1,3));
		torx_free((void*)&array);
		GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
		g_signal_connect(factory, "bind", G_CALLBACK(chat_builder),(void*)(uint64_t)&ui_private_message);
		GtkWidget *list_view = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory); // TODO NOT A direct child of a scrolled window, could have issues with > 205 widgets
		gtk_box_append (GTK_BOX(box_popover), list_view);
	}
}

static void ui_populate_peer_popover(GtkWidget *entry_search_popover,GParamSpec *pspec,GtkWidget *box_popover)
{ // For inviting peers to groups
	(void) pspec;
	gtk_box_remove_all(box_popover);
	const char *entry_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (entry_search_popover)));
	int len = 0;
	int *array = refined_list(&len,ENUM_OWNER_CTRL,ENUM_STATUS_FRIEND,entry_text);
	if(len)
	{
		GListStore *list_store = g_list_store_new(int_pair_get_type());
		GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (list_store));
		for(int pos = 0 ; pos < len ; pos++) // or len if starting from other direction, then count down instead of up
			g_list_store_append(list_store, int_pair_new(array[pos],-1,-1,1));
		torx_free((void*)&array);
		GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
		g_signal_connect(factory, "bind", G_CALLBACK(chat_builder),(void*)(uint64_t)&ui_group_invite);
		GtkWidget *list_view = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory); // TODO NOT A direct child of a scrolled window, could have issues with > 205 widgets
		gtk_box_append (GTK_BOX(box_popover), list_view);
	}
}

static void ui_choose_invite(GtkWidget *arg,const gpointer data)
{
	const int g = vptoi(data); // DO NOT FREE ARG
	const int group_n = getter_group_int(g,offsetof(struct group_list,n));
	const uint8_t g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
	GtkWidget *button_invite = arg;
	if(g_invite_required)
	{ // Private Group, show contact list for sending invite
		t_main.popover_invite = custom_popover_new(button_invite);

		GtkWidget *box_popover_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
		gtk_widget_add_css_class(box_popover_outer, "popover_inner");
		GtkWidget *scrolled_window_popover = gtk_scrolled_window_new();
		gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW (scrolled_window_popover),size_peerlist_minimum_width);

		gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_popover),box_popover_outer);
		gtk_popover_set_child(GTK_POPOVER(t_main.popover_invite),scrolled_window_popover);
		gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (scrolled_window_popover),size_peerlist_button_height*8);
		GtkWidget *box_popover_inner = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);

		// Search bar
		GtkWidget *entry_search_popover = gtk_entry_new();
		gtk_widget_set_valign(entry_search_popover, GTK_ALIGN_CENTER);
		gtk_widget_add_css_class(entry_search_popover, "search_field");
		gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(entry_search_popover))), 56);
		gtk_entry_set_placeholder_text(GTK_ENTRY(entry_search_popover),text_placeholder_search);
		g_signal_connect(entry_search_popover, "notify::text", G_CALLBACK (ui_populate_peer_popover),box_popover_inner); // TODO Callback for typed characters  // DO NOT FREE arg because this only gets passed ONCE.
		gtk_entry_set_icon_from_gicon(GTK_ENTRY(entry_search_popover),GTK_ENTRY_ICON_SECONDARY, global_theme == DARK_THEME ? search_dark : search_light);
		gtk_box_append(GTK_BOX(box_popover_outer), entry_search_popover);

		gtk_box_append(GTK_BOX(box_popover_outer), box_popover_inner);
		ui_populate_peer_popover(entry_search_popover,NULL,box_popover_inner);

		gtk_popover_popup(GTK_POPOVER(t_main.popover_invite));
	}
	else
	{ // Public Group, show QR and Group ID TODO consider having also popover_invite as an option
		unsigned char id[GROUP_ID_SIZE];
		pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
		memcpy(id,group[g].id,sizeof(id));
		pthread_rwlock_unlock(&mutex_expand_group); // 🟩
		char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
		sodium_memzero(id,sizeof(id));
		GtkWidget *generated_qr_code;
		struct qr_data *qr_data;
		if(QR_IS_PNG)
		{
			generated_qr_code = gtk_image_new();
			qr_data = qr_bool(group_id_encoded,8);
			size_t png_size = 0;
			void* png_data = return_png(&png_size,qr_data);
			GBytes *bytes = g_bytes_new(png_data,png_size);
			GdkTexture *texture = gdk_texture_new_from_bytes(bytes,NULL);
			gtk_image_set_from_paintable(GTK_IMAGE(generated_qr_code),GDK_PAINTABLE(texture));
			torx_free((void*)&png_data);
		}
		else
		{
			qr_data = qr_bool(group_id_encoded,1);
			char *qr = qr_utf8(qr_data);
			if(qr == NULL)
				return;
			generated_qr_code = gtk_label_new(qr);
			torx_free((void*)&qr);
		}
		gtk_widget_set_size_request(generated_qr_code,size_box_qr_code,size_box_qr_code);

		t_main.popover_qr = custom_popover_new(button_invite);

		GtkWidget *button_copy_qr = gtk_button_new_with_label (text_copy_qr);
		g_signal_connect_swapped(button_copy_qr, "clicked", G_CALLBACK(ui_copy_qr),itovp(group_n)); // DO NOT FREE arg because this only gets passed ONCE.
		GtkWidget *button_save_qr = gtk_button_new_with_label (text_save_qr);
		g_signal_connect_swapped(button_save_qr, "clicked", G_CALLBACK(ui_save_qr),itovp(group_n)); // DO NOT FREE arg because this only gets passed ONCE.
		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_ten);
		gtk_widget_add_css_class(box, "popover_inner");
		GtkWidget *label_group_id = gtk_label_new(group_id_encoded);
		gtk_label_set_selectable(GTK_LABEL(label_group_id), TRUE);
		gtk_widget_add_css_class(label_group_id, "text");
		GtkWidget *button_copy = gtk_button_new_with_label (text_copy);
		g_signal_connect(button_copy, "clicked", G_CALLBACK(ui_copy),itovp(group_n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_box_append(GTK_BOX(box),generated_qr_code);
		gtk_box_append(GTK_BOX(box),label_group_id);
		gtk_box_append(GTK_BOX(box),button_copy);
		gtk_box_append(GTK_BOX(box),button_copy_qr);
		gtk_box_append(GTK_BOX(box),button_save_qr);
		gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
		gtk_popover_set_child(GTK_POPOVER(t_main.popover_qr),box);
		gtk_popover_popup(GTK_POPOVER(t_main.popover_qr));
	//	sodium_memzero(qr,qr_data->size_allocated);	
		torx_free((void*)&qr_data->data);
		torx_free((void*)&qr_data);
		torx_free((void*)&group_id_encoded);
	}
}

static void ui_choose_vertical(GtkWidget *button,const gpointer data)
{ // TODO SIGTRAP on second click because we are using global children
	const int n = vptoi(data); // DO NOT FREE ARG
	t_main.popover_more = custom_popover_new(button);
	GtkWidget *box_popover = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box_popover, "popover_inner");
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_more),box_popover);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	gtk_box_append(GTK_BOX(box_popover), ui_button_generate(ENUM_BUTTON_CALL,n));
	if(owner != ENUM_OWNER_GROUP_CTRL)
	{
		gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_ADD_BLOCK,n));
		gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_KILL,n));
	}
	gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_DELETE_CTRL,n));
	gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_DELETE_LOG,n));
	gtk_popover_popup(GTK_POPOVER(t_main.popover_more));
}

static void ui_toggle_keyboard(GtkWidget *button,const gpointer data)
{ // XXX normally we make popovers global but for emoji chooser it seems unnecessary because all children are local
	const int n = vptoi(data); // DO NOT FREE ARG
	if(show_keyboard)
	{
		show_keyboard = 0;
		gtk_widget_set_visible(t_main.button_send,FALSE);
		gtk_widget_set_visible(t_main.button_record,TRUE);
		gtk_widget_set_visible(t_main.write_message,FALSE);
		gtk_widget_set_visible(t_main.button_emoji,FALSE);
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(keyboard_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(keyboard_light)));
	}
	else
	{
		show_keyboard = 1;
		ui_button_determination(n);
		gtk_widget_set_visible(t_main.button_record,FALSE);
		gtk_widget_set_visible(t_main.write_message,TRUE);
		gtk_widget_set_visible(t_main.button_emoji,TRUE);
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_light)));
	}
}

GtkWidget *ui_button_generate(const int type,const int n)
{
	GtkWidget *button = gtk_button_new();
	gtk_widget_add_css_class(button, "invisible");
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	if(type == ENUM_BUTTON_LOGGING)
	{
		ui_set_image_logging(button,n);
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_toggle_logging),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_MUTE)
	{
		ui_set_image_mute(button,n);
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_toggle_mute_button),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_CALL)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(call_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(call_light)));
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_call_start_button),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_KILL)
	{
		gtk_widget_set_tooltip_text(button,text_tooltip_button_kill);
	//	if(1) // TODO replace 1 with some sort of if(kill_active). Make kill toggleable.
	//	{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(kill_inactive)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(kill_inactive_light)));
	//	}
	//	else
	//		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(kill_active)));
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_toggle_kill),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_DELETE_CTRL)
	{
		gtk_widget_set_tooltip_text(button,text_tooltip_button_delete);
	//	if(1) // TODO replace 1
	//	{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive_light)));
	//	}
	//	else
	//		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_active)));
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_toggle_delete),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_DELETE_LOG)
	{
		gtk_widget_set_tooltip_text(button,text_tooltip_button_delete_log);
	//	if(1) // TODO replace 1
	//	{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(clear_all_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(clear_all_light)));
	//	}
	//	else
	//		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_active)));
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_delete_log),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_ADD_BLOCK)
	{ // add for group, block for peer
		if(owner != ENUM_OWNER_GROUP_CTRL)
			ui_set_image_block(button,n);
		else if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(add_inactive)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(add_inactive_light)));
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		if(owner == ENUM_OWNER_GROUP_CTRL)
		{ // Popover peerlist for sending Group Invitation
			global_group = set_g(n,NULL); // just looking up existing
			g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_invite),itovp(global_group)); // TODO no need to use swapped. already passing button
		}
		else
			g_signal_connect(button, "clicked", G_CALLBACK(ui_toggle_block_button),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_ADD_BLOCK_MENU)
	{
	/*	if(owner != ENUM_OWNER_GROUP_CTRL)
			ui_set_image_block(button,n);
		else
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive_light)));
		//	gtk_widget_set_tooltip_text(button,text_tooltip_blocked_on); // TODO set some tooltip text for this ("Menu"?)
		} */
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(more_vert_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(more_vert_light)));
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_vertical),itovp(n));
	}
	else if(type == ENUM_BUTTON_KEYBOARD_MICROPHONE)
	{
		if(show_keyboard)
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_light)));
		}
		else
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(keyboard_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(keyboard_light)));
		}
		gtk_widget_set_size_request(button, size_icon_bottom_right, size_icon_bottom_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_toggle_keyboard),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_EMOJI)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(emoji_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(emoji_light)));
		gtk_widget_set_size_request(button, size_icon_bottom_right, size_icon_bottom_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_emoji),t_main.write_message); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_STICKER)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(gif_box_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(gif_box_light)));
		gtk_widget_set_size_request(button, size_icon_bottom_right, size_icon_bottom_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_sticker_chooser),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_ATTACH)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(attach_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(attach_light)));
		gtk_widget_set_size_request(button, size_icon_bottom_right, size_icon_bottom_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_file),itovp(1)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else if(type == ENUM_BUTTON_ADD_STICKER)
	{
		if(global_theme == DARK_THEME)
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(add_active)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(add_inactive_light)));
		gtk_widget_set_name(button,"button_add_sticker");
		gtk_widget_set_size_request(button, size_icon_bottom_right, size_icon_bottom_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_file),itovp(2)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	else
		error_simple(0,"ui_button_generate called for invalid button type. Coding error. Report to UI devs.");
	return button;
}

static void ui_group_peerlist(const gpointer data)
{
	(void) data;
//	const int g = set_g(vptoi(data),NULL); // DO NOT FREE ARG
	t_main.popover_group_peerlist = custom_popover_new(t_main.last_online);

	GtkWidget *box_popover_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box_popover_outer, "popover_inner");
	GtkWidget *scrolled_window_popover = gtk_scrolled_window_new();
	gtk_scrolled_window_set_min_content_width (GTK_SCROLLED_WINDOW (scrolled_window_popover),size_peerlist_minimum_width);

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_window_popover),box_popover_outer);
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_group_peerlist),scrolled_window_popover);
	gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (scrolled_window_popover),size_peerlist_button_height*8);
	GtkWidget *box_popover_inner = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);

	// Search bar
	GtkWidget *entry_search_popover = gtk_entry_new();
	gtk_widget_set_valign(entry_search_popover, GTK_ALIGN_CENTER);
	gtk_widget_add_css_class(entry_search_popover, "search_field");
	gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(entry_search_popover))), 56);
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry_search_popover),text_placeholder_search);
	g_signal_connect(entry_search_popover, "notify::text", G_CALLBACK (ui_populate_group_peerlist_popover),box_popover_inner); // TODO Callback for typed characters  // DO NOT FREE arg because this only gets passed ONCE.
	gtk_entry_set_icon_from_gicon(GTK_ENTRY(entry_search_popover),GTK_ENTRY_ICON_SECONDARY, global_theme == DARK_THEME ? search_dark : search_light);
	gtk_box_append(GTK_BOX(box_popover_outer), entry_search_popover);

	gtk_box_append(GTK_BOX(box_popover_outer), box_popover_inner);
	ui_populate_group_peerlist_popover(entry_search_popover,NULL,box_popover_inner);

	gtk_popover_popup(GTK_POPOVER(t_main.popover_group_peerlist));
}

static inline void on_file_drop(GtkDropTarget *drop_target, const GValue *value, gpointer arg)
{ // https://gitlab.gnome.org/GNOME/gtk/-/issues/3755
	(void) drop_target;
	const int n = vptoi(arg);
	if (n > -1 && G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
	{
		GSList *list = g_value_get_boxed (value);
		for (const GSList *iter = list; iter && G_IS_FILE (iter->data); iter = iter->next)
		{ // NOTE: We could also handle non-GFile things to allow dragging and dropping of text, for example.
			char *file_path = g_file_get_path(iter->data); // free'd
			file_send(n,file_path);
			g_free(file_path); file_path = NULL;
		}
	}
}

static void ui_send_pressed(GtkGestureClick *gesture, int n_press, double x, double y, gpointer *arg)
{ // This is NOT exclusive to record button, but also send button.
	(void)gesture;
	(void)n_press;
	(void)x;
	(void)y;
	(void)arg;
	if(!show_keyboard)
	{
		if(current_recording.pipeline)
		{ // TODO should also call_leave_all_except(-1,-1);
			error_simple(0,"An existing recording was detected and disposed of. Coding error. Report this to UI Devs.");
			unsigned char *to_free = record_stop(NULL,NULL,&current_recording);
			torx_free((void*)&to_free);
			call_mute_all_except(-1,-1); // mute active calls, both mic and speaker
		}
		record_start(&current_recording,16000,NULL,NULL); // 8000, 12000, 16000 works
	}
}

static void ui_send_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *button)
{
	(void)gesture;
	(void)n_press;
	if(show_keyboard)
		ui_keypress(NULL, 0, 0, 0, itovp(global_n));
	else if(button && global_n > -1 && current_recording.pipeline)
	{
		size_t data_len;
		uint32_t duration; // milliseconds
		unsigned char *data = record_stop(&data_len,&duration,&current_recording);
		if(gtk_widget_contains(button,x,y) && data && data_len && duration > MINIMUM_AUDIO_MESSAGE_LENGTH_IN_MILLISECONDS)
		{ // TODO check minimum length also (1 second?)
			data = torx_realloc_shift(data,sizeof(uint32_t) + data_len,1);
			const uint32_t trash = htobe32(duration);
			memcpy(data,&trash,sizeof(trash));
			const uint8_t owner = getter_uint8(global_n,INT_MIN,-1,offsetof(struct peer_list,owner));
			if (t_peer[global_n].edit_n > -1 && t_peer[global_n].edit_i > INT_MIN)
				error_simple(0,"Currently no support for modifying an audio message to another audio message. Replace it with text instead, or modify message_edit() to facilitate.");
			else if (t_peer[global_n].edit_n > -1)
				error_simple(0, "Cannot modify peernick to an audio message.");
			else if (t_peer[global_n].pm_n > -1)
				message_send(t_peer[global_n].pm_n, ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE, data, (uint32_t)(sizeof(uint32_t) + data_len));
			else if (owner == ENUM_OWNER_GROUP_CTRL)
			{
				const int g = set_g(global_n, NULL);
				const uint8_t g_invite_required = getter_group_uint8(g, offsetof(struct group_list, invite_required));
				if (owner == ENUM_OWNER_GROUP_CTRL && g_invite_required != 0) // date && sign private group messages
					message_send(global_n, ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED, data, (uint32_t)(sizeof(uint32_t) + data_len));
				else
					message_send(global_n, ENUM_PROTOCOL_AAC_AUDIO_MSG, data, (uint32_t)(sizeof(uint32_t) + data_len));
			}
			else
				message_send(global_n, ENUM_PROTOCOL_AAC_AUDIO_MSG, data, (uint32_t)(sizeof(uint32_t) + data_len));
		}
		else
			torx_free((void*)&data);
	}
}

static void ui_select_changed(const void *arg)
{ /* show_peer() Triggered when a peer is selected from the peer list */
	const int n = vptoi(arg); // DO NOT FREE ARG
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	int g = -1;
//	if(t_peer[n].unread)
//	{ // TODO 2024/03/22 this conditional SHOULD exist, but it causes problems in horizonal mode because "clicked" callbacks are only available once.
		if(t_peer[n].unread > 0)
		{
			if (owner == ENUM_OWNER_GROUP_CTRL)
				totalUnreadGroup -= t_peer[n].unread;
			else
				totalUnreadPeer -= t_peer[n].unread;
	      		t_peer[n].unread = 0; // reset unread count for specific peer
		//	ui_decorate_panel_left_top(); // no need because ui_decorate_panel_left is later called
		}
		if(owner == ENUM_OWNER_GROUP_CTRL)
			ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
		else
		{
			const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
			ui_populate_peers(itovp(status));
		}
//	}
//	t_peer[global_n].pm_n = -1; // this is necessary, do not remove this
	if(vertical_mode)
		vertical_mode = 2;
	t_main.window = window_peer;
	global_n = n; // XXX THIS WILL GET RESET TO -1 if t_main.window = window_peer is not set before ui_decorate_panel_left() XXX
	ui_decorate_panel_left(n);

	/* Build Info Area of Header Bar */
	GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_five);
	gtk_widget_set_halign(info_box, GTK_ALIGN_START);
	gtk_widget_set_valign(info_box, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(info_box,size_margin_ten);
	char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
	GtkWidget *group_or_user_name = gtk_editable_label_new(peernick);
	torx_free((void*)&peernick);
	gtk_widget_set_tooltip_text(group_or_user_name,text_tooltip_group_or_user_name);
	t_main.last_online = gtk_label_new(NULL);
	gtk_widget_set_halign(group_or_user_name, GTK_ALIGN_START);
	gtk_widget_set_halign(t_main.last_online, GTK_ALIGN_START);
	ui_set_last_seen(n);

	GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);

	gtk_widget_add_css_class(group_or_user_name, "group_or_user_name");
	gtk_widget_add_css_class(t_main.last_online, "last_online");
	g_signal_connect(group_or_user_name, "notify::editing", G_CALLBACK(ui_editing_nick),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.

	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		GtkGesture *click = gtk_gesture_click_new();
		g_signal_connect_swapped(click, "pressed", G_CALLBACK(ui_group_peerlist), itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(t_main.last_online, GTK_EVENT_CONTROLLER(click));
	}

	gtk_box_append(GTK_BOX(name_box), group_or_user_name);
	gtk_box_append(GTK_BOX(name_box), t_main.last_online);
	gtk_box_append(GTK_BOX(info_box), name_box);
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_left), info_box);

	gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_LOGGING,n));
	gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_MUTE,n));

	/* Append buttons as appropriate */
	if(vertical_mode)
	{
		if(owner == ENUM_OWNER_GROUP_CTRL)
			gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_ADD_BLOCK,n));
		gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_ADD_BLOCK_MENU,n));
	}
	else
	{
		gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_CALL,n));
		gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_ADD_BLOCK,n));
		gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_KILL,n));
		gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_DELETE_CTRL,n));
		gtk_box_append(GTK_BOX(t_main.chat_headerbar_right), ui_button_generate(ENUM_BUTTON_DELETE_LOG,n));
	}

	/* Scroll_box_right (where messages get displayed) */
	t_main.scrolled_window_right = gtk_scrolled_window_new();
//	t_main.scroll_box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_message_spacing); // XXX penguin XXX
//	if(MESSAGES_START_AT_TOP_OF_WINDOW == 0)
//		gtk_widget_set_valign(t_main.scroll_box_right, GTK_ALIGN_END);
//	gtk_widget_set_vexpand(t_main.scroll_box_right, TRUE);	// works, do not remove

	g_signal_connect(t_main.scrolled_window_right, "edge-reached", G_CALLBACK(ui_load_more_messages),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
//	if(t_main.list_store_chat && G_IS_LIST_STORE(t_main.list_store_chat))
//		g_list_store_remove_all(t_main.list_store_chat);
//	else
		t_main.list_store_chat = g_list_store_new(int_pair_get_type());
	t_main.global_pos_min = t_main.global_pos_max = 0;

	/* Record Button */
	t_main.button_record = gtk_label_new(text_hold_to_talk);
	gtk_widget_set_size_request(t_main.button_record, size_icon_send_width, size_icon_bottom_right);
	gtk_widget_set_hexpand(t_main.button_record, TRUE);
	gtk_widget_set_visible(t_main.button_record,FALSE);
	gtk_widget_add_css_class(t_main.button_record,"auth_button");
	gtk_widget_add_css_class(t_main.button_record,"button_record");
	GtkGesture *press_record = gtk_gesture_click_new();
	g_signal_connect(press_record, "pressed", G_CALLBACK(ui_send_pressed),NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(press_record, "released", G_CALLBACK(ui_send_released),t_main.button_record); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.button_record, GTK_EVENT_CONTROLLER(press_record));

	/* Send Button */
	if(global_theme == DARK_THEME)
		t_main.button_send = gtk_image_new_from_paintable(GDK_PAINTABLE(send_dark));
	else
		t_main.button_send = gtk_image_new_from_paintable(GDK_PAINTABLE(send_light));
	GtkGesture *press_send = gtk_gesture_click_new();
	g_signal_connect(press_send, "pressed", G_CALLBACK(ui_send_pressed),NULL); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect(press_send, "released", G_CALLBACK(ui_send_released),t_main.button_send); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.button_send, GTK_EVENT_CONTROLLER(press_send));

	/* Write Message Box (outer, includes attach + send buttons) */
	GtkWidget *write_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_zero);
	gtk_widget_set_valign(write_box, GTK_ALIGN_END);

	/* Write Message Box (inner) */
	t_main.write_message = gtk_text_view_new();
	if(t_peer[n].buffer_write == NULL)
		t_peer[n].buffer_write = gtk_text_buffer_new(NULL);

	gtk_text_view_set_buffer(GTK_TEXT_VIEW(t_main.write_message),t_peer[n].buffer_write);
	gtk_widget_add_css_class(t_main.write_message, "write_message");
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(t_main.write_message), GTK_WRAP_WORD_CHAR);

	/* Add drop target (for drag-and-drop file sends) */
	GtkDropTarget *drop_target = gtk_drop_target_new(GDK_TYPE_FILE_LIST, GDK_ACTION_COPY);
	g_signal_connect(drop_target, "drop", G_CALLBACK(on_file_drop), itovp(n));
	gtk_widget_add_controller(t_main.write_message, GTK_EVENT_CONTROLLER(drop_target));

	/* Write Message Box (inner-outer2) */
	GtkWidget *write_message_scroll = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(write_message_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (write_message_scroll), t_main.write_message);
	gtk_scrolled_window_set_propagate_natural_height (GTK_SCROLLED_WINDOW(write_message_scroll),TRUE); // grow taller
	gtk_widget_set_hexpand(t_main.write_message, TRUE);

	/* Keypress callback */
	GtkEventController *return_controller = gtk_event_controller_key_new();
	g_signal_connect_after(return_controller, "key-released", G_CALLBACK(ui_keypress),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.write_message,  GTK_EVENT_CONTROLLER(return_controller));
	gtk_box_append(GTK_BOX(write_box), write_message_scroll);

	t_main.button_emoji = ui_button_generate(ENUM_BUTTON_EMOJI,n);
	t_main.button_keyboard_microphone = ui_button_generate(ENUM_BUTTON_KEYBOARD_MICROPHONE,n);
	t_main.button_sticker = ui_button_generate(ENUM_BUTTON_STICKER,n);
	t_main.button_attach = ui_button_generate(ENUM_BUTTON_ATTACH,n);

	if(gtk_text_buffer_get_char_count(t_peer[n].buffer_write))
	{
		gtk_widget_set_visible(t_main.button_keyboard_microphone,FALSE);
		gtk_widget_set_visible(t_main.button_sticker,FALSE);
		gtk_widget_set_visible(t_main.button_attach,FALSE);
	}

	ui_button_determination(n);

	gtk_box_append(GTK_BOX(write_box), t_main.button_record);
	gtk_box_append(GTK_BOX(write_box), t_main.button_emoji);
	gtk_box_append(GTK_BOX(write_box), t_main.button_keyboard_microphone);
	gtk_box_append(GTK_BOX(write_box), t_main.button_sticker);
	gtk_box_append(GTK_BOX(write_box), t_main.button_attach);
	gtk_box_append(GTK_BOX(write_box), t_main.button_send);

	/* Set margins on write box */
	gtk_widget_set_margin_top(write_box, size_spacing_zero); // reduced to 0 due to t_main.call_box
	gtk_widget_set_margin_start(write_box, size_margin_ten);
	gtk_widget_set_margin_end(write_box, size_margin_ten);
	gtk_widget_set_margin_bottom(write_box, size_margin_fifteen);

	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);

	t_main.call_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_set_halign(t_main.call_box, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.call_box);
	for(size_t c = 0; c < sizeof(t_peer[n].t_call)/sizeof(struct t_call_list) ; c++)
		if(t_peer[n].t_call[c].column)
			gtk_box_append(GTK_BOX(t_main.call_box), t_peer[n].t_call[c].column);
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{ // Iterate through all group peers and add their rows too
		g = set_g(n,NULL);
		const uint32_t peercount = getter_group_uint32(g,offsetof(struct group_list,peercount));
		for(uint32_t nn = 0 ; nn < peercount ; nn++)
		{
			pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
			const int peer_n = group[g].peerlist[nn];
			pthread_rwlock_unlock(&mutex_expand_group); // 🟩
			for(size_t c = 0; c < sizeof(t_peer[peer_n].t_call)/sizeof(struct t_call_list) ; c++)
				if(t_peer[peer_n].t_call[c].column)
					gtk_box_append(GTK_BOX(t_main.call_box), t_peer[peer_n].t_call[c].column);
		}
	}

	// Current activity (Edit, private message)
	t_main.button_activity = gtk_button_new();
	if(t_peer[n].pm_n > -1 || t_peer[n].edit_n > -1)
	{
		if(t_peer[n].pm_n > -1)
		{
			uint32_t len;
			char *peernick_local = getter_string(&len,t_peer[n].pm_n,INT_MIN,-1,offsetof(struct peer_list,peernick));
			char cancel_message[ARBITRARY_ARRAY_SIZE];
			snprintf(cancel_message,sizeof(cancel_message),"%s %s",text_private_messaging,peernick_local);
			torx_free((void*)&peernick_local);
			gtk_button_set_label(GTK_BUTTON(t_main.button_activity),cancel_message);
			sodium_memzero(cancel_message,sizeof(cancel_message));
		}
		else if(t_peer[n].edit_n > -1)
			gtk_button_set_label(GTK_BUTTON(t_main.button_activity),text_cancel_editing);
		g_signal_connect_swapped(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),itovp(n));
		gtk_widget_set_visible(t_main.button_activity,TRUE);
	}
	else
		gtk_widget_set_visible(t_main.button_activity,FALSE);
	gtk_widget_set_size_request(t_main.button_activity, size_icon_send_width, size_icon_bottom_right);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.button_activity);

	gtk_box_append(GTK_BOX(t_main.panel_right), write_box);

	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		pthread_rwlock_rdlock(&mutex_expand_group); // 🟧
		struct msg_list *page = group[g].msg_first;
		pthread_rwlock_unlock(&mutex_expand_group); // 🟩
		while(page)
		{
			if(page->message_next)
				ui_print_message(page->n,page->i,0); // no scroll
			else
				ui_print_message(page->n,page->i,1); // last message, scroll
			page = page->message_next;
		}
	}
	else
	{
		const int max_i = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,max_i));
		const int min_i = getter_int(n,INT_MIN,-1,offsetof(struct peer_list,min_i));
		for(int i = min_i; i <= max_i; i++)
		{ // Display messages
			if(i == max_i)
				ui_print_message(n,i,1); // scroll, but no new message
			else
				ui_print_message(n,i,0); // no scroll
		}
	}
	/* NOTE: This block MUST come AFTER ui_print_message */
	GtkNoSelection *ns = gtk_no_selection_new (G_LIST_MODEL (t_main.list_store_chat));
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "bind", G_CALLBACK(message_builder),NULL); // not using unbind, setup, etc. Have tried setup but it didn't work as well as we'd hope.
/*NO	GtkWidget *list_view = gtk_column_view_new(GTK_SELECTION_MODEL (ns));
	gtk_column_view_set_row_factory(GTK_COLUMN_VIEW(list_view),factory);
	gtk_column_view_insert_column(GTK_COLUMN_VIEW(list_view),0,gtk_column_view_column_new("fishsticks",factory));	NO*/
	GtkWidget *list_view = gtk_list_view_new (GTK_SELECTION_MODEL (ns), factory);
	if(INVERSION_TEST)
	{
	// OPTION 1, do not delete. This seperates the list view and the scroll bar (though effect is the same)
	//	gtk_widget_add_css_class(list_view,"invert-vertical");
	//	gtk_widget_add_css_class(gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(t_main.scrolled_window_right)),"invert-vertical");
	// OPTION 2, do not delete. This just inverts the scrolled window
		gtk_widget_add_css_class(t_main.scrolled_window_right,"invert-vertical");
	}

	gtk_widget_add_css_class(list_view, "invisible"); // XXX important
	gtk_widget_set_vexpand(list_view, TRUE); // works, do not remove
	if(MESSAGES_START_AT_TOP_OF_WINDOW == 0)
		gtk_widget_set_valign(list_view, GTK_ALIGN_END);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_right), list_view); // Do not change. ListView should be a direct child of a Scrolled Window otherwise weird things happen with > 205 widgets.
}

GtkWidget *ui_add_chat_node(const int n,const int call_n,const int call_c,void (*callback_click)(const void *),const int minimal_size)
{ /* XXX Note: Populate_list() is more refined and flexible than this. This only handles CTRL onions, status 0 or 1. */
	GtkWidget *chat_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	if(minimal_size == 0)
		gtk_widget_set_size_request(chat_info,size_peerlist_minimum_width,size_peerlist_button_height);
	else
		gtk_widget_set_size_request(chat_info,size_peerlist_minimum_width,size_peerlist_button_height/2);
//	gtk_button_set_child(GTK_BUTTON (button_peer), chat_info);
	GtkWidget *chat_info_vert = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	gtk_widget_set_valign(chat_info_vert, GTK_ALIGN_CENTER);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,owner));
	const uint8_t status = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,status));
	const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,sendfd_connected));
	const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,offsetof(struct peer_list,recvfd_connected));
	size_t iter = 0;
	if(call_n > -1 && call_c > -1 && minimal_size == 2)
	{ // Must determine the iter
		for( ; iter < sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int) ; iter++)
			if(t_peer[call_n].t_call[call_c].participating[iter] == n)
				break;
		if(iter == sizeof(t_peer[call_n].t_call[call_c].participating)/sizeof(int))
		{ // Sanity check. Might be fatal.
			error_simple(0,"Serious error in ui_add_chat_node caused by race condition. Coding error. Report this.");
			return NULL;
		}
	}
	/* Add Click Function Callbacks */
	if(minimal_size == 2 || minimal_size == 3)
	{ // This is only passed for Group Peerlist, and Participant List
		// Build popover
		GtkWidget *popover = custom_popover_new(chat_info);
		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
		gtk_widget_add_css_class(box, "popover_inner");
		create_button(text_audio_call,call_start,itovp(n))
		create_button(text_private_messaging,ui_private_message,itovp(n))
		create_button(text_rename,ui_activity_rename,itovp(n))
		if(t_peer[n].mute)
			create_button(text_unignore,ui_toggle_mute,itovp(n))
		else
			create_button(text_ignore,ui_toggle_mute,itovp(n))
		if(status == ENUM_STATUS_BLOCKED)
			create_button(text_unblock,ui_toggle_block,itovp(n))
		else
			create_button(text_block,ui_toggle_block,itovp(n))
		gtk_popover_set_child(GTK_POPOVER(popover),box);

		GtkGesture *long_press = gtk_gesture_long_press_new(); // NOTE: "cancelled" == single click, "pressed" == long press
		gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(long_press),LONG_PRESS_TIME);
		if(callback_click) // Single click
			g_signal_connect_swapped(long_press, "cancelled", G_CALLBACK(callback_click),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		// Long press
		g_signal_connect_swapped(long_press, "pressed", G_CALLBACK(gtk_popover_popup),GTK_POPOVER(popover)); // DO NOT FREE arg because this only gets passed ONCE.

		gtk_widget_add_controller(chat_info, GTK_EVENT_CONTROLLER(long_press));

		// Tooltip
		const char *identifier;
		char onion[56+1];
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		{
			identifier = text_torxid;
			getter_array(&onion,52+1,n,INT_MIN,-1,offsetof(struct peer_list,torxid));
		}
		else
		{
			identifier = text_onionid;
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,offsetof(struct peer_list,onion));
		}
		char tooltip[256];
		const int g = set_g(n,NULL);
		const uint8_t invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
		int invitor_n = -1;
		if(invite_required)
		{
			unsigned char verification_message[56+crypto_sign_PUBLICKEYBYTES];
			getter_array(verification_message,56,n,INT_MIN,-1,offsetof(struct peer_list,peeronion));
			getter_array(&verification_message[56],crypto_sign_PUBLICKEYBYTES,n,INT_MIN,-1,offsetof(struct peer_list,peer_sign_pk));
			unsigned char invitation[crypto_sign_BYTES];
			getter_array(invitation,sizeof(invitation),n,INT_MIN,-1,offsetof(struct peer_list,invitation));
			invitor_n = group_check_sig(g,(char *)verification_message,sizeof(verification_message),0,invitation,NULL);
			sodium_memzero(verification_message,sizeof(verification_message));
			sodium_memzero(invitation,sizeof(invitation));
		}
		if(invitor_n > -1)
		{
			const int group_n = getter_group_int(g,offsetof(struct group_list,n));
			torx_read(invitor_n) // 🟧🟧🟧
			if(invitor_n == group_n)
				snprintf(tooltip,sizeof(tooltip),"%s: %s\n%s: %s",identifier,onion,text_invitor,text_you);
			else
				snprintf(tooltip,sizeof(tooltip),"%s: %s\n%s: %s",identifier,onion,text_invitor,peer[invitor_n].peernick);
			torx_unlock(invitor_n) // 🟩🟩🟩
		}
		else
			snprintf(tooltip,sizeof(tooltip),"%s: %s",identifier,onion);
		gtk_widget_set_tooltip_text(chat_info,tooltip);
		sodium_memzero(onion,sizeof(onion));
		sodium_memzero(tooltip,sizeof(tooltip));
	}
	else if(callback_click)
	{
		GtkGesture *click = gtk_gesture_click_new();
		g_signal_connect_swapped(click, "pressed", G_CALLBACK(callback_click), itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(chat_info, GTK_EVENT_CONTROLLER(click));
	}

	/* Build Avatar or Online Status Indicator Image */
	GtkWidget *image_peerlist;
	if(owner == ENUM_OWNER_GROUP_CTRL) // Handle group TODO should have a > arrow that goes to downward pointing upon click, to show individual GROUP_PEERs
		image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo));
	else if(status == ENUM_STATUS_BLOCKED) // Handling blocks second
		image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
	else
	{
		if(sendfd_connected > 0 && recvfd_connected > 0) // https://docs.gtk.org/Pango/enum.Weight.html
			image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_green));
		else if(sendfd_connected > 0 && recvfd_connected < 1)
			image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_yellow));
		else if(sendfd_connected < 1 && recvfd_connected > 0)
			image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_orange));
		else
			image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_grey));
	}
	gtk_widget_set_halign(image_peerlist, GTK_ALIGN_FILL);
	gtk_widget_set_valign(image_peerlist, GTK_ALIGN_CENTER);
	GtkWidget *overlay = gtk_overlay_new();
	gtk_overlay_set_child(GTK_OVERLAY(overlay),image_peerlist);
	if(minimal_size == 0)
	{
		gtk_widget_set_size_request(image_peerlist,size_peerlist_icon_size,size_peerlist_icon_size);
		if(t_peer[n].unread > 0)
		{ // Handle "Badges"
			GtkWidget *overlay2 = gtk_overlay_new();
			gtk_widget_set_size_request(overlay2,(int)((double)size_peerlist_icon_size/1.5),(int)((double)size_peerlist_icon_size/1.5));
			gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
			gtk_widget_set_valign(overlay2, GTK_ALIGN_END);
			GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
			gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

			char unread_count[21];
			snprintf(unread_count,sizeof(unread_count),"%zu",t_peer[n].unread);
			GtkWidget *unread_counter = gtk_label_new(unread_count);
			gtk_widget_add_css_class(unread_counter, "unread_count");
			gtk_widget_set_halign(unread_counter, GTK_ALIGN_CENTER);
			gtk_widget_set_valign(unread_counter, GTK_ALIGN_CENTER);
			gtk_overlay_add_overlay(GTK_OVERLAY(overlay2),unread_counter);

			gtk_overlay_add_overlay(GTK_OVERLAY(overlay),overlay2);
		}
	}
	else
	{
		gtk_widget_set_size_request(image_peerlist,size_peerlist_icon_size/2,size_peerlist_icon_size/2);
		if(minimal_size == 2)
		{
		//	if(currently speaking) // TODO
		//	{ // Handle "Badges"
				GtkWidget *overlay2 = gtk_overlay_new();
				gtk_widget_set_size_request(overlay2,(int)((double)size_peerlist_icon_size/2/1.5),(int)((double)size_peerlist_icon_size/2/1.5));
				gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
				gtk_widget_set_valign(overlay2, GTK_ALIGN_END);
				GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
				gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

				gtk_overlay_add_overlay(GTK_OVERLAY(overlay),overlay2);
		//	}
		}
	}

	/* Assemble everything */
	gtk_box_append (GTK_BOX(chat_info), overlay); // For groups, we'll probably do 3 dots in a triangle. 1 Green for each Green user.

	/* Build Peernick Label */
	char *peernick = getter_string(NULL,n,INT_MIN,-1,offsetof(struct peer_list,peernick));
	GtkWidget *chat_name = gtk_label_new(peernick);
	torx_free((void*)&peernick);
	gtk_widget_add_css_class(chat_name, "chat_name");
	gtk_widget_set_halign(chat_name, GTK_ALIGN_START);
	gtk_widget_set_valign(chat_name, GTK_ALIGN_CENTER);

	gtk_box_append (GTK_BOX(chat_info_vert), chat_name);
	if(minimal_size == 0)
	{
		char last_message[80+1] = {0}; // length is arbitrary here, could be more. another issue is that it newline support complicates this.
		if(t_peer[n].buffer_write != NULL && gtk_text_buffer_get_char_count(t_peer[n].buffer_write) > 0)
		{ // Draft exists
			GtkTextIter start, end;
			gtk_text_buffer_get_bounds(t_peer[n].buffer_write, &start, &end);
			char *text = gtk_text_buffer_get_text(t_peer[n].buffer_write, &start, &end, FALSE);
			snprintf(last_message,sizeof(last_message),"%s: %s",text_draft,text);
			g_free(text);
			text = NULL;
		}
		else
		{ // No draft
			int last_message_n;
			int last_message_i = INT_MIN;
			for(int count_back = 0; (last_message_i = set_last_message(&last_message_n,n,count_back)) > INT_MIN ; count_back++)
			{
				if(t_peer[last_message_n].mute && getter_uint8(last_message_n,INT_MIN,-1,offsetof(struct peer_list,owner)) == ENUM_OWNER_GROUP_PEER)
					continue; // do not print, these are hidden messages from ignored users
				else
					break;
			}
			int p_iter;
			if(last_message_i > INT_MIN && (p_iter = getter_int(last_message_n,last_message_i,-1,offsetof(struct message_list,p_iter))) > -1)
			{
				const int max_i = getter_int(last_message_n,INT_MIN,-1,offsetof(struct peer_list,max_i));
				pthread_rwlock_rdlock(&mutex_protocols); // 🟧
				const uint16_t protocol = protocols[p_iter].protocol;
				const uint8_t file_offer = protocols[p_iter].file_offer;
				const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
				pthread_rwlock_unlock(&mutex_protocols); // 🟩
				const char *message = getter_string(NULL,last_message_n,last_message_i,-1,offsetof(struct message_list,message));
				if(max_i > INT_MIN/* && protocol > 0*/ && message)
				{
					int prefix = 0; // NOTE: could be larger than size due to weird way snprintf returns
					const uint8_t stat = getter_uint8(last_message_n,last_message_i,-1,offsetof(struct message_list,stat));
					if(stat == ENUM_MESSAGE_RECV && t_peer[n].unread > 0) // NOTE: This n needs to be n, not last_message_n. n is the group_ctrl whereas last_message_n is peer
					{}//	prefix = snprintf(last_message,sizeof(last_message),"🔔 "); // this bell is 4char
					else if(stat == ENUM_MESSAGE_FAIL && owner != ENUM_OWNER_GROUP_CTRL)
						prefix = snprintf(last_message,sizeof(last_message),"%s: ",text_queued);
					else if(stat != ENUM_MESSAGE_RECV)
						prefix = snprintf(last_message,sizeof(last_message),"%s: ",text_you);
					if(file_offer)
					{ // Last message is file offer
						int file_n = last_message_n;
						if(protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP || protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP_DATE_SIGNED)
						{
							const int g = set_g(last_message_n,NULL);
							file_n = getter_group_int(g,offsetof(struct group_list,n));
						}
						const int f = set_f(file_n,(const unsigned char*)message,CHECKSUM_BIN_LEN-1);
						if(f > -1)
						{
							torx_read(file_n) // 🟧🟧🟧
							snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",peer[file_n].file[f].filename);
							torx_unlock(file_n) // 🟩🟩🟩
						}
					}
					else if(null_terminated_len)
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",message);
					else if(protocol == ENUM_PROTOCOL_GROUP_OFFER || protocol == ENUM_PROTOCOL_GROUP_OFFER_FIRST)
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",text_group_offer);
					else if(protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE || protocol == ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED)
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",text_audio_message);
					else if(protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED)
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",text_sticker);
					else
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"Protocol: %d",protocol);
				}
				torx_free((void*)&message);
			}
		}
		int count = sizeof(last_message);
		int permitted_newlines = 1;
		while(count--)
		{ // change newlines to null bytes, after the permitted number
			if(last_message[count] == '\n')
			{
				if(permitted_newlines < 1)
					last_message[count] = '\0';
				permitted_newlines--;
			}
		}
		/* Build Last Message Label */
		GtkWidget *chat_message = gtk_label_new(last_message);
		gtk_widget_add_css_class(chat_message, "last_message");
		gtk_widget_set_halign(chat_message, GTK_ALIGN_START);
		gtk_widget_set_valign(chat_message, GTK_ALIGN_CENTER);
		gtk_box_append (GTK_BOX(chat_info_vert), chat_message);
	}
	gtk_box_append (GTK_BOX(chat_info), chat_info_vert);

	if(minimal_size == 2)
	{
		struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
		printing->n = call_n;
		printing->i = call_c;
		printing->scroll = (int)iter;

		GtkWidget *button_mic = gtk_button_new();
		gtk_widget_add_css_class(button_mic, "invisible");
		if(t_peer[call_n].t_call[call_c].participant_mic[iter])
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(mic_off_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(mic_off_light)));
		}
		else
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_mic),gtk_image_new_from_paintable(GDK_PAINTABLE(microphone_light)));
		}
		gtk_widget_set_size_request(button_mic, size_peerlist_icon_size, size_peerlist_icon_size);
		g_signal_connect(button_mic, "clicked", G_CALLBACK(toggle_mic),printing);
		gtk_box_append(GTK_BOX(chat_info),button_mic);

		GtkWidget *button_speaker = gtk_button_new();
		gtk_widget_add_css_class(button_speaker, "invisible");
		if(t_peer[call_n].t_call[call_c].participant_speaker[iter])
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_off_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_off_light)));
		}
		else
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_up_dark)));
			else
				gtk_button_set_child(GTK_BUTTON(button_speaker),gtk_image_new_from_paintable(GDK_PAINTABLE(volume_up_light)));
		}
		gtk_widget_set_size_request(button_speaker, size_peerlist_icon_size, size_peerlist_icon_size);
		g_signal_connect(button_speaker, "clicked", G_CALLBACK(toggle_speaker),printing);
		gtk_box_append(GTK_BOX(chat_info),button_speaker);
	}
	return chat_info;
}

static void ui_show_main_screen(GtkWidget *window)
{ // This sets up a lot of stuff that only has to be done once.
	gtk_widget_add_css_class(t_main.main_window, "window_main");
	ui_determine_orientation();
	/* Build Left Panel */
	t_main.panel_left = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_set_halign(t_main.panel_left, GTK_ALIGN_FILL);
	gtk_widget_set_valign(t_main.panel_left, GTK_ALIGN_FILL);
	gtk_widget_set_vexpand(t_main.panel_left, TRUE); // Forces it to grow to fit its parent
//	gtk_widget_add_css_class(t_main.panel_left, "panel_left");
	/* Build Logo */
	GtkWidget *logo = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo));
	gtk_widget_set_size_request(logo, size_logo_top, size_logo_top);
	/* Build Search Bar */
	t_main.search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
	gtk_widget_set_halign(t_main.search_panel, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(t_main.search_panel, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_top(t_main.search_panel, size_spacing_eight);
	gtk_widget_set_margin_bottom(t_main.search_panel,size_margin_fifteen);
	gtk_widget_set_margin_start(t_main.search_panel,size_spacing_eight);
	gtk_widget_set_margin_end(t_main.search_panel,size_spacing_eight);
	t_main.entry_search = gtk_entry_new();
	gtk_widget_set_valign(t_main.entry_search, GTK_ALIGN_CENTER);
/*	gtk_widget_set_halign(t_main.entry_search, GTK_ALIGN_FILL); // DOES NOT WORK TODO
	gtk_widget_set_hexpand(t_main.search_panel, TRUE); // DOES NOT WORK TODO 
	gtk_widget_set_hexpand(t_main.entry_search, TRUE); // DOES NOT WORK TODO */
	gtk_widget_add_css_class(t_main.entry_search, "search_field");
	gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_search))), 56);
	gtk_entry_set_placeholder_text(GTK_ENTRY(t_main.entry_search),text_placeholder_search);
	g_signal_connect_swapped(t_main.entry_search, "notify::text", G_CALLBACK (ui_populate_peers),itovp(0)); // Callback for typed characters  // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect_swapped(t_main.entry_search, "notify::text", G_CALLBACK (ui_populate_peers),itovp(ENUM_STATUS_GROUP_CTRL)); // Callback for typed characters  // DO NOT FREE arg because this only gets passed ONCE.
	gtk_entry_set_icon_from_gicon(GTK_ENTRY(t_main.entry_search),GTK_ENTRY_ICON_SECONDARY, global_theme == DARK_THEME ? search_dark : search_light);
	/* Assemble Panel from Above Built Objects */
	gtk_box_append(GTK_BOX(t_main.search_panel), logo);
	gtk_box_append(GTK_BOX(t_main.search_panel), t_main.entry_search);

	/* Setup window including frame, panel */
	GtkWidget *frame = gtk_frame_new (NULL);
	gtk_widget_add_css_class(frame, "frame");
	t_main.paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_set_start_child (GTK_PANED (t_main.paned), t_main.panel_left);
	gtk_paned_set_end_child (GTK_PANED (t_main.paned), t_main.panel_right);
	gtk_paned_set_resize_start_child (GTK_PANED (t_main.paned),FALSE);	// THIS TOOK FOREVER TO FIND OH MY GOD

	gtk_frame_set_child (GTK_FRAME (frame), t_main.paned);

	/* Setup first window */
	if(vertical_mode > 0)
	{
		vertical_mode = 1;
		ui_decorate_panel_left(-1);
	}
	else
	{
		t_main.window = window_main;
		ui_decorate_panel_left(-1);
		ui_show_generate();
	}

	/* Assemble everything */
	gtk_box_append(GTK_BOX(t_main.panel_left), t_main.search_panel);
//	if(t_main.scrolled_window_peer != NULL)
//		gtk_stack_remove(GTK_STACK (t_main.panel_left_stack), t_main.scrolled_window_peer);
//	if(t_main.scrolled_window_group != NULL)
//		gtk_stack_remove(GTK_STACK (t_main.panel_left_stack), t_main.scrolled_window_group);
//	if(t_main.scrolled_window_block != NULL)
//		gtk_stack_remove(GTK_STACK (t_main.panel_left_stack), t_main.scrolled_window_block);
	t_main.scrolled_window_peer = gtk_scrolled_window_new();
	t_main.scrolled_window_group = gtk_scrolled_window_new();
	t_main.scrolled_window_block = gtk_scrolled_window_new();
	gtk_widget_set_size_request(t_main.scrolled_window_peer, size_peerlist_minimum_width, size_peerlist_minimum_height);
	gtk_widget_set_size_request(t_main.scrolled_window_group, size_peerlist_minimum_width, size_peerlist_minimum_height);
	gtk_widget_set_size_request(t_main.scrolled_window_block, size_peerlist_minimum_width, size_peerlist_minimum_height);

	/* Build Stack */
	t_main.panel_left_stack = gtk_stack_new();
	gtk_stack_add_titled (GTK_STACK (t_main.panel_left_stack), t_main.scrolled_window_peer, "0",text_peer);
	gtk_stack_add_titled (GTK_STACK (t_main.panel_left_stack), t_main.scrolled_window_group, "1",text_group);
	gtk_stack_add_titled (GTK_STACK (t_main.panel_left_stack), t_main.scrolled_window_block, "2",text_block);
	g_signal_connect_swapped(t_main.panel_left_stack, "notify::visible-child", G_CALLBACK(ui_populate_peers), itovp(0)); // DO NOT FREE arg because this only gets passed ONCE.
	g_signal_connect_swapped(t_main.panel_left_stack, "notify::visible-child", G_CALLBACK(ui_populate_peers), itovp(ENUM_STATUS_GROUP_CTRL)); // DO NOT FREE arg because this only gets passed ONCE.
//TODO move start TODO


//TODO move end TODO

	/* Append stack switcher and stack */
	t_main.custom_switcher_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_zero);
	gtk_widget_set_margin_start(t_main.custom_switcher_box,size_margin_eight);
	gtk_widget_set_margin_end(t_main.custom_switcher_box,size_margin_eight);
	gtk_box_append(GTK_BOX(t_main.panel_left),t_main.custom_switcher_box); // t_main.custom_switcher_box
	gtk_box_append(GTK_BOX(t_main.panel_left),t_main.panel_left_stack);

	GtkWidget *custom_switcher = gtk_custom_switcher_new(GTK_STACK (t_main.panel_left_stack),GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_append(GTK_BOX(t_main.custom_switcher_box),custom_switcher);

	/* Set Panel Size Requests */
	gtk_window_set_child(GTK_WINDOW(window), frame);
//	gtk_widget_set_size_request (t_main.panel_left,size_natural,size_natural);
//	gtk_widget_set_size_request (t_main.panel_right,size_natural,size_natural);

	ui_populate_peers(itovp(0));
	ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));

//	if(!gtk_widget_get_visible(window))
//		gtk_window_present(GTK_WINDOW(window));
}

static int login_act_idle(void *arg)
{
	const int value = vptoi(arg);
	if(value == 0 && threadsafe_read_uint8(&mutex_global_variable,&tor_running))
		ui_show_main_screen(t_main.main_window);
	else if(value == 0)
		ui_show_missing_binaries();
	else
	{
		gtk_button_set_label(GTK_BUTTON(t_main.auth_button),text_enter);
		gtk_label_set_label(GTK_LABEL(t_main.auth_error),text_incorrect);
		gtk_editable_delete_text(GTK_EDITABLE(t_main.auth_entry_pass),0,-1);

		ui_input_bad(t_main.auth_entry_pass);
	}
	return 0;
}

void login_cb_ui(const int value)
{ // GUI Callback
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,login_act_idle,itovp(value),NULL);
}

static void ui_send_login(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;
	if(threadsafe_read_uint8(&mutex_global_variable,&lockout))
		return;
	gtk_button_set_label(GTK_BUTTON(t_main.auth_button),text_wait);
//	GtkWidget **entry_field = (GtkWidget **)data;
//	GtkPasswordEntryBuffer *password_field_buf = GTK_PASSWORD_ENTRY_BUFFER(gtk_password_entry_buffer_new());
	char *password = gtk_editable_get_chars(GTK_EDITABLE(t_main.auth_entry_pass),0,-1);
	login_start(password);
	sodium_memzero(password,strlen(password));
	g_free(password);
	password = NULL;
}

static gboolean switch_state_set(GtkSwitch *self,gboolean state,gpointer data)
{
	(void) self;
	(void) data;
	uint8_t censored_region_local = threadsafe_read_uint8(&mutex_global_variable,&censored_region);
	if(state == TRUE && censored_region_local == 0)
	{
		censored_region_local = 1;
		threadsafe_write(&mutex_global_variable,&censored_region,&censored_region_local,sizeof(censored_region_local));
		sql_setting(1,-1,"censored_region","1",1);
	}
	else if(state == FALSE && censored_region_local == 1)
	{
		censored_region_local = 0;
		threadsafe_write(&mutex_global_variable,&censored_region,&censored_region_local,sizeof(censored_region_local));
		sql_setting(1,-1,"censored_region","0",1);
	}
	return FALSE;
}

void ui_show_auth_screen(void)
{ // Note: This runs twice if there is a saved language setting.
	if(!tor_location) // shouldn't need mutex
	{
		error_simple(0,"Must set Tor location.");
		return;
	}
	t_main.window = window_auth; // Auth

//	t_main.auth_entry_pass = GTK_PASSWORD_ENTRY(gtk_password_entry_new());	// WORKS 
	t_main.auth_entry_pass = gtk_password_entry_new(); // WORKS
	gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(t_main.auth_entry_pass),TRUE);
	t_main.auth_error = gtk_label_new(""); // do not set to null

	/* Build background box */
	GtkWidget *auth_background = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_five);
	gtk_widget_set_halign(auth_background, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(auth_background, GTK_ALIGN_CENTER);

	GtkWidget *auth_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_zero);
	gtk_widget_set_halign(auth_logo_box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(auth_logo_box, GTK_ALIGN_CENTER);

	/* Build title, logo, password area */
/*	GtkWidget *auth_logo = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo)); // goat
	gtk_widget_set_size_request(auth_logo,size_logo_auth,size_logo_auth);
	gtk_box_append (GTK_BOX(auth_logo_box), auth_logo);	*/

// TESTING TODO Do NOT delete. This is a ready-to-go colored .svg / symbolic icon setup, but it won't work until GTK 4.14, according to #gtk
	GFile *file = g_file_new_for_uri("resource:///org/torx/gtk4/other/scalable/apps/logo-torx-symbolic.svg");
	GtkIconPaintable *paintable = gtk_icon_paintable_new_for_file (file,size_logo_auth,1);
	if(gtk_icon_paintable_is_symbolic(paintable) == TRUE)
		error_simple(0,RED"is symbolic"RESET); // goat
	else
		error_simple(0,RED"is NOT symbolic"RESET);
	GdkRGBA colors = {1.0,0.0,0.0,1.0};
//	gdk_rgba_parse(&colors,"rgb(200,70,70)");
	GdkSnapshot* snapshot = gtk_snapshot_new();
	gtk_symbolic_paintable_snapshot_symbolic(GTK_SYMBOLIC_PAINTABLE(paintable),snapshot,size_logo_auth,size_logo_auth,&colors,1);
	GdkPaintable *colored = gtk_snapshot_free_to_paintable(snapshot,NULL);
	GtkWidget *image_logo = gtk_image_new_from_paintable(GDK_PAINTABLE(colored));
	gtk_widget_set_size_request(image_logo,size_logo_auth,size_logo_auth);
	gtk_box_append (GTK_BOX(auth_logo_box),image_logo);
// TESTING TODO
	GtkWidget *auth_text_next_logo = gtk_label_new(text_title);
	gtk_box_append (GTK_BOX(auth_logo_box), auth_text_next_logo);
	if(threadsafe_read_uint8(&mutex_global_variable,&lockout))
		t_main.auth_button = gtk_button_new_with_label (text_wait);
	else
		t_main.auth_button = gtk_button_new_with_label (text_enter);
	gtk_widget_set_size_request(t_main.auth_button, size_button_auth_width, size_button_auth_height);
//	gtk_widget_set_size_request(t_main.auth_entry_pass, size_entry_auth, size_natural);
	gtk_widget_set_margin_top(t_main.auth_button, size_margin_fifteen);
	gtk_editable_set_alignment(GTK_EDITABLE(t_main.auth_entry_pass),0.5);
	GtkWidget *auth_text_under_logo = gtk_label_new(text_enter_password);

	/* Send password information to be verified */
	g_signal_connect(t_main.auth_button, "clicked", G_CALLBACK (ui_send_login), NULL); // DO NOT FREE arg because this only gets passed ONCE.

	/* This works but throws errors for unknown reasons */
	g_signal_connect(t_main.auth_entry_pass, "activate", G_CALLBACK (ui_send_login), NULL); // DO NOT FREE arg because this only gets passed ONCE.

	/* Assemble everything */
	gtk_widget_add_css_class(t_main.main_window, "window_auth");
	gtk_widget_add_css_class(auth_background, "main_box");
//	gtk_widget_set_name(t_main.auth_button, "auth_button");
	gtk_widget_add_css_class(t_main.auth_button,"auth_button");
//	gtk_style_context_add_class(gtk_widget_get_style_context(t_main.auth_button),"auth_button");	// XXX this is for the :hover functionality. Important.
//	gtk_style_context_add_provider(gtk_widget_get_style_context(t_main.auth_button), GTK_STYLE_PROVIDER(t_main.provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
//	ui_load_css(t_main.provider, t_main.auth_button);
	gtk_widget_add_css_class(auth_text_under_logo, "login_label_subtext");
	gtk_widget_add_css_class(auth_text_next_logo, "login_label");
	gtk_widget_add_css_class(t_main.auth_error, "auth_error");

	gtk_box_append (GTK_BOX(auth_background), auth_logo_box);
	gtk_box_append (GTK_BOX(auth_background), auth_text_under_logo);
	gtk_box_append (GTK_BOX(auth_background), t_main.auth_error);
	gtk_box_append (GTK_BOX(auth_background), t_main.auth_entry_pass);
	gtk_box_append (GTK_BOX(auth_background), t_main.auth_button);
	if(snowflake_location)
	{
		t_main.switch_censored_region = gtk_switch_new();
		if(threadsafe_read_uint8(&mutex_global_variable,&censored_region))
			gtk_switch_set_active(GTK_SWITCH(t_main.switch_censored_region),TRUE);
		g_signal_connect(t_main.switch_censored_region, "state-set", G_CALLBACK(switch_state_set), NULL);
		GtkWidget *auth_box_censored_region = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,size_spacing_five);
		GtkWidget *auth_text_censored_region = gtk_label_new(text_censored_region);
		gtk_widget_add_css_class(auth_text_censored_region, "login_label_subtext");
		gtk_widget_set_halign(auth_text_censored_region, GTK_ALIGN_END);
		gtk_widget_set_hexpand(auth_text_censored_region, TRUE);
		gtk_box_append (GTK_BOX(auth_box_censored_region), auth_text_censored_region);
		gtk_box_append (GTK_BOX(auth_box_censored_region), t_main.switch_censored_region);
		gtk_box_append (GTK_BOX(auth_background), auth_box_censored_region);
	}
	gtk_window_set_child (GTK_WINDOW(t_main.main_window),auth_background);
}

static void try_now(void)
{
	const uint8_t tor_running_local = threadsafe_read_uint8(&mutex_global_variable,&tor_running);
	const uint8_t keyed_local = threadsafe_read_uint8(&mutex_global_variable,&keyed);
	if(!keyed_local)
		ui_show_auth_screen();
	else if(tor_running_local) // This is not a certainty. The binary could be bad.
		ui_show_main_screen(t_main.main_window);
	else // Bad binary, clear it
		ui_show_missing_binaries();
}

void ui_show_missing_binaries(void)
{ // Shows on startup if tor_location is unset
	t_main.window = window_auth; // Auth
	gtk_widget_add_css_class(t_main.main_window, "window_auth");

	GtkWidget *auth_background = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_five);
	gtk_widget_set_halign(auth_background, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(auth_background, GTK_ALIGN_CENTER);
	gtk_widget_add_css_class(auth_background, "main_box");

	GtkWidget *title = gtk_label_new(text_settings);
	gtk_widget_add_css_class(title, "page_title");
	gtk_box_append(GTK_BOX(auth_background), title);

	ui_determine_orientation();

	gtk_box_append (GTK_BOX (auth_background), ui_choose_binary(&tor_location,"tor_location",text_tor));
	gtk_box_append (GTK_BOX (auth_background), ui_choose_binary(&snowflake_location,"snowflake_location",text_snowflake));
	gtk_box_append (GTK_BOX (auth_background), ui_choose_binary(&lyrebird_location,"lyrebird_location",text_lyrebird));
	gtk_box_append (GTK_BOX (auth_background), ui_choose_binary(&conjure_location,"conjure_location",text_conjure));
	gtk_box_append (GTK_BOX (auth_background), ui_spinbutton(text_set_tor_port_socks,ENUM_SPIN_TOR_PORT_SOCKS,(int)threadsafe_read_uint16(&mutex_global_variable,&tor_socks_port),0,65536));
	gtk_box_append (GTK_BOX (auth_background), ui_spinbutton(text_set_tor_port_ctrl,ENUM_SPIN_TOR_PORT_CONTROL,(int)threadsafe_read_uint16(&mutex_global_variable,&tor_ctrl_port),0,65536));

	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	gtk_box_append (GTK_BOX (auth_background), ui_setting_entry(ui_tor_control_password_change,text_set_tor_password,control_password_clear));
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩

	GtkWidget *button = gtk_button_new_with_label (text_enter);
	gtk_widget_set_size_request(button, size_button_auth_width, size_button_auth_height);
	gtk_widget_set_margin_top(button, size_margin_fifteen);

	g_signal_connect(button, "clicked", G_CALLBACK (try_now), NULL); // DO NOT FREE arg because this only gets passed ONCE.

	gtk_widget_add_css_class(button,"auth_button");
	gtk_box_append (GTK_BOX(auth_background), button);

	gtk_window_set_child (GTK_WINDOW(t_main.main_window),auth_background);
}

static int icon_failure_idle(void *arg)
{
	const uint16_t icon_port = (uint16_t) vptoi(arg);
	error_printf(0,"Appindicator is NOT functioning on port %u",icon_port);
	appindicator_functioning = 0;
	return 0;
}

static int icon_communicator_idle(void *arg)
{
	evutil_socket_t sockfd = (evutil_socket_t)(intptr_t)arg;
	char message[256];
	size_t diff,str_len = 0; // I think only str_len must be initialized here
//	printf("Checkpoint data: %d = %lu - %lu\n",(int)totalUnreadPeer - (int)last_totalUnreadPeer,totalUnreadPeer,last_totalUnreadPeer);
	if ((diff = totalUnreadPeer - last_totalUnreadPeer) && (diff != 1 ? (str_len = (size_t) snprintf(message,sizeof(message),"p%zu",totalUnreadPeer)) : (str_len = (size_t) snprintf(message,sizeof(message),"p"))) > 0)
	{
	//	printf("Checkpoint awesome: %s\n",message);
		if(send(SOCKET_CAST_OUT sockfd, message, SOCKET_WRITE_SIZE str_len+1,0) < 0)
			goto end;
		last_totalUnreadPeer = totalUnreadPeer;
	}
	if ((diff = totalUnreadGroup - last_totalUnreadGroup) && (diff != 1 ? (str_len = (size_t) snprintf(message,sizeof(message),"g%zu",totalUnreadGroup)) : (str_len = (size_t) snprintf(message,sizeof(message),"g"))) > 0)
	{
		if(send(SOCKET_CAST_OUT sockfd, message, SOCKET_WRITE_SIZE str_len+1,0) < 0)
			goto end;
		last_totalUnreadGroup = totalUnreadGroup;
	}
	if ((diff = totalIncoming - last_totalIncoming) && (diff != 1 ? (str_len = (size_t) snprintf(message,sizeof(message),"i%zu",totalIncoming)) : (str_len = (size_t) snprintf(message,sizeof(message),"i"))) > 0)
	{
		if(send(SOCKET_CAST_OUT sockfd, message, SOCKET_WRITE_SIZE str_len+1,0) < 0)
			goto end;
		last_totalIncoming = totalIncoming;
	}
	return TRUE;
	end: {}
	error_simple(0,"Icon_communicator failed after successful connection");
	if(sockfd > 0)
		evutil_closesocket(sockfd);
	icon_failure_idle(arg);
	return FALSE;
}

static void *icon_communicator(void* arg)
{ // XXX NOT IN UI THREAD, but is threadsafe because we don't access any GTK stuff or global variables directly XXX
	pusher(zero_pthread,(void*)&thread_icon_communicator)
	setcanceltype(TORX_PHTREAD_CANCEL_TYPE,NULL);
	const uint16_t icon_port = (uint16_t) vptoi(arg);
	evutil_socket_t sockfd;
	if ((sockfd = SOCKET_CAST_IN socket(AF_INET, SOCK_STREAM, 0)) < 0)
		goto end;
	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(icon_port);
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
		goto end;
	evutil_socket_t ret = {0};
	for (uint8_t attempts = ICON_COMMUNICATION_ATTEMPTS; attempts ; attempts--)
	{
		if ((ret = connect(SOCKET_CAST_OUT sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == 0)
			break;
		if(attempts - 1) // don't sleep after last attempt
			sleep(1);
	}
	if(!ret)
	{ // Succssfully connected
		appindicator_functioning = 1; // yes, set here. safe to set despite threaded. no need idle or locks because icon_communicator_idle isn't started yet.
		g_timeout_add(500,icon_communicator_idle,(void*)(intptr_t)sockfd);
		return 0;
	}
	end: {}
	if(sockfd > 0)
		evutil_closesocket(sockfd);
	g_idle_add_full(G_PRIORITY_HIGH_IDLE,icon_failure_idle,arg,NULL); // XXX
	if(tray_pid > 0)
	{ // Do not remove this error message. It can be triggered on very slow systems, indicating that ICON_COMMUNICATION_ATTEMPTS must be increased.
		error_printf(0,"Icon communicator failed to connect in %d seconds, despite a PID existing. Killing PID.",ICON_COMMUNICATION_ATTEMPTS);
		pid_kill(tray_pid,SIGTERM);
	}
	else
		error_simple(0,"Icon_communicator failed before successful connection.");
	pthread_exit(NULL);
	return NULL;
}

static inline char *path_generator(const char *directory,const char *partial_or_full_path)
{ // Tested on Windows, GDB (full arg0 is full path), normal (arg0 is relative path)
	size_t len_dir,len_path;
	if(!directory || !partial_or_full_path || (len_dir = strlen(directory)) < 2 || (len_path = strlen(partial_or_full_path)) < 2)
	{
		error_simple(-1,"Null directory or partial_or_full_path passed to path_generator");
		return NULL;
	}
	char tmp[PATH_MAX];
	if(partial_or_full_path[0] == '.' && partial_or_full_path[1] == platform_slash) // partial, needs to be stripped
		snprintf(tmp,sizeof(tmp),"%s%s",directory,&partial_or_full_path[1]);
	else if((platform_slash == '/' && /*!strstr(partial_or_full_path,"/")*/partial_or_full_path[0] != '/') || (platform_slash == '\\' && !strstr(partial_or_full_path,"\\") && !strstr(partial_or_full_path,":"))) // must append to dir directly, after a platform_slash
		snprintf(tmp,sizeof(tmp),"%s%c%s",directory,platform_slash,partial_or_full_path);
	else // should be complete?
		snprintf(tmp,sizeof(tmp),"%s",partial_or_full_path);
	char *p;
	if(get_file_size(tmp) < REALISTIC_MINIMUM_BINARY_SIZE)
	{ // XXX NOTE: We are prioritizing our own relative path over which. We only fall-back to which.
		p = which(basename(tmp)); // fine to call basename here, despite possibility for modification
		if(!p || get_file_size(p) < REALISTIC_MINIMUM_BINARY_SIZE)
		{
			error_simple(0,"Path generator ultimately failed to generate a valid path. Minimize to tray unavailable. Coding error. Report this to UI Devs.");
			return NULL;
		}
	}
	else
		p = tmp;
	const size_t final_len = strlen(p);
	char *final = torx_insecure_malloc(final_len+1);
	memcpy(final,p,final_len+1);
	return final;
}

//static void activate (GtkApplication* app, gpointer user_data)
static void ui_activate(GtkApplication *application,void *arg)
{ // Cannot set window position, GTK removed it.// https://stackoverflow.com/questions/62614703/how-to-make-window-centered-in-gtk4
	(void) arg;
	if(running)
	{ // Already running, show the window to the user. XXX NOTE: Arguments passed on command line are in a different process and therefore not available. (ie: -m, -f, -d)
		if(t_main.main_window && gtk_widget_get_visible(t_main.main_window))
			gtk_widget_set_visible(t_main.main_window,FALSE); // hide
		else if(t_main.main_window)
			gtk_window_present(GTK_WINDOW(t_main.main_window)); // show
		return;
	}
	running = 1;

	if(getcwd(starting_dir,sizeof(starting_dir))) // Must be before initial
		error_printf(4,"Starting directory: %s\n",starting_dir);
	binary_path = path_generator(starting_dir,argv_0);

	/* Options configurable by client */
	torx_debug_level(4);
	reduced_memory = 2; // TODO probably remove before release

	/* Utilizing setter functions instead of direct setting (ex: stream_registered = stream_cb_ui;) for typechecking */
	initialize_n_setter(initialize_n_cb_ui);
	initialize_i_setter(initialize_i_cb_ui);
	initialize_f_setter(initialize_f_cb_ui);
	initialize_g_setter(initialize_g_cb_ui);
	shrinkage_setter(shrinkage_cb_ui);
	expand_file_struc_setter(expand_file_struc_cb_ui);
	expand_message_struc_setter(expand_message_struc_cb_ui);
	expand_peer_struc_setter(expand_peer_struc_cb_ui);
	expand_group_struc_setter(expand_group_struc_cb_ui);
	transfer_progress_setter(transfer_progress_cb_ui);
	change_password_setter(change_password_cb_ui);
	incoming_friend_request_setter(incoming_friend_request_cb_ui);
	onion_deleted_setter(onion_deleted_cb_ui);
	peer_online_setter(peer_online_cb_ui);
	peer_offline_setter(peer_offline_cb_ui);
	peer_new_setter(peer_new_cb_ui);
	onion_ready_setter(onion_ready_cb_ui);
	tor_log_setter(tor_log_cb_ui);
	error_setter(error_cb_ui);
	fatal_setter(fatal_cb_ui);
	custom_setting_setter(custom_setting_cb_ui);
	message_new_setter(message_new_cb_ui);
	message_modified_setter(message_modified_cb_ui);
	message_deleted_setter(message_deleted_cb_ui);
	login_setter(login_cb_ui);
	peer_loaded_setter(peer_loaded_cb_ui);
	cleanup_setter(cleanup_cb_ui);
	stream_setter(stream_cb_ui);
	message_extra_setter(message_extra_cb_ui);
	message_more_setter(message_more_cb_ui);

	t_peer = torx_insecure_malloc(sizeof(struct t_peer_list) *11);

	initial();
	gst_init(NULL, NULL);

	protocol_registration(ENUM_PROTOCOL_STICKER_HASH,"Sticker","",0,0,0,1,1,0,0,ENUM_EXCLUSIVE_GROUP_MSG,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED,"Sticker Date Signed","",0,1,1,1,1,0,0,ENUM_EXCLUSIVE_GROUP_MSG,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_HASH_PRIVATE,"Sticker Private","",0,0,0,1,1,0,0,ENUM_EXCLUSIVE_GROUP_PM,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_REQUEST,"Sticker Request","",0,0,0,0,0,0,0,ENUM_EXCLUSIVE_NONE,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_DATA_GIF,"Sticker data","",0,0,0,0,0,0,0,ENUM_EXCLUSIVE_NONE,0,1,ENUM_STREAM_NON_DISCARDABLE); // NOTE: if making !stream, need to move related handler from stream_cb to print_message_idle
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_MSG, "AAC Audio Message", "", 0, 0, 0, 1, 1, 0, 0, ENUM_EXCLUSIVE_GROUP_MSG, 0, 1, 0);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_MSG_DATE_SIGNED, "AAC Audio Message Date Signed", "", 0, 1, 1, 1, 1, 0, 0, ENUM_EXCLUSIVE_GROUP_MSG, 0, 1, 0);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_MSG_PRIVATE, "AAC Audio Message Private", "", 0, 0, 0, 1, 1, 0, 0, ENUM_EXCLUSIVE_GROUP_PM, 0, 1, 0);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN, "AAC Audio Stream Join", "", 0, 0, 0, 0, 1, 0, 0, ENUM_EXCLUSIVE_GROUP_MSG, 0, 1, ENUM_STREAM_NON_DISCARDABLE);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_STREAM_JOIN_PRIVATE, "AAC Audio Stream Join Private", "", 0, 0, 0, 0, 1, 0, 0, ENUM_EXCLUSIVE_GROUP_PM, 0, 1, ENUM_STREAM_NON_DISCARDABLE);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_STREAM_PEERS, "AAC Audio Stream Peers", "", 0, 0, 0, 0, 0, 0, 0, ENUM_EXCLUSIVE_NONE, 0, 1, ENUM_STREAM_DISCARDABLE);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_STREAM_LEAVE, "AAC Audio Stream Leave", "", 0, 0, 0, 0, 1, 0, 0, ENUM_EXCLUSIVE_NONE, 0, 1, ENUM_STREAM_NON_DISCARDABLE);
	protocol_registration(ENUM_PROTOCOL_AAC_AUDIO_STREAM_DATA_DATE, "AAC Audio Data Date", "", 0, 1, 0, 0, 0, 0, 0, ENUM_EXCLUSIVE_NONE, 0, 1, ENUM_STREAM_DISCARDABLE);

	char current_working_directory[PATH_MAX];
	if(getcwd(current_working_directory,sizeof(current_working_directory)))
	{ // Do not use starting_dir because cwd has been changed by initial
		char tdd_full_path[PATH_MAX];
		const int tdd_len = snprintf(tdd_full_path,sizeof(tdd_full_path),"%s%ctor_data_directory",current_working_directory,platform_slash) + 1;
		tor_data_directory = torx_insecure_malloc((size_t)tdd_len);
		memcpy(tor_data_directory,tdd_full_path,(size_t)tdd_len);
	}

	ui_initialize_language(NULL);

	if(!get_file_size(FILENAME_ICON))
	{
		GBytes *bytes = g_resources_lookup_data("/org/torx/gtk4/other/scalable/apps/logo-torx-symbolic.svg",G_RESOURCE_LOOKUP_FLAGS_NONE,NULL);
		size_t size = 0;
		const void *data = g_bytes_get_data(bytes,&size);
		write_bytes(FILENAME_ICON,data,size);
	}
/*	if(!get_file_size(FILENAME_BEEP))
	{
		GBytes *bytes = g_resources_lookup_data("/org/torx/gtk4/other/beep.wav",G_RESOURCE_LOOKUP_FLAGS_NONE,NULL);
		size_t size = 0;
		const void *data = g_bytes_get_data(bytes,&size);
		write_bytes(FILENAME_BEEP,data,size);
	}*/

	/* Load all headerbar resources and resources required by Auth Screen */
	texture_logo = gdk_texture_new_from_resource("/org/torx/gtk4/other/scalable/apps/logo-torx-symbolic.svg");
	texture_headerbar_button1 = gdk_texture_new_from_resource("/org/torx/gtk4/other/close.png");
	texture_headerbar_button2 = gdk_texture_new_from_resource("/org/torx/gtk4/other/minimize.png");
	texture_headerbar_button3 = gdk_texture_new_from_resource("/org/torx/gtk4/other/maximize.png");
	texture_headerbar_button_leave1 = gdk_texture_new_from_resource("/org/torx/gtk4/other/close_hover.png");
	texture_headerbar_button_leave2 = gdk_texture_new_from_resource("/org/torx/gtk4/other/minimize_hover.png");
	texture_headerbar_button_leave3 = gdk_texture_new_from_resource("/org/torx/gtk4/other/maximize_hover.png");

	t_main.main_window = gtk_application_window_new (application);

	/* Set Panel Icon from GResource (works now) */
	GtkIconTheme* icon_theme = gtk_icon_theme_get_for_display (gtk_widget_get_display(t_main.main_window));
	gtk_icon_theme_add_resource_path(icon_theme,"/org/torx/gtk4/other/");
//	gtk_window_set_icon_name(GTK_WINDOW(t_main.main_window),"icon_square"); // Works, specific window. Redundant in our case.
	gtk_window_set_default_icon_name("icon_round"); // Works. https://discourse.gnome.org/t/gtk-4-used-only-48x48-application-icon-that-leads-to-fuzzy-icons-gtk2-works-fine/9757

	gtk_window_set_title (GTK_WINDOW ( t_main.main_window), text_title);
	gtk_window_set_default_size (GTK_WINDOW ( t_main.main_window), size_window_default_width, size_window_default_height);
	gtk_window_set_resizable (GTK_WINDOW ( t_main.main_window),TRUE);

	// Load Auth Page CSS
	t_main.provider = gtk_css_provider_new();
	if(global_theme == -1)
		global_theme = THEME_DEFAULT;
	if(global_theme == DARK_THEME)
		gtk_css_provider_load_from_resource(t_main.provider,RESOURCE_CSS_DARK);
	else if(global_theme == LIGHT_THEME)
		gtk_css_provider_load_from_resource(t_main.provider,RESOURCE_CSS_LIGHT);
	gtk_style_context_add_provider_for_display(gdk_display_get_default(),GTK_STYLE_PROVIDER(t_main.provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

	/* Load All Globally defined icon and Image files that will be required for main window */
	file_file = gdk_texture_new_from_resource("/org/torx/gtk4/other/file.png");
	file_txt = gdk_texture_new_from_resource("/org/torx/gtk4/other/txt.png");
	file_image = gdk_texture_new_from_resource("/org/torx/gtk4/other/image.png");
	file_music = gdk_texture_new_from_resource("/org/torx/gtk4/other/music.png");
	file_zip = gdk_texture_new_from_resource("/org/torx/gtk4/other/zip.png");

	search_dark = g_file_icon_new(g_file_new_for_uri("resource:///org/torx/gtk4/other/search_200dp_D2D2D2.svg"));
	search_light = g_file_icon_new(g_file_new_for_uri("resource:///org/torx/gtk4/other/search_200dp_474747.svg"));
	fail_ico = gdk_texture_new_from_resource("/org/torx/gtk4/other/cancel_200dp_FF0000.svg");
	add_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/add_200dp_ECB365.svg");
	add_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/add_200dp_D2D2D2.svg");
	add_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/add_200dp_474747.svg");
	home_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/home_200dp_ECB365.svg");
	home_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/home_200dp_D2D2D2.svg");
	home_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/home_200dp_474747.svg");
	settings_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/settings_200dp_ECB365.svg");
	settings_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/settings_200dp_D2D2D2.svg");
	settings_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/settings_200dp_474747.svg");
	dot_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/circle_200dp_FF0000.svg");
	dot_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/circle_200dp_27FF00.svg");
	dot_yellow = gdk_texture_new_from_resource("/org/torx/gtk4/other/circle_200dp_F5FF00.svg");
	dot_orange = gdk_texture_new_from_resource("/org/torx/gtk4/other/circle_200dp_FFA500.svg");
	dot_grey = gdk_texture_new_from_resource("/org/torx/gtk4/other/circle_200dp_919191.svg");
	lock_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_200dp_FF0000.svg");
	lock_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_200dp_27FF00.svg");
	lock_yellow = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_200dp_F5FF00.svg");
	lock_orange = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_200dp_FFA500.svg");
	lock_grey = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_200dp_919191.svg");
	unlock_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_open_200dp_FF0000.svg");
	unlock_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_open_200dp_27FF00.svg");
	unlock_yellow = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_open_200dp_F5FF00.svg");
	unlock_orange = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_open_200dp_FFA500.svg");
	unlock_grey = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_open_200dp_919191.svg");
	logging_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/article_200dp_ECB365.svg");
	logging_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/article_200dp_D2D2D2.svg");
	logging_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/article_200dp_474747.svg");
	globe_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/language_200dp_ECB365.svg");
	globe_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/language_200dp_D2D2D2.svg");
	globe_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/language_200dp_474747.svg");
	bell_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/notifications_active_200dp_ECB365.svg");
	bell_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/notifications_off_200dp_D2D2D2.svg");
	bell_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/notifications_off_200dp_474747.svg");
	block_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/block_200dp_FF0000.svg");
	block_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/block_200dp_D2D2D2.svg");
	block_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/block_200dp_474747.svg");
	kill_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/local_fire_department_200dp_ECB365.svg");
	kill_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/local_fire_department_200dp_D2D2D2.svg");
	kill_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/local_fire_department_200dp_474747.svg");
	delete_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/delete_forever_200dp_ECB365.svg");
	delete_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/delete_forever_200dp_D2D2D2.svg");
	delete_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/delete_forever_200dp_474747.svg");
	clear_all_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/clear_all_200dp_D2D2D2.svg");
	clear_all_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/clear_all_200dp_474747.svg");
	clear_all_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/clear_all_200dp_ECB365.svg");
	keyboard_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/keyboard_200dp_D2D2D2.svg");
	keyboard_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/keyboard_200dp_474747.svg");
	microphone_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/mic_200dp_D2D2D2.svg");
	microphone_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/mic_200dp_474747.svg");
	emoji_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/emoji_emotions_200dp_474747.svg");
	emoji_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/emoji_emotions_200dp_D2D2D2.svg");
	gif_box_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/gif_box_200dp_D2D2D2.svg");
	gif_box_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/gif_box_200dp_474747.svg");
	attach_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/attach_file_200dp_474747.svg");
	attach_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/attach_file_200dp_D2D2D2.svg");
	more_vert_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/more_vert_200dp_D2D2D2.svg");
	more_vert_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/more_vert_200dp_474747.svg");
	send_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/send_200dp_D2D2D2.svg");
	send_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/send_200dp_474747.svg");
//	clear_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/clear_200dp_D2D2D2.svg"); // TODO unused so far, for clearing search bar
//	clear_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/clear_200dp_474747.svg"); // TODO unused so far, for clearing search bar
	call_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/call_200dp_474747.svg");
	call_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/call_200dp_D2D2D2.svg");
	call_end_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/call_end_200dp_474747.svg");
	call_end_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/call_end_200dp_D2D2D2.svg");
//	diversity_2_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/diversity_2_200dp_474747.svg"); // TODO unused so far, for replacing "Group"
//	diversity_2_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/diversity_2_200dp_D2D2D2.svg"); // TODO unused so far, for replacing "Group"
	group_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/group_200dp_474747.svg");
	group_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/group_200dp_D2D2D2.svg");
//	groups_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/groups_200dp_474747.svg"); // TODO unused so far, might not use, in favor of diversity_2
//	groups_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/groups_200dp_D2D2D2.svg"); // TODO unused so far, might not use, in favor of diversity_2
	mic_off_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/mic_off_200dp_474747.svg");
	mic_off_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/mic_off_200dp_D2D2D2.svg");
//	no_accounts_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/no_accounts_200dp_474747.svg"); // TODO unused so far, for replacing "Block"
//	no_accounts_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/no_accounts_200dp_D2D2D2.svg"); // TODO unused so far, for replacing "Block"
//	person_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/person_200dp_474747.svg"); // TODO unused so far, for replacing "Peer"
//	person_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/person_200dp_D2D2D2.svg"); // TODO unused so far, for replacing "Peer"
//	phone_callback_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/phone_callback_200dp_474747.svg"); // TODO unused so far, might not use
//	phone_callback_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/phone_callback_200dp_D2D2D2.svg"); // TODO unused so far, might not use
//	phone_callback_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/phone_callback_200dp_00C80A.svg"); // TODO unused so far
//	phone_missed_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/phone_missed_200dp_474747.svg"); // TODO unused so far, might not use
//	phone_missed_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/phone_missed_200dp_D2D2D2.svg"); // TODO unused so far, might not use
//	phone_missed_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/phone_missed_200dp_DE3131.svg"); // TODO unused so far
	volume_off_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/volume_off_200dp_474747.svg");
	volume_off_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/volume_off_200dp_D2D2D2.svg");
	volume_up_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/volume_up_200dp_474747.svg");
	volume_up_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/volume_up_200dp_D2D2D2.svg");

	pthread_rwlock_rdlock(&mutex_global_variable); // 🟧
	const char *tor_location_local = tor_location;
	pthread_rwlock_unlock(&mutex_global_variable); // 🟩
	if(tor_location_local) // shouldn't need mutex
		ui_show_auth_screen();
	else // missing a required binary
		ui_show_missing_binaries();

	ui_decorate_headerbar(); // XXX commenting this line disables custom headerbar

	if(start_maximized) // can be before gtk_window_present
		ui_toggle_maximize();
	if(!start_daemonized)
	{
		gtk_window_present(GTK_WINDOW(t_main.main_window));
		char array[2048]; // arbitrary size
		snprintf(array,sizeof(array),"%sTorX Library Version: %u.%u.%u.%u\n",CLIENT_VERSION,torx_library_version[0],torx_library_version[1],torx_library_version[2],torx_library_version[3]);
		ui_notify(text_welcome,array);
	}
	if(start_minimized) // must be AFTER gtk_window_present
		ui_minimize();

	t_main.tor_log_buffer = gtk_text_buffer_new(NULL);
	t_main.torx_log_buffer = gtk_text_buffer_new(NULL);
	t_main.textbuffer_torrc = gtk_text_buffer_new(NULL);

	const uint8_t lockout_local = threadsafe_read_uint8(&mutex_global_variable,&lockout);
	if(tor_location && no_password && !lockout_local) // shouldn't need mutex // UI setting, relevant to first_run only because otherwise login_start is called by sql_populate_setting
		login_start("");

	if(lockout_local)
		gtk_button_set_label(GTK_BUTTON(t_main.auth_button),text_wait);

	#ifdef ENABLE_APPINDICATOR
	if(binary_path)
	{
		const uint16_t icon_port = randport(0);
		char port_array[6];
		snprintf(port_array,sizeof(port_array),"%u",icon_port);
	/*
	// The following CANNOT be used because run_binary hangs due to limitations.
	// If we want to use the following, we have to read tray_fd_stdout for FAILURE_STRING, but that could be subject to race conditions
		char appindicator_path[PATH_MAX];
		char small_p[3];
		char large_p[3];
		snprintf(appindicator_path,sizeof(appindicator_path),"torx-tray");
		snprintf(small_p,sizeof(small_p),"-p");
		snprintf(large_p,sizeof(large_p),"-P");
		char* const args_cmd[] = {appindicator_path,small_p,port_array,large_p,binary_path,NULL};
		#ifdef WIN32
		run_binary(&tray_pid,NULL,tray_fd_stdout,args_cmd,NULL);
		#else
		run_binary(&tray_pid,NULL,&tray_fd_stdout,args_cmd,NULL);
		#endif
		if(tray_pid < 0)
		{
			char binary_path_copy[PATH_MAX];
			snprintf(binary_path_copy,sizeof(binary_path_copy),"%s",binary_path);
			char *current_binary_directory = dirname(binary_path_copy); // NECESSARY TO COPY
			snprintf(appindicator_path,sizeof(appindicator_path),"%s%c%s%ctorx-tray",starting_dir,platform_slash,current_binary_directory,platform_slash);
			#ifdef WIN32
			run_binary(&tray_pid,NULL,tray_fd_stdout,args_cmd,NULL);
			#else
			run_binary(&tray_pid,NULL,&tray_fd_stdout,args_cmd,NULL);
			#endif
			if(tray_pid < 0 && )
			{ // try for GDB
				snprintf(appindicator_path,sizeof(appindicator_path),"%s%ctorx-tray",current_binary_directory,platform_slash);
				#ifdef WIN32
				run_binary(&tray_pid,NULL,tray_fd_stdout,args_cmd,NULL);
				#else
				run_binary(&tray_pid,NULL,&tray_fd_stdout,args_cmd,NULL);
				#endif
				if(tray_pid < 0 && )
					error_printf(0,"Failed to start appindicator on port %s\n",port_array);
			}
		}
	 */
		char appindicator_path[PATH_MAX];
		#ifdef WIN32
		snprintf(appindicator_path,sizeof(appindicator_path),"torx-tray.exe -p %s -P %s",port_array,binary_path);
		STARTUPINFO siStartInfo;
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION process_info;
		if(!CreateProcess(NULL,appindicator_path,NULL,NULL,TRUE,0,NULL,NULL,&siStartInfo,&process_info))
		{ // after checking PATH, assuming this isn't running in GDB
			char binary_path_copy[PATH_MAX];
			snprintf(binary_path_copy,sizeof(binary_path_copy),"%s",binary_path);
			char *current_binary_directory = dirname(binary_path_copy); // NECESSARY TO COPY
			snprintf(appindicator_path,sizeof(appindicator_path),"%s\\%s\\torx-tray.exe -p %s -P %s",starting_dir,current_binary_directory,port_array,binary_path);
			if(!CreateProcess(NULL,appindicator_path,NULL,NULL,TRUE,0,NULL,NULL,&siStartInfo,&process_info))
			{ // try for GDB
				snprintf(appindicator_path,sizeof(appindicator_path),"%s\\torx-tray.exe -p %s -P %s",current_binary_directory,port_array,binary_path);
				if(!CreateProcess(NULL,appindicator_path,NULL,NULL,TRUE,0,NULL,NULL,&siStartInfo,&process_info))
					error_printf(0,"Failed to start appindicator on port %s",port_array);
			}
		}
		#else
		snprintf(appindicator_path,sizeof(appindicator_path),"torx-tray");
		if((tray_pid = fork()) == -1)
			error_simple(-1,"fork");
		if(tray_pid == 0)
		{ // Check in path before attempting to check from directory we run from
			if(execlp("torx-tray","torx-tray","-p",port_array,"-P",binary_path,NULL))
			{ // after checking PATH, assuming this isn't running in GDB
				char binary_path_copy[PATH_MAX];
				snprintf(binary_path_copy,sizeof(binary_path_copy),"%s",binary_path);
				char *current_binary_directory = dirname(binary_path_copy); // NECESSARY TO COPY
				snprintf(appindicator_path,sizeof(appindicator_path),"%s%c%s%ctorx-tray",starting_dir,platform_slash,current_binary_directory,platform_slash);
				if(execl(appindicator_path,"torx-tray","-p",port_array,"-P",binary_path,NULL))
				{ // try for GDB
					snprintf(appindicator_path,sizeof(appindicator_path),"%s%ctorx-tray",current_binary_directory,platform_slash);
					if(execl(appindicator_path,"torx-tray","-p",port_array,"-P",binary_path,NULL))
						fprintf(stderr,"Failed to start appindicator on port %s\n",port_array);
				}
			}
			exit(0);
		}
		#endif
		if(pthread_create(&thread_icon_communicator,&ATTR_DETACHED,&icon_communicator,itovp(icon_port)))
			error_simple(0,"Failed to create thread");
		#endif
	}
}

static gboolean option_handler(const gchar* option_name,const gchar* value,gpointer data,GError** error)
{
	(void) data;
	(void) error;
	const size_t len = strlen(option_name);
	if(!strncmp(option_name,"-V",len) || !strncmp(option_name,"--version",len))
	{ // Use printf here, not error_printf
		char array[2048]; // arbitrary size
		snprintf(array,sizeof(array),"%sTorX Library Version: %u.%u.%u.%u\n",CLIENT_VERSION,torx_library_version[0],torx_library_version[1],torx_library_version[2],torx_library_version[3]);
		printf("%s",array);
		exit(0);
	}
	else if(!strncmp(option_name,"-n",len) || !strncmp(option_name,"--no-password",len))
		no_password = 1;
	else if(!strncmp(option_name,"-m",len) || !strncmp(option_name,"--minimized",len))
		start_minimized = 1;
	else if(!strncmp(option_name,"-f",len) || !strncmp(option_name,"--fullscreen",len))
		start_maximized = 1;
	else if(!strncmp(option_name,"-d",len) || !strncmp(option_name,"--daemonize",len))
		start_daemonized = 1;
	else if(!strncmp(option_name,"-v",len) || !strncmp(option_name,"--verbose",len))
	{
		if(!value)
			debug = 1;
		else
			debug = (int8_t)strtoll(value, NULL, 10);
	}
	return true; // tells GTK we processed it successfully
}

int main(int argc,char **argv)
{ // XXX WARNING: Do not use error_* before initial or SEVERE memory errors will occur that are VERY hard to diagnose XXX
	argv_0 = argv[0];

	gtk_application_gtk4 = gtk_application_new (DBUS_TITLE, G_APPLICATION_DEFAULT_FLAGS);
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	GOptionEntry entries[] = {
		{ "version", 'V', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, option_handler, "Print library version",NULL},
		{ "no-password", 'n', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, option_handler, "No password",NULL},
		{ "minimized", 'm', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, option_handler, "Start minimized",NULL},
		{ "fullscreen", 'f', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, option_handler, "Start fullscreen",NULL},
		{ "daemonize", 'd', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, option_handler, "Start daemonized",NULL},
		{ "verbose", 'v', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, option_handler, "Set debug level","level"},
		{0} };
	#pragma GCC diagnostic pop
	g_application_add_main_option_entries(G_APPLICATION(gtk_application_gtk4),entries);

	g_signal_connect(gtk_application_gtk4, "activate", G_CALLBACK (ui_activate), NULL); // DO NOT FREE arg because this only gets passed ONCE.
	const int status = g_application_run (G_APPLICATION (gtk_application_gtk4), argc, argv);

	g_object_unref (gtk_application_gtk4); // was g_application_gtk4
//	waitpid(tor_pid); // Might stop our issues on close, if issues are caused by late closing of tor fork
	return status;
}
