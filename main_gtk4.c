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
//#include "other/scalable/apps/logo_torx.h" // XXX Fun alternative to GResource (its a .svg in b64 defined as a macro). but TODO DO NOT USE IT, use g_resources_lookup_data instead to get gbytes

#define ALPHA_VERSION 1 // enables debug print to stderr
#define CLIENT_VERSION "TorX-GTK4 Alpha 2.0.12 2024/07/29 by SymbioticFemale\n© Copyright 2024 SymbioticFemale.\nAttribution-NonCommercial-NoDerivatives 4.0 International (CC BY-NC-ND 4.0)\n"
#define DBUS_TITLE "org.torx.gtk4" // GTK Hardcoded Icon location: /usr/share/icons/hicolor/48x48/apps/org.gnome.TorX.png
#define DARK_THEME 0
#define LIGHT_THEME 1
#define THEME_DEFAULT DARK_THEME
#define RESOURCE_CSS_DARK "/org/torx/gtk4/theme/dark_chat.css"
#define RESOURCE_CSS_LIGHT "/org/torx/gtk4/theme/light_chat.css"
#define MESSAGES_START_AT_TOP_OF_WINDOW 1 // 0 seems a bit buggy
#define INVERSION_TEST 1 // https://gitlab.gnome.org/GNOME/gtk/-/issues/4680 NOTE: using g_list_store_insert / g_list_store_splice at position 0 will cause a rebuild of the entire list
#define GTK_FACTORY_BUG 1 // Memory intensive. Enabling this is kind of necessary until https://gitlab.gnome.org/GNOME/gtk/-/merge_requests/7420 is widely merged.
#define QR_IS_PNG 1
/* XXX NOTE: Bundled files are the two below, the .desktop, and /usr/share/icons/hicolor/48x48/apps/org.gnome.TorX.png XXX */
#define FILENAME_ICON "icon.svg" // .png/.svg
#define FILENAME_BEEP "beep.wav" // .wav files only, on linux and windows
#define MAX_STICKER_COLUMNS 5
#define MAX_STICKER_ROWS 6
#define ENABLE_SPINNERS 0 // this should only be enabled if people have a GPU.
#define LONG_PRESS_TIME 0.5 // 0.5 to 2.0 seconds, default is 1.0.

#define ICON_COMMUNICATION_ATTEMPTS 3 // currently must be 1+
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
static char *binary_path = NULL; // current binary's full path
static char *binary_name = NULL; // current binary's full path
static uint8_t running = 0;
static int global_n = -1; // Always CTRL or GROUP_CTRL. Currently open chat window. Avoid using where possible. (except notifications perhaps)
static int treeview_n = -1;
static int global_theme = -1;
static int log_unread = 1;
static int vertical_mode = 0; // see ui_determine_orientation()
static int8_t force_sign = 0; // TODO Should be 0. Global value for testing only. This should be per-peer. can add timestamp to message (as a salt to prevent relay attacks in groups), Tor project does this with all signed messages.
static uint8_t save_all_stickers = 0; // TODO handle saving manually
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
/* Global Structs */ // NOTE: Access must be in UI thread (_idle) or all usage must have mutex/rwlock

static pthread_t thread_icon_communicator = {0};

const char *supported_image_formats[] = {".jpg",".jpeg",".png",".gif",".bmp",".svg"}; // case insensitive

#define MAX_PEERS 4096 // TODO this isnt ideal because library has no such limitation. this is just laziness.
#define STICKERS_LIST_SIZE 500

#define ENUM_STATUS_GROUP_CTRL 4

static struct sticker_list {
	GdkPaintable *paintable_animated; // Check before printing
	GdkPaintable *paintable_static; // DO NOT FREE upon delete UNTIL shutdown. // TODO display except when mouseover, in ui_sticker_chooser
	unsigned char checksum[CHECKSUM_BIN_LEN];
	int peers[MAX_PEERS]; // must initialize as -1. This is a list of peers we sent this sticker to. Only these peers can request data, to prevent fingerprinting by sticker list.
	unsigned char *data; // Only exists for unsaved stickers
	size_t data_len; // Only exists for unsaved stickers
} sticker [STICKERS_LIST_SIZE] = {0};

static struct t_peer_list { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	size_t unread; // number of new unread messages (currently since startup only, otherwise this needs to be in peer not t_peer
	int8_t mute; // 0 no, 1 yes
	int pm_n;
	int edit_n;
	int edit_i;
	int pointer_location;
	GtkTextBuffer* buffer_write;
	struct t_message_list { // XXX DO NOT DELETE XXX
		guint pos;
		uint8_t visible;
		#if GTK_FACTORY_BUG
		GtkWidget *message_box;
		#endif
	} *t_message; // TODO do not initialize automatically, but upon use
	struct t_file_list {
		GtkWidget *progress_bar;
		GtkWidget *file_size;
		int n; // The peer iter that progress_bar is in, for rebuilds upon completion
		int i; // The message iter that progress_bar is in, for rebuilds upon completion
	} *t_file;
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

static struct { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX // Putting these global values in a struct lets us easily intialize as NULL
	GListStore *treestore_incoming;
	GListStore *treestore_outgoing;
	GListStore *treestore_mult;
	GListStore *treestore_sing;

	GListStore *list_store_chat;

	guint global_pos;
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
	GtkWidget *button_activity; // editing, private messaging, etc
	GtkWidget *write_message; // TODO can probably be eliminated with some work
	GtkWidget *button_send;
	GtkWidget *popover_message; // needs to be global to avoid being made permanently parentless when children of outer message box get deleted in print_message(,,3)
	GtkWidget *popover_sticker;

	/* Individual Group Chat Pages */
	GtkWidget *popover_invite; // TODO can probably be eliminated with lots of work
	GtkWidget *popover_group_peerlist;
	GtkWidget *popover_block;

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
	ENUM_BUTTON_ADD_BLOCK_MENU,
	ENUM_BUTTON_ADD_BLOCK,
	ENUM_BUTTON_KILL,
	ENUM_BUTTON_DELETE_CTRL,
	ENUM_BUTTON_DELETE_LOG,
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
	ENUM_SPIN_MULT_EXPIRATION
};

enum ui_protocols {
//	ENUM_PROTOCOL_TEST_STREAM = 3313,		// Just for testing stream. Works fine.
	ENUM_PROTOCOL_STICKER_HASH = 29812,		// Sticker will be sending a sticker hash. If peer doesn't have the sticker, request.
	ENUM_PROTOCOL_STICKER_HASH_PRIVATE = 40505,
	ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED = 1891,
	ENUM_PROTOCOL_STICKER_REQUEST = 24931,		// hash
	ENUM_PROTOCOL_STICKER_DATA_GIF = 46093		// hash + data
	// TODO vibration... but do it only in flutter as a UI specific protocol
};

void initialize_n_cb_ui(const int n);
void initialize_i_cb_ui(const int n,const int i);
void initialize_f_cb_ui(const int n,const int f);
void initialize_g_cb_ui(const int g);
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
void login_cb_ui(const int value);

/* Global Text Declarations for ui_initialize_language() */
static const char *text_title = {0};
static const char *text_welcome = {0};
static const char *text_transfer_completed_to = {0};
static const char *text_transfer_completed_from = {0};
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
static const char *text_draft = {0}; // TODO implement
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
static const char *text_button_send = {0};
static const char *text_done = {0};
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
static const char *text_set_download_directory = {0};
static const char *text_set_tor = {0};
static const char *text_set_cpu = {0};
static const char *text_set_suffix = {0};
static const char *text_set_validity_sing = {0};
static const char *text_set_validity_mult = {0};
static const char *text_set_auto_mult = {0};
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

const char **options[] = { &text_peer, &text_group, &text_block, &text_incoming, &text_outgoing, &text_active_mult, &text_active_sing }; // do not modify without thinking

static void ui_input_new(GtkWidget *entry);
static void ui_input_bad(GtkWidget *entry);
static void ui_select_changed(const void *data);
static void ui_decorate_panel_left(const int n);
static void ui_theme(const int theme_local);
static void ui_show_generate(void);
static void ui_show_home(void);
static void ui_show_settings(void);
static int ui_populate_peers(void *arg);
GtkWidget *ui_add_chat_node(const int n,void (*callback_click)(const void *),const int minimal_size)__attribute__((warn_unused_result));
GtkWidget *ui_button_generate(const int type,const int n)__attribute__((warn_unused_result));
static inline uint8_t is_image_file(const char *filename)__attribute__((warn_unused_result));
static void ui_print_message(const int n,const int i,const int scroll);

static GtkApplication	*gtk_application_gtk4;

static GdkTexture *texture_logo;
static GdkTexture *texture_headerbar_button1;
static GdkTexture *texture_headerbar_button2;
static GdkTexture *texture_headerbar_button3;
static GdkTexture *texture_headerbar_button_leave1;
static GdkTexture *texture_headerbar_button_leave2;
static GdkTexture *texture_headerbar_button_leave3;
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
static GdkTexture *file_file;
static GdkTexture *file_txt;
static GdkTexture *file_music;
static GdkTexture *file_image;
static GdkTexture *file_zip;
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
static GdkTexture *emoji_dark;
static GdkTexture *emoji_light;
//static GdkTexture *send_inactive;
static GdkTexture *attach_dark;
static GdkTexture *attach_light;

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
//	size_t heading_len;
//	size_t message_len;
};

struct file_nf { // XXX Do not sodium_malloc structs unless they contain sensitive arrays XXX
	int n;
	int f;
//	int target_n;
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

static int initialize_n_idle(void *arg)
{
	const int n = vptoi(arg);
	t_peer[n].unread = 0;
	t_peer[n].mute = 0;
	t_peer[n].pm_n = -1;
	t_peer[n].edit_n = -1;
	t_peer[n].edit_i = INT_MIN;
	t_peer[n].buffer_write = NULL;
	t_peer[n].pointer_location = -10;
	t_peer[n].t_message = (struct t_message_list *)torx_insecure_malloc(sizeof(struct t_message_list) *21) - t_peer[n].pointer_location; // XXX Note this shift
	t_peer[n].t_file = torx_insecure_malloc(sizeof(struct t_file_list) *11);
	return 0;
}

void initialize_n_cb_ui(const int n)
{
	g_idle_add(initialize_n_idle,itovp(n));
}

static int initialize_i_idle(void *arg)
{ // XXX NOTICE: These also need to be carefully reset following message_deleted_cb (currently they are)
	struct int_int *int_int = (struct int_int*) arg; // Casting passed struct
	const int n = int_int->n; // XXX DO NOT DELETE XXX
	const int i = int_int->i;
	torx_free((void*)&int_int);
	t_peer[n].t_message[i].pos = 0;
	t_peer[n].t_message[i].visible = 0;
	#if GTK_FACTORY_BUG
	t_peer[n].t_message[i].message_box = NULL;
	#endif
	return 0;
}

void initialize_i_cb_ui(const int n,const int i)
{
	struct int_int *int_int = torx_insecure_malloc(sizeof(struct int_int));
	int_int->n = n;
	int_int->i = i;
	g_idle_add(initialize_i_idle,int_int);
}

static int initialize_f_idle(void *arg)
{
	struct file_nf *file_nf = (struct file_nf*) arg; // Casting passed struct
	const int n = file_nf->n;
	const int f = file_nf->f;
	torx_free((void*)&file_nf);

	t_peer[n].t_file[f].n = -1;
	t_peer[n].t_file[f].i = INT_MIN;
	t_peer[n].t_file[f].progress_bar = NULL;
	t_peer[n].t_file[f].file_size = NULL;
	return 0;
}

void initialize_f_cb_ui(const int n,const int f)
{
	struct file_nf *file_nf = torx_insecure_malloc(sizeof(struct file_nf));
	file_nf->n = n;
	file_nf->f = f;
	g_idle_add(initialize_f_idle,file_nf);
}

void initialize_g_cb_ui(const int g)
{
	(void) g;
}

static int expand_file_struc_idle(void *arg)
{
	struct file_nf *file_nf = (struct file_nf*) arg; // Casting passed struct
	const int n = file_nf->n;
//	const int f = file_nf->f;
	torx_free((void*)&file_nf);
	const size_t current_allocation_size = torx_allocation_len(t_peer[n].t_file);
	t_peer[n].t_file = torx_realloc(t_peer[n].t_file,current_allocation_size + sizeof(struct t_file_list) *10);
	return 0;
}

void expand_file_struc_cb_ui(const int n,const int f)
{
	struct file_nf *file_nf = torx_insecure_malloc(sizeof(struct file_nf));
	file_nf->n = n;
	file_nf->f = f;
	g_idle_add(expand_file_struc_idle,file_nf);
}

static int expand_message_struc_idle(void *arg)
{ // XXX DO NOT DELETE XXX
	struct int_int *int_int = (struct int_int*) arg; // Casting passed struct
	const int n = int_int->n;
	const int i = int_int->i;
	torx_free((void*)&int_int);
	const size_t current_allocation_size = torx_allocation_len(t_peer[n].t_message + t_peer[n].pointer_location);
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
	struct int_int *int_int = torx_insecure_malloc(sizeof(struct int_int));
	int_int->n = n; // XXX DO NOT DELETE XXX
	int_int->i = i;
	g_idle_add(expand_message_struc_idle,int_int);
}

static int expand_peer_struc_idle(void *arg)
{
	(void) arg;
	const size_t current_allocation_size = torx_allocation_len(t_peer);
	t_peer = torx_realloc(t_peer,current_allocation_size + sizeof(struct t_peer_list) *10);
	return 0;
}

void expand_peer_struc_cb_ui(const int n)
{
	g_idle_add(expand_peer_struc_idle,itovp(n));
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

static inline int display_notification_idle(void *arg) // g_idle_add() functions must return int
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
//	sodium_memzero(notification->heading,notification->heading_len);
//	sodium_memzero(notification->message,notification->message_len);
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
	g_idle_add(display_notification_idle,notification);
}

static int ui_populate_list(void *arg) // g_idle_add() functions must return int
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
	if(list_store && G_IS_LIST_STORE(list_store))
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
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_PEER)
	{
		g = set_g(n,NULL);
		group_n = getter_group_int(g,offsetof(struct group_list,n));
	}
	if((global_n != n && (g == -1 || global_n != group_n)) || !t_peer[n].t_file[f].progress_bar || !GTK_IS_WIDGET(t_peer[n].t_file[f].progress_bar))
		return 0; // should check if visible?
	const uint64_t size = getter_uint64(n,INT_MIN,f,-1,offsetof(struct file_list,size));
	char *file_path = getter_string(NULL,n,INT_MIN,f,offsetof(struct file_list,file_path));
	if(transferred == size)
	{ // Notify of completion
		const uint8_t status = getter_uint8(n,INT_MIN,f,-1,offsetof(struct file_list,status));
		char peernick[56+1];
		getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
		char heading[256] = {0}; // zero'd
		if(status == ENUM_FILE_OUTBOUND_COMPLETED)
			snprintf(heading,sizeof(heading),"%s %s",text_transfer_completed_to,peernick);
		else if(status == ENUM_FILE_INBOUND_COMPLETED)
			snprintf(heading,sizeof(heading),"%s %s",text_transfer_completed_from,peernick);
		else
		{ // probably something full of zeroes. this should never occur. Its a bug.
			sodium_memzero(peernick,sizeof(peernick));
			error_printf(0,"Unhandled file status in transfer_progress_cb: %u transferred: %lu [f].size %lu",status,transferred,size);
			torx_free((void*)&file_path);
			return 0;
		}
		sodium_memzero(peernick,sizeof(peernick));
		ui_notify(heading,file_path);
		sodium_memzero(heading,sizeof(heading));
	}
	char *file_size_text = file_progress_string(n,f);
	gtk_label_set_text(GTK_LABEL(t_peer[n].t_file[f].file_size),file_size_text);
	torx_free((void*)&file_size_text);
	double fraction = 0;
	if(size > 0)
		fraction = ((double)(getter_uint64(n,INT_MIN,f,-1,offsetof(struct file_list,last_transferred)))*1.000/(double)size);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(t_peer[n].t_file[f].progress_bar),fraction);
	const uint8_t status = getter_uint8(n,INT_MIN,f,-1,offsetof(struct file_list,status));
	if(status == ENUM_FILE_INBOUND_ACCEPTED || status == ENUM_FILE_INBOUND_COMPLETED || status == ENUM_FILE_OUTBOUND_ACCEPTED || status == ENUM_FILE_OUTBOUND_COMPLETED)
		gtk_widget_set_visible(t_peer[n].t_file[f].progress_bar,TRUE);
	if(status == ENUM_FILE_INBOUND_COMPLETED && is_image_file(file_path))
		ui_print_message(t_peer[n].t_file[f].n,t_peer[n].t_file[f].i,3); // rebuild message to display image
	torx_free((void*)&file_path);
	return 0;
}

void transfer_progress_cb_ui(const int n,const int f,const uint64_t transferred)
{ // GUI Callback. // This is called every packet on a file transfer. Packets are PACKET_LEN-10 in size, so 488 (as of 2022/08/19, may be changed to accomodate sequencing)
	struct progress *progress = torx_insecure_malloc(sizeof(struct progress));
	progress->n = n;
	progress->f = f;
	progress->transferred = transferred;
	g_idle_add(transfer_progress_idle,progress);
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

static void gtk_box_remove_all(GtkWidget *box)
{
	if(!box)
		return;
	for(GtkWidget *child = gtk_widget_get_first_child(box) ; child != NULL ; child = gtk_widget_get_first_child(box))
		gtk_box_remove(GTK_BOX(box), child);
}

static void ui_set_image_lock(const int n)
{
	if(n != global_n)
		return;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		gtk_image_set_from_paintable(GTK_IMAGE(t_main.image_header),GDK_PAINTABLE(texture_logo));
		const char *identifier;
		char onion[56+1];
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		{
			identifier = text_torxid;
			getter_array(&onion,52+1,n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
		}
		else
		{
			identifier = text_onionid;
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
		}
		const int g = set_g(n,NULL);
		pthread_rwlock_rdlock(&mutex_expand_group);
		char *groupid = b64_encode(group[g].id,GROUP_ID_SIZE);
		pthread_rwlock_unlock(&mutex_expand_group);
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
			const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,sendfd_connected));
			const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,recvfd_connected));
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
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL); // just looking up existing
		const uint32_t g_peercount = getter_group_uint32(g,offsetof(struct group_list,peercount));
		char peer_count[256];
		snprintf(peer_count,sizeof(peer_count),"%s: %d %s %u",text_status_online,group_online(g),text_of,g_peercount);
		gtk_label_set_text(GTK_LABEL(t_main.last_online),peer_count);
		return;
	}
	const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,sendfd_connected));
	const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,recvfd_connected));
	if(sendfd_connected > 0 && recvfd_connected > 0)
	{
		gtk_label_set_text(GTK_LABEL(t_main.last_online),text_status_online);
		return;
	}
	char last_online_text[256];
	const time_t last_seen = getter_time(n,INT_MIN,-1,-1,offsetof(struct peer_list,last_seen));
	if(last_seen > 0)
	{
		struct tm *info = localtime(&last_seen);
		char timebuffer[20] = {0};
		strftime(timebuffer,20,"%Y/%m/%d %T",info);
		snprintf(last_online_text,sizeof(last_online_text),"%s%s",text_status_last_seen,timebuffer);
	}
	else
		snprintf(last_online_text,sizeof(last_online_text),"%s%s",text_status_last_seen,text_status_never);
	gtk_label_set_text(GTK_LABEL(t_main.last_online),last_online_text);
}

void change_password_cb_ui(const int value)
{ // GUI Callb	char *heading = "redrawing";//peer [n]. peernick;
	g_idle_add(change_password_idle,itovp(value));
}

static int incoming_friend_request_idle(void *arg)
{
	totalIncoming++;
	// TODO rebuild ui_decorate_panel_left_top();
	ui_populate_list(arg);
	return 0;
}

void incoming_friend_request_cb_ui(const int n)
{ // GUI Callback
	char peernick[56+1];
	getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
	ui_notify(peernick,text_new_friend);
	sodium_memzero(peernick,sizeof(peernick));
	g_idle_add(incoming_friend_request_idle,itovp(ENUM_OWNER_CTRL));
}

static int onion_deleted_idle(void *arg)
{
	struct int_int *int_int = (struct int_int*) arg; // Casting passed struct
	const int n = int_int->n;
	const int owner = int_int->i;
	torx_free((void*)&arg);
	t_peer[n].unread = 0;
	t_peer[n].mute = 0;
	t_peer[n].pm_n = -1;
	t_peer[n].edit_n = -1;
	t_peer[n].edit_i = INT_MIN;
	if(generated_n > -1 && getter_uint8(generated_n,INT_MIN,-1,-1,offsetof(struct peer_list,owner)) == 0)
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
	return 0;
}

void onion_deleted_cb_ui(const uint8_t owner,const int n)
{ // GUI Callback, // array necessary because owner about to be zero'd
	struct int_int *int_int = torx_insecure_malloc(sizeof(struct int_int));
	int_int->n = n;
	int_int->i = owner;
	error_printf(0,"Checkpoint onion_deleted_cb owner: %d\n",owner);
	g_idle_add(onion_deleted_idle,int_int);
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
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
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
		const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,sendfd_connected));
		const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,recvfd_connected));
		if((owner != ENUM_OWNER_GROUP_PEER || t_peer[group_n].mute == 0) && t_peer[n].mute == 0 && sendfd_connected > 0 && recvfd_connected > 0)
		{
			char peernick[56+1];
			getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
			ui_notify(peernick,text_online);
			sodium_memzero(peernick,sizeof(peernick));
		}
	}
	return 0;
}

void peer_online_cb_ui(const int n)
{ // GUI Callback from socks.c (successful send_init)
	g_idle_add(peer_online_idle,itovp(n));
}

static int peer_offline_idle(void *arg)
{ // currently a duplicate of peer_online_idle and the contents of this exists in a 3rd place
	return peer_online_idle(arg);
}

void peer_offline_cb_ui(const int n)
{ // GUI Callback from socks.c (successful send_init)
	g_idle_add(peer_offline_idle,itovp(n));
}

static int peer_new_idle(void *arg)
{ // N is passed but we currently don't use it
	const int n = vptoi(arg);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
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
	g_idle_add(peer_new_idle,itovp(n));
}

void peer_loaded_cb_ui(const int n)
{ // NOTE: this runs frequently on startup
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
		g_idle_add(ui_populate_peers,itovp(ENUM_STATUS_GROUP_CTRL));
	else
	{
		const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
		g_idle_add(ui_populate_peers,itovp(status));
	}
}

static void ui_copy_qr(const void *arg)
{ // UTF8 only, no PNG
	if(t_main.popover_qr && GTK_IS_WIDGET(t_main.popover_qr))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_qr)); // TODO throws GTK error if not existing. should check first
	const int n = vptoi(arg);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	struct qr_data *qr_data;
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL); // just looking up existing
		unsigned char id[GROUP_ID_SIZE];
		pthread_rwlock_rdlock(&mutex_expand_group);
		memcpy(id,group[g].id,sizeof(id));
		pthread_rwlock_unlock(&mutex_expand_group);
		char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
		sodium_memzero(id,sizeof(id));
		qr_data = qr_bool(group_id_encoded,1);
		torx_free((void*)&group_id_encoded);
	}
	else
	{
		char torxid[52+1];
		getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
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
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_GROUP_CTRL)
		{
			const int g = set_g(n,NULL); // just looking up existing
			unsigned char id[GROUP_ID_SIZE];
			pthread_rwlock_rdlock(&mutex_expand_group);
			memcpy(id,group[g].id,sizeof(id));
			pthread_rwlock_unlock(&mutex_expand_group);
			char *group_id_encoded = b64_encode(id,GROUP_ID_SIZE);
			sodium_memzero(id,sizeof(id));
			qr_data = qr_bool(group_id_encoded,8);
			torx_free((void*)&group_id_encoded);
		}
		else
		{
			char torxid[52+1];
			getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
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
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(t_main.window == window_main)
	{
		gtk_entry_buffer_delete_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_generate_peernick)),0,-1); // Clear peernick
		char torxid[52+1];
		getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,torxid,-1);
		else
		{
			char onion[56+1];
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
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
	g_idle_add(onion_ready_idle,itovp(n));
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
{ // Returns >-1 unless bug, even if empty. Be sure to check return for if(s > -1 && sticker[s].paintable_animated (!/=)= NULL)
	if(!checksum)
	{
		error_simple(0,"Null passed to set_sticker. Coding error. Report this.");
		breakpoint();
		return -1;
	}
	int s = 0;
	while(sticker[s].paintable_static && memcmp(sticker[s].checksum,checksum,CHECKSUM_BIN_LEN) && s < STICKERS_LIST_SIZE)
		s++;
	if(s == STICKERS_LIST_SIZE)
	{
		error_simple(0,"Hit STICKERS_LIST_SIZE, cannot add more stickers.");
		breakpoint();
		return -1;
	}
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
}

static int ui_sticker_register(const unsigned char *data,const size_t data_len)
{ // If utilizing, be sure to check return for if(s > -1 && sticker[s].paintable_animated != NULL)
	unsigned char checksum[CHECKSUM_BIN_LEN];
	if(!data || !data_len || b3sum_bin(checksum,NULL,data,0,data_len) != data_len)
		return -1; // bug
	const int s = ui_sticker_set(checksum);
	if(s > -1 && sticker[s].paintable_animated == NULL)
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

static void ui_on_choose_file(GtkFileDialog *dialog,GAsyncResult *res,GtkWidget *button)
{ // For selecting a file for reasons other than file sharing
	GFile *file = gtk_file_dialog_open_finish (dialog,res,NULL);
	if(file)
	{
		if(!strncmp(gtk_widget_get_name(button),"tor_location",12))
		{ // TODO consider having a option to unset / reset default path
			char *path = g_file_get_path(file); // free'd
			if(!path)
				return;
			gtk_button_set_label ( GTK_BUTTON(button),path);
			const size_t len = strlen(path);
			pthread_rwlock_wrlock(&mutex_global_variable);
			torx_free((void*)&tor_location);
			tor_location = torx_secure_malloc(len+1);
			snprintf(tor_location,len+1,"%s",path);
			pthread_rwlock_unlock(&mutex_global_variable);
			sql_setting(0,-1,"tor_location",path,strlen(path));
			// TODO free path somehow? gtk allocated
			start_tor();
			g_free(path); path = NULL;
		}
		else if(!strncmp(gtk_widget_get_name(button),"custom_input_location",21))
		{
			t_main.custom_input_location = g_file_get_path(file); // TODO g_free
			char *path = g_file_get_path(file); // free'd
			char *privkey = custom_input_file(path);
			gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(t_main.entry_custom_privkey)),privkey,-1);
			torx_free((void*)&privkey);
			g_free(path); path = NULL;
		}
		else if(!strncmp(gtk_widget_get_name(button),"button_add_sticker",18))
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
		pthread_rwlock_wrlock(&mutex_global_variable);
		torx_free((void*)&download_dir);
		download_dir = allocation;
		pthread_rwlock_unlock(&mutex_global_variable);
		sql_setting(0,-1,"download_dir",folder_path,len);
		g_free(folder_path); folder_path = NULL;
	}
	else
	{ // Unset if cancelled
		pthread_rwlock_wrlock(&mutex_global_variable);
		torx_free((void*)&download_dir);
		pthread_rwlock_unlock(&mutex_global_variable);
		sql_delete_setting(0,-1,"download_dir");
	}
	pthread_rwlock_rdlock(&mutex_global_variable);
	gtk_button_set_label (GTK_BUTTON(data),download_dir);
	pthread_rwlock_unlock(&mutex_global_variable);
}

static void ui_on_choose_folder(GtkFileDialog *dialog,GAsyncResult *res,gpointer data)
{ // For selecting a folder for saving file
	if(data)
	{ // Choosing folder path
		struct file_nf *file_nf = (struct file_nf*) data;
		const int n = file_nf->n;
		const int f = file_nf->f;
		torx_free((void*)&data);
		GFile *folder = gtk_file_dialog_select_folder_finish(dialog,res,NULL);
		if(folder)
		{
			torx_read(n) // XXX
			const char *filename = peer[n].file[f].filename;
			torx_unlock(n) // XXX
			if(filename == NULL)
			{
				error_simple(0,"Null file name. Failed sanity check.");
				gtk_window_destroy(GTK_WINDOW(dialog));
				return;
			}
			char *folder_path = g_file_get_path(folder); // free'd (in most cases)
			if(folder_path == NULL || !write_test(folder_path)) // null has happened, stupid GTK
				return;
			torx_write(n) // XXX
			torx_free((void*)&peer[n].file[f].file_path);
			const size_t maxlen = strlen(folder_path) + strlen(peer[n].file[f].filename) + 2;
			peer[n].file[f].file_path = torx_secure_malloc(maxlen);
			snprintf(peer[n].file[f].file_path,maxlen,"%s%c%s",folder_path,platform_slash,peer[n].file[f].filename);
			torx_unlock(n) // XXX
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

}

static void ui_sticker_send(gpointer *arg)
{
	const int s = vptoi(arg);
	if(s < 0 || sticker[s].paintable_animated == NULL)
		return;
	int g = -1;
	uint8_t g_invite_required = 0;
	const uint8_t owner = getter_uint8(global_n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
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
	if(!GDK_IS_PIXBUF_ANIMATION(PIXBUF_PAINTABLE(paintable)->animation))
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
	(void)n;
	t_main.popover_sticker = gtk_popover_new();
	gtk_widget_set_parent (t_main.popover_sticker,parent);
	gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_sticker),TRUE);
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
			GtkWidget *outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
			gtk_widget_set_margin_top(inner_box, size_spacing_three);
			gtk_widget_set_margin_bottom(inner_box,size_spacing_three);
			gtk_widget_set_margin_start(inner_box,size_spacing_three);
			gtk_widget_set_margin_end(inner_box,size_spacing_three);
			if(sticker[s].data)
				gtk_widget_add_css_class(outer_box,"unsaved_sticker");
			gtk_box_append (GTK_BOX(outer_box), inner_box);

			GtkWidget *popover = gtk_popover_new();
			GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
			gtk_widget_add_css_class(box, "popover_inner");
			if(sticker[s].data)
				create_button(text_save,ui_sticker_save,itovp(s))
			create_button(text_delete,ui_sticker_delete,itovp(s))
			gtk_popover_set_child(GTK_POPOVER(popover),box);
			gtk_popover_set_autohide(GTK_POPOVER(popover),TRUE);
			gtk_widget_set_parent(popover,outer_box);
			g_signal_connect(popover, "closed", G_CALLBACK (gtk_widget_unparent), popover); // XXX necessary or a warning occurs

			GtkGesture *long_press = gtk_gesture_long_press_new(); // NOTE: "cancelled" == single click, "pressed" == long press
			gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(long_press),LONG_PRESS_TIME);
			g_signal_connect_swapped(long_press, "cancelled", G_CALLBACK(ui_sticker_send),itovp(s)); // DO NOT FREE arg because this only gets passed ONCE.
			g_signal_connect_swapped(long_press, "pressed", G_CALLBACK(gtk_popover_popup),GTK_POPOVER(popover)); // DO NOT FREE arg because this only gets passed ONCE.

			gtk_widget_add_controller(outer_box, GTK_EVENT_CONTROLLER(long_press));

			gtk_grid_attach (GTK_GRID(grid),outer_box,x,y,1,1);
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
	g_signal_connect(t_main.popover_sticker, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_sticker); // XXX necessary or a warning occurs
	gtk_popover_popup(GTK_POPOVER(t_main.popover_sticker));
}

static void ui_emoji_picked(const GtkEmojiChooser *chooser,const char *text,GtkText *self)
{
	(void) chooser;
	(void) self;
	if(!gtk_text_buffer_insert_interactive_at_cursor (gtk_text_view_get_buffer (GTK_TEXT_VIEW(t_main.write_message)), text, -1,TRUE))
		gtk_widget_error_bell (GTK_WIDGET (t_main.write_message));
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

static void ui_toggle_file(GtkGestureLongPress* self,gpointer data)
{ // Toggle File transfer (accept, pause/cancel)
	(void) self;
	if(!data)
		return;
	const int n = vptoii_n(data);
	const int f = vptoii_i(data);
	torx_read(n) // XXX
	const uint8_t status = peer[n].file[f].status;
	const uint64_t size = peer[n].file[f].size;
	const uint64_t transferred = calculate_transferred(n,f);
	torx_unlock(n) // XXX
	char *file_path = getter_string(NULL,n,INT_MIN,f,offsetof(struct file_list,file_path));
	printf("Checkpoint toggle_file: %u\n",status);
	if(size > 0 && size == transferred && size == get_file_size(file_path))
	{ // NOTE: we have gtk_file_launcher_open_containing_folder in two places
		GFile *file = g_file_new_for_path(file_path);
		GtkFileLauncher *launcher = gtk_file_launcher_new (file);
	//	gtk_file_launcher_set_always_ask (launcher,TRUE); // doesn't do shit
	//	gtk_file_launcher_launch (launcher,GTK_WINDOW(t_main.main_window),NULL,NULL,NULL); // not good, it opens in default without asking
		gtk_file_launcher_open_containing_folder (launcher,GTK_WINDOW(t_main.main_window),NULL,NULL,NULL);
		torx_free((void*)&file_path);
		return;
	}
	if(status == ENUM_FILE_INBOUND_PENDING)
	{
		pthread_rwlock_rdlock(&mutex_global_variable);
		const char *download_dir_local = download_dir;
		pthread_rwlock_unlock(&mutex_global_variable);
		if(file_path == NULL && download_dir_local == NULL)
		{ // this should only be for received files that are not started, otherwise should accept/pause/cancel via file_accept
			GtkFileDialog *dialog = gtk_file_dialog_new();
			gtk_file_dialog_set_modal(dialog,TRUE); // block interaction with UI
			gtk_file_dialog_set_title(dialog,text_choose_folder); // also: gtk_file_dialog_set_filters
			struct file_nf *file_nf = torx_insecure_malloc(sizeof(struct file_nf));
			file_nf->n = n;
			file_nf->f = f;
			gtk_file_dialog_select_folder (dialog,GTK_WINDOW(t_main.main_window),NULL,(GAsyncReadyCallback)ui_on_choose_folder,file_nf);
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
	const int8_t log_messages = getter_int8(n,INT_MIN,-1,-1,offsetof(struct peer_list,log_messages));
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
	const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
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
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
		if (owner == ENUM_OWNER_GROUP_CTRL)
			totalUnreadGroup -= t_peer[n].unread;
		else
			totalUnreadPeer -= t_peer[n].unread;
      		t_peer[n].unread = 0; // reset unread count for specific peer
	}
	if(n > -1 && t_peer[n].buffer_write != NULL)
	{ // Draft exists
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_GROUP_CTRL)
			ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
		else
		{
			const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
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
	const int peer_index = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,peer_index));
	int8_t log_messages = getter_int8(n,INT_MIN,-1,-1,offsetof(struct peer_list,log_messages));
	// Update Setting
	if(log_messages == -1 || log_messages == 0)
	{
		log_messages++;
		setter(n,INT_MIN,-1,-1,offsetof(struct peer_list,log_messages),&log_messages,sizeof(log_messages));
	}
	else if(log_messages == 1)
	{
		log_messages = -1;
		setter(n,INT_MIN,-1,-1,offsetof(struct peer_list,log_messages),&log_messages,sizeof(log_messages));
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
	const int peer_index = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,peer_index));
	// Update Setting
	if(t_peer[n].mute == 0)
		t_peer[n].mute = 1;
	else
		t_peer[n].mute = 0;
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_group_peerlist),TRUE); // WORKS!!! gtk_popover_set_cascade_popdown / gtk_popover_popdown do NOT work
	// Save Setting
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",t_peer[n].mute);
	sql_setting(0,peer_index,"mute",p1,strlen(p1));
}

static void ui_toggle_mute_button(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data);// DO NOT FREE ARG
	ui_toggle_mute(data);
	ui_set_image_mute(button,n);
}

static void ui_toggle_block(const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	block_peer(n);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_group_peerlist),TRUE); // WORKS!!! gtk_popover_set_cascade_popdown / gtk_popover_popdown do NOT work
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
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner != ENUM_OWNER_GROUP_PEER)
		ui_set_image_block(button,n);
}

static void ui_toggle_kill(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	if(n != -1) // Not global kill
		gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(kill_active)));
	kill_code(n,NULL);
	if(t_main.popover_block && GTK_IS_WIDGET(t_main.popover_block))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_block));
//	error("Need to add additional logic here to make this image properly togglable"); // no, currently can't unkill. Kill is kill.
}

static void ui_toggle_delete(GtkWidget *button,const gpointer data)
{ // should not need to ui_populate_peers() because onion_deleted_cb should run
	const int n = vptoi(data); // DO NOT FREE ARG
	const int peer_index = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,peer_index));
	gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_active)));
	takedown_onion(peer_index,1);
	if(t_main.popover_block && GTK_IS_WIDGET(t_main.popover_block))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_block));
	ui_go_back(data); // DO NOT FREE arg because this only gets passed ONCE.
}

static void ui_delete_log(GtkWidget *button,const gpointer data)
{
	const int n = vptoi(data); // DO NOT FREE ARG
	gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_active)));
	delete_log(n);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
		ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
	else
	{
		const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
		ui_populate_peers(itovp(status));
	}
	// TODO re-draw scroll window?
}


static void ui_load_more_messages(const GtkScrolledWindow *scrolled_window,const GtkPositionType pos,const gpointer data)
{ // Do not delete this function.
	(void) scrolled_window;
	const int n = vptoi(data); // DO NOT FREE ARG
	if((INVERSION_TEST && pos == GTK_POS_TOP) || (!INVERSION_TEST && pos == GTK_POS_BOTTOM)) // GTK_POS_BOTTOM
		return;
	const int peer_index = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,peer_index));
	const uint32_t local_show_log_messages = threadsafe_read_uint32(&mutex_global_variable,&show_log_messages);
	const int loaded = sql_populate_message(peer_index,0,local_show_log_messages);
	if(loaded)
	{ // Need to re-sort messages
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
		if(owner == ENUM_OWNER_GROUP_CTRL)
		{
			const int g = set_g(n,NULL);
			message_sort(g);
		}
	}
	fprintf(stderr,"Checkpoint load_more_messages() \"unlimited scroll\" of %d of requested %u\n",loaded,local_show_log_messages);
}

/* Other */

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
		for(int peer_index,n = 0 ; (peer_index = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,peer_index))) > -1 || getter_byte(n,INT_MIN,-1,-1,offsetof(struct peer_list,onion)) != 0 ; n++)
		{ // storing last_seen time to .key file. NOTE: this will execute more frequently than we might want if logging is disabled. however there is little we can do.
			if(peer_index < 0)
				continue;
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
			if(owner == ENUM_OWNER_CTRL || owner == ENUM_OWNER_GROUP_CTRL)
			{ // for private messages, will need to be more complicated than just adding GROUP_PEER here
				snprintf(p1,sizeof(p1),"%lu",t_peer[n].unread);
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
	g_idle_add(cleanup_idle,itovp(sig_num));
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

//	struct mouseover *mouseover = torx_insecure_malloc(sizeof(mouseover));
//	mouseover->on = t_main.headerbar_buttons_box_enter;
//	mouseover->off = t_main.headerbar_buttons_box_leave;

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
	snprintf(p1,sizeof(p1),"%lu",stack_change->iter);
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
		snprintf(p1,sizeof(p1),"%lu",iter);
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
			gtk_widget_set_size_request(overlay2,(int)(size_peerlist_icon_size/1.5),(int)(size_peerlist_icon_size/1.5));
			gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
			gtk_widget_set_valign(overlay2, GTK_ALIGN_END);
			GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
			gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

			char unread_count[21];
			snprintf(unread_count,sizeof(unread_count),"%lu",overlay_count);
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
		gtk_widget_set_size_request(overlay2,(int)(size_peerlist_icon_size/1.5),(int)(size_peerlist_icon_size/1.5));
		gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
		gtk_widget_set_valign(overlay2, GTK_ALIGN_START);
		GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
		gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

		char incoming_count[21];
		snprintf(incoming_count,sizeof(incoming_count),"%lu",totalIncoming);
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
	t_main.chat_headerbar_left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
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
	t_main.chat_headerbar_right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
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
		t_main.popover_torrc_errors = gtk_popover_new();
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_torrc_errors),TRUE);
		gtk_widget_set_parent (t_main.popover_torrc_errors,button);
		GtkWidget *label = gtk_label_new(torrc_errors);
		gtk_widget_add_css_class(label, "text");

		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_five);
		gtk_widget_add_css_class(box, "popover_inner");
		GtkWidget *button_override = gtk_button_new_with_label (text_override);
		gtk_box_append (GTK_BOX (box), label);
		gtk_box_append (GTK_BOX (box), button_override);
		g_signal_connect(button_override, "clicked", G_CALLBACK (ui_override_save_torrc), torrc_content_local); // XXX NOTE: this gets freed which is OK because only called once

		gtk_popover_set_child(GTK_POPOVER(t_main.popover_torrc_errors),box);
		g_signal_connect(t_main.popover_torrc_errors, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_torrc_errors); // XXX necessary or a warning occurs
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
	pthread_rwlock_rdlock(&mutex_global_variable);
	gtk_text_buffer_set_text(t_main.textbuffer_torrc,torrc_content,-1);
	pthread_rwlock_unlock(&mutex_global_variable);
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

static void ui_initialize_language(GtkWidget *combobox)
{
	if(combobox != NULL) // from settings page
	{
		if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 0)
			snprintf(language,sizeof(language),"en_US");
		sql_setting(1,-1,"language",language,strlen(language));
	}
	if(language[0] == '\0' || !strncmp(language,"en_US",5))
	{
		text_title = "TorX";
		text_welcome = "Welcome to TorX";
		text_transfer_completed_to = "Transfer completed to";
		text_transfer_completed_from = "Transfer completed from";
		text_online = "Has come online";
		text_new_friend = "Has a new friend request";
		text_accepted_file = "Accepted a file";
		text_spoiled = "A single-use onion was spoiled";
		text_placeholder_privkey = "Onion Private Key (base64, 88 characters including trailing ==, or select from file)";
		text_placeholder_identifier = "Peer Nick or Mult Identifier (56 character max)";
		text_placeholder_add_identifier = "Peer Nick (56 characters max)";
		text_placeholder_add_onion = "Peer TorX-ID or OnionID (provided by peer)";
		text_placeholder_add_group_identifier = "Group Nick (56 characters max)";
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
		text_sticker = "Sticker";
		text_current_members = "Current Members: ";
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
		text_you = "You: ";
		text_queued = "Queued: ";
		text_draft = "Draft: ";
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
		text_button_send = "Send";
		text_done = "Done";
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
		text_status_last_seen = "Last seen: ";
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
		text_set_download_directory = "Select Download Directory";
		text_set_tor = "Select Custom Tor binary location (effective immediately)";
		text_set_cpu = "Maximum CPU threads for TorX-ID generation";
		text_set_suffix = "Minimum Suffix Length for TorX-ID generation";
		text_set_validity_sing = "Single-Use TorX-ID expiration time (days)";
		text_set_validity_mult = "Multiple-Use TorX-ID expiration time (days)";
		text_set_auto_mult = "Automatically Accept Incoming Mult Requests";
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

static void ui_change_daemonize(const gpointer combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&minimize_to_tray,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"minimize_to_tray",p1,strlen(p1));
}

/*static void ui_change_full_duplex(const gpointer combobox)
{ // Setting
	int8_t local_setting = 0;
	if(gtk_drop_down_get_selected(GTK_DROP_DOWN(combobox)) == 1)
		local_setting = 1;
	threadsafe_write(&mutex_global_variable,&full_duplex_requests,&local_setting,sizeof(local_setting));
	char p1[21];
	snprintf(p1,sizeof(p1),"%d",local_setting);
	sql_setting(0,-1,"full_duplex_requests",p1,strlen(p1));
}*/

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
	pthread_rwlock_rdlock(&mutex_global_variable);
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
	else
	{
		pthread_rwlock_unlock(&mutex_global_variable);
		return; // unknown spinner, coding error
	}
	pthread_rwlock_unlock(&mutex_global_variable);
	if((int)(value_current = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton))) != value_original)
	{
		char p1[21];
		pthread_rwlock_wrlock(&mutex_global_variable);
		if(spin == ENUM_SPIN_DEBUG)
		{
			pthread_rwlock_unlock(&mutex_global_variable);
			torx_debug_level((int8_t)value_current); // Does not save to file because safety
		}
		else if(spin == ENUM_SPIN_CPU_THREADS)
		{
			global_threads = (uint32_t)value_current;
			snprintf(p1,sizeof(p1),"%u",global_threads);
			pthread_rwlock_unlock(&mutex_global_variable);
			sql_setting(0,-1,"global_threads",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_SUFFIX_LENGTH)
		{
			suffix_length = (uint8_t)value_current;
			snprintf(p1,sizeof(p1),"%u",suffix_length);
			pthread_rwlock_unlock(&mutex_global_variable);
			sql_setting(0,-1,"suffix_length",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_SING_EXPIRATION)
		{
			sing_expiration_days = (uint32_t)value_current;
			snprintf(p1,sizeof(p1),"%u",sing_expiration_days);
			pthread_rwlock_unlock(&mutex_global_variable);
			sql_setting(0,-1,"sing_expiration_days",p1,strlen(p1));
		}
		else if(spin == ENUM_SPIN_MULT_EXPIRATION)
		{
			mult_expiration_days = (uint32_t)value_current;
			snprintf(p1,sizeof(p1),"%u",mult_expiration_days);
			pthread_rwlock_unlock(&mutex_global_variable);
			sql_setting(0,-1,"mult_expiration_days",p1,strlen(p1));
		}
		else
			pthread_rwlock_unlock(&mutex_global_variable); // unknown spinner, coding error
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
	getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
	if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		gtk_text_buffer_set_text(t_main.textbuffer_custom_onion,torxid,-1);
	else
	{
		char onion[56+1];
		getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
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
	GtkWidget *outer_box = {0};
	if(vertical_mode)
		outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	else
		outer_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
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
	GtkWidget *outer_box = {0};
	if(vertical_mode)
		outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	else
		outer_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
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
	GtkWidget *box_language;
	if(vertical_mode)
		box_language = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	else
		box_language = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label_language = gtk_label_new(text_set_select_language);
	gtk_widget_set_halign(label_language, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label_language, TRUE);	// works, do not remove

	GtkWidget *inner_box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box, GTK_ALIGN_END);

	const char* strings1[] = {"English", NULL};
	GtkWidget *combo_box_language = gtk_drop_down_new_from_strings(strings1);
	gtk_box_append (GTK_BOX (inner_box), combo_box_language);
	if(language[0] == '\0' || !strncmp(language,"en_US",5))
		gtk_drop_down_set_selected(GTK_DROP_DOWN(combo_box_language),0);
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

	// Downloads Folder
	GtkWidget *box0;
	if(vertical_mode)
		box0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	else
		box0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label0 = gtk_label_new(text_set_download_directory);
	gtk_widget_set_halign(label0, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label0, TRUE);	// works, do not remove
	pthread_rwlock_rdlock(&mutex_global_variable);
	GtkWidget *button_download_dir = gtk_button_new_with_label (download_dir);
	pthread_rwlock_unlock(&mutex_global_variable);
	GtkWidget *inner_box1  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box1, GTK_ALIGN_END);
	gtk_box_append (GTK_BOX (inner_box1), button_download_dir);
	g_signal_connect(button_download_dir, "clicked", G_CALLBACK(ui_choose_folder), NULL);
	gtk_box_append (GTK_BOX (box0), label0);
	gtk_box_append (GTK_BOX (box0 ), inner_box1);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box0);

	// Tor binary location
	GtkWidget *box1;
	if(vertical_mode)
		box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	else
		box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	GtkWidget *label1 = gtk_label_new(text_set_tor);
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_widget_set_hexpand(label1, TRUE);	// works, do not remove
	pthread_rwlock_rdlock(&mutex_global_variable);
	GtkWidget *button_tor_location = gtk_button_new_with_label (tor_location);
	GtkWidget *inner_box2  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_widget_set_halign(inner_box2, GTK_ALIGN_END);
	gtk_box_append (GTK_BOX (inner_box2), button_tor_location);
	pthread_rwlock_unlock(&mutex_global_variable);
	gtk_widget_set_name(button_tor_location,"tor_location");
	g_signal_connect(button_tor_location, "clicked", G_CALLBACK(ui_choose_file), itovp(0));
	gtk_box_append (GTK_BOX (box1), label1);
	gtk_box_append (GTK_BOX (box1 ), inner_box2);
	gtk_box_append (GTK_BOX (t_main.scroll_box_right), box1);

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

	// Custom Input
	GtkWidget *box8;
	if(vertical_mode)
		box8 = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_ten);
	else
		box8 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);
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
			g_idle_add_full(301,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
		torx_free((void*)&data);
	}
/*	if(t_main.window == window_log_tor)
		return 1; // continue
	else
		return 0; // stop*/
	return 0;
}

void tor_log_cb_ui(char *message)
{
	if(!message)
		return;
	g_idle_add(tor_log_idle,message); // frees pointer*
}

static void beep(void)
{ // This function currently does not need to run in the UI thread, which is why it is not ui_prefixed // Probably bad idea but portable beeps: We can use GTKVideo to play a bundled .mp3 as a GResource. Company recommends .mp3 or .mkv/.webm with opus. we'd have to make it invisible.
	#ifdef WIN32
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
			g_idle_add_full(301,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
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
	g_idle_add(error_idle,error_message); // frees pointer*
}

void fatal_cb_ui(char *error_message)
{
	ui_notify(text_fatal_error,error_message);
	fprintf(stderr,"%s: %s\n",text_fatal_error,error_message);
	g_idle_add(error_idle,error_message); // frees pointer*
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
	getter_array(&byte,1,n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
	if(byte != '\0')
		treeview_n = n;
	else
		treeview_n = -1; // should not occur. could remove this check and else, for brevity
//	fprintf(stderr,"Checkpoint n_from_treeview: %d %d\nOnion in table: %s\nOnion in struct: %s\n",n,treeview_n,onion,peer[n].onion);
}

static void ui_rename(GtkCellEditable *self,GParamSpec *pspec,gpointer data)
{ // Make iter editable, and have a signal for after it gets changed
	(void) pspec;
	const int n = vptoi(data);
	if(!gtk_editable_label_get_editing(GTK_EDITABLE_LABEL(self)))
	{
		char freshpeernick[56+1]; // zero'd
		char *p = gtk_editable_get_chars(GTK_EDITABLE(self),0,56);
		if(p && strlen(p) && p[0] != ' ')
		{
			snprintf(freshpeernick,sizeof(freshpeernick),"%s",p);
			sodium_memzero(p,strlen(p));
			g_free(p);
			p = NULL;
			change_nick(n,freshpeernick);
			sodium_memzero(freshpeernick,sizeof(freshpeernick));
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
			ui_populate_list(itovp(owner));
		}
	}
}

static void ui_accept(void)
{
	if(treeview_n < 0)
		return;
	totalIncoming--;
	// TODO rebuild ui_decorate_panel_left_top();
	peer_accept(treeview_n);
	ui_populate_list(itovp(ENUM_OWNER_CTRL));
	ui_populate_peers(itovp(ENUM_STATUS_FRIEND));
}

static void ui_delete(void)
{
	if(treeview_n < 0)
		return;
	const uint8_t owner = getter_uint8(treeview_n,INT_MIN,-1,-1,offsetof(struct peer_list,owner)); // in case of delete
	const int peer_index = getter_int(treeview_n,INT_MIN,-1,-1,offsetof(struct peer_list,peer_index));
	if(owner == ENUM_OWNER_CTRL)
	{
		totalIncoming--;
	// TODO rebuild ui_decorate_panel_left_top();
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
		return;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL);
		const uint8_t g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
		if(!g_invite_required)
		{ // Public gropu, copy group_id
			unsigned char id[GROUP_ID_SIZE];
			pthread_rwlock_rdlock(&mutex_expand_group);
			memcpy(id,group[g].id,sizeof(id));
			pthread_rwlock_unlock(&mutex_expand_group);
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
			getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
			gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),torxid);
			sodium_memzero(torxid,sizeof(torxid));
		}
		else
		{
			char onion[56+1];
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
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
	getter_array(&torxid,sizeof(torxid),treeview_n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));

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

	t_main.popover_qr = gtk_popover_new();
	gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_qr),TRUE);
	gtk_widget_set_parent (t_main.popover_qr,t_main.button_show_qr);
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
	g_signal_connect(t_main.popover_qr, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_qr); // XXX necessary or a warning occurs
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
//	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	char byte = '\0';
	getter_array(&byte,1,n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
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
{
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
			char peernick[56+1];
			getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
			GtkWidget *label = gtk_editable_label_new(peernick);
			g_signal_connect(label, "notify::editing", G_CALLBACK(ui_rename),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
			sodium_memzero(peernick,sizeof(peernick));
			gtk_box_append(GTK_BOX(box), label);
		}
		if(column_number == 1)
		{
			char onion[56+1];
			if(list == ENUM_OWNER_PEER)
				getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,peeronion));
			else
				getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
			GtkWidget *label = gtk_label_new(onion);
			sodium_memzero(onion,sizeof(onion));
			gtk_box_append(GTK_BOX(box), label);
		}
		else if(column_number == 2)
		{
			char torxid[52+1];
			getter_array(&torxid,sizeof(torxid),n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
			GtkWidget *label = gtk_label_new(torxid);
			sodium_memzero(torxid,sizeof(torxid));
			gtk_box_append(GTK_BOX(box), label);
		}
		else if(column_number == 3)
		{
			const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
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

	GtkWidget *button_kill_global;
	if(vertical_mode)
		button_kill_global = gtk_button_new_with_label (text_vertical_emit_global_kill);
	else
		button_kill_global = gtk_button_new_with_label (text_emit_global_kill);
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
	GtkWidget *custom_switcher;
	if(vertical_mode)
		custom_switcher = gtk_custom_switcher_new(GTK_STACK (stack),GTK_ORIENTATION_VERTICAL,1);
		//gtk_orientable_set_orientation(GTK_ORIENTABLE(stack_switcher),GTK_ORIENTATION_VERTICAL);
	else
		custom_switcher = gtk_custom_switcher_new(GTK_STACK (stack),GTK_ORIENTATION_HORIZONTAL,1);
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
			pthread_rwlock_rdlock(&mutex_expand_group);
			memcpy(id,group[g].id,sizeof(id));
			pthread_rwlock_unlock(&mutex_expand_group);
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
		getter_array(&torxid,sizeof(torxid),generated_n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
			gtk_text_buffer_set_text(t_main.textbuffer_generated_onion,torxid,-1);
		else
		{
			char onion[56+1];
			getter_array(&onion,sizeof(onion),generated_n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
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
	{
		memcpy(language,setting_value,setting_value_len);
		language[setting_value_len] = '\0';
	}
	else if(!strncmp(setting_name,"gtk4-width",10))
		size_window_default_width = (int)strtoll(setting_value, NULL, 10);
	else if(!strncmp(setting_name,"gtk4-height",11))
		size_window_default_height = (int)strtoll(setting_value, NULL, 10);
	else if(!strncmp(setting_name,"minimize_to_tray",20))
		minimize_to_tray = (uint8_t)strtoull(setting_value, NULL, 10);
	else if(plaintext == 0 && !strncmp(setting_name,"mute",4))
		t_peer[n].mute = (int8_t)strtoll(setting_value, NULL, 10);
	else if(plaintext == 0 && !strncmp(setting_name,"unread",6))
	{
		if(log_unread != 1)
			return 0; // ignoring (potentially old)
		t_peer[n].unread = strtoull(setting_value, NULL, 10);
		if(t_peer[n].unread > 0)
		{
			const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
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
	g_idle_add(custom_setting_idle,custom_setting);
}

static void ui_message_copy(const gpointer data)
{
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	const int p_iter = getter_int(n,i,-1,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	pthread_rwlock_unlock(&mutex_protocols);
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
/*	const int p_iter = getter_int(n,i,-1,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint16_t protocol = protocols[p_iter].protocol;
	pthread_rwlock_unlock(&mutex_protocols);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	printf("Checkpoint ui_message_resend owner==%d\n",owner);
	int target_n = n;
	if(owner == ENUM_OWNER_GROUP_PEER)
	{ // should always be true unless this is != RECV
		const int g = set_g(n,NULL);
		target_n = getter_group_int(g,offsetof(struct group_list,n));
	} */
	message_resend(n,i);
}

static int handle_stuff(const int n,const int i)
{
	torx_read(n)
	const int p_iter = peer[n].message[i].p_iter;
	torx_unlock(n)
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint8_t group_msg = protocols[p_iter].group_msg;
	pthread_rwlock_unlock(&mutex_protocols);
	int nnn = n;
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
	{
		const int g = set_g(n,NULL);
		nnn = getter_group_int(g,offsetof(struct group_list,n));
	}
	return nnn;
}

static void ui_message_pause(const gpointer data)
{ // file_accept alternates between accept and pause
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	const int nnn = handle_stuff(n,i);
	const int f = set_f_from_i(n,i);
	file_accept(nnn,f);
}
static void ui_message_cancel(const gpointer data)
{ // file_cancel() NOTE: this is for cancelling FILES not unsent messages.
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	const int nnn = handle_stuff(n,i);
	const int f = set_f_from_i(n,i);
	file_cancel(nnn,f);
}
static void ui_message_reject(const gpointer data)
{ // file_cancel() TODO NOTE: this should delete the related messages too.
	ui_message_cancel(data);
}

static void ui_open_folder(const gpointer data)
{ // NOTE: we have gtk_file_launcher_open_containing_folder in two places
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	const int nnn = handle_stuff(n,i);
	const int f = set_f_from_i(n,i);
	char *file_path = getter_string(NULL,nnn,INT_MIN,f,offsetof(struct file_list,file_path));
	if(file_path == NULL)
	{
		error_simple(0,"File path is NULL. Cannot open folder.");
		torx_free((void*)&file_path);
		return;
	}
	GFile *file = g_file_new_for_path(file_path);
	GtkFileLauncher *launcher = gtk_file_launcher_new (file);
	gtk_file_launcher_open_containing_folder (launcher,GTK_WINDOW(t_main.main_window),NULL,NULL,NULL);
	torx_free((void*)&file_path);
}

static void ui_activity_cancel(const gpointer data)
{ // Cancel active activity
	(void) data;
	if(t_peer[global_n].edit_n == -1 && t_peer[global_n].edit_i == INT_MIN && t_peer[global_n].pm_n == -1) // make it safe to call from select_changed
		return;
//	if(t_main.write_message == NULL || t_main.button_activity == NULL)
//		return;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	gtk_text_buffer_set_text(buffer,"",0);
	if(t_peer[global_n].edit_n > -1 || t_peer[global_n].edit_i > INT_MIN)
	{ // Editing was active (of message or a GROUP_PEER peernick)
		t_peer[global_n].edit_n = -1;
		t_peer[global_n].edit_i = INT_MIN;
		gtk_button_set_label(GTK_BUTTON(t_main.button_send),text_button_send); // redundant, doing later in all cases
		if(t_peer[global_n].pm_n > -1)
		{ // PM was active before edit, restore it
			char peernick[56+1];
			getter_array(&peernick,sizeof(peernick),t_peer[global_n].pm_n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
			char cancel_message[256];
			snprintf(cancel_message,sizeof(cancel_message),"%s %s",text_private_messaging,peernick);
			sodium_memzero(peernick,sizeof(peernick));
			gtk_button_set_label(GTK_BUTTON(t_main.button_activity),cancel_message);
			sodium_memzero(cancel_message,sizeof(cancel_message));
		}
	}
	else if(t_peer[global_n].pm_n > -1) // DO NOT MAKE ELSE. PM was active and editing was not
		t_peer[global_n].pm_n = -1;
	if(t_peer[global_n].edit_n == -1 && t_peer[global_n].edit_i == INT_MIN && t_peer[global_n].pm_n == -1) // DO NOT MAKE ELSE.	
	{
		gtk_widget_set_visible(t_main.button_activity,FALSE);
		if(!INVERSION_TEST)
			g_idle_add_full(301,scroll_func_idle,t_main.scrolled_window_right,NULL);
	}
}

static void ui_activity_rename(const gpointer data)
{ // Rename a GROUP_PEER via change_nick(n,nick);
	t_peer[global_n].edit_n = vptoi(data);
	gtk_button_set_label(GTK_BUTTON(t_main.button_activity),text_cancel_editing);
	g_signal_connect(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),NULL);
	gtk_widget_set_visible(t_main.button_activity,TRUE);
	gtk_button_set_label(GTK_BUTTON(t_main.button_send),text_done);
	if(t_main.popover_message && GTK_IS_WIDGET(t_main.popover_message))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_message));
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_group_peerlist),TRUE); // WORKS!!! gtk_popover_set_cascade_popdown / gtk_popover_popdown do NOT work
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	char peernick[56+1];
	getter_array(&peernick,sizeof(peernick),t_peer[global_n].edit_n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
	gtk_text_buffer_set_text(buffer,peernick,(int)strlen(peernick));
	sodium_memzero(peernick,sizeof(peernick));
}

static void ui_establish_pm(const int n,void *popover)
{
	if(t_peer[global_n].edit_n > -1 && t_peer[global_n].edit_i > INT_MIN)
	{
		gtk_button_set_label(GTK_BUTTON(t_main.button_send),text_button_send); // cancel any ongoing edits first, without cleaning buffer
		t_peer[global_n].edit_n = -1;
		t_peer[global_n].edit_i = INT_MIN;
	}
	t_peer[global_n].pm_n = n;
	char peernick[56+1];
	getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
	char cancel_message[256];
	snprintf(cancel_message,sizeof(cancel_message),"%s %s",text_private_messaging,peernick);
	sodium_memzero(peernick,sizeof(peernick));
	gtk_button_set_label(GTK_BUTTON(t_main.button_activity),cancel_message);
	sodium_memzero(cancel_message,sizeof(cancel_message));
	g_signal_connect(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),NULL);
	gtk_widget_set_visible(t_main.button_activity,TRUE);
	if(t_main.popover_group_peerlist && GTK_IS_WIDGET(t_main.popover_group_peerlist))
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_group_peerlist),TRUE); // WORKS!!! gtk_popover_set_cascade_popdown / gtk_popover_popdown do NOT work
	if(popover)
		gtk_popover_popdown(GTK_POPOVER(popover));
}

static void ui_private_message(const void *data)
{ // start PM from group peerlist
	const int n = vptoi(data);
	ui_establish_pm(n,t_main.popover_group_peerlist);
}

static void ui_activity_pm(const gpointer data)
{ // start PM from message popover
	const int n = vptoii_n(data);
//	const int i = vptoii_i(data);
	ui_establish_pm(n,t_main.popover_message);
}

static void ui_activity_edit(const gpointer data)
{
	t_peer[global_n].edit_n = vptoii_n(data);
	t_peer[global_n].edit_i = vptoii_i(data);
	gtk_button_set_label(GTK_BUTTON(t_main.button_activity),text_cancel_editing);
	g_signal_connect(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),NULL);
	gtk_widget_set_visible(t_main.button_activity,TRUE);
	gtk_button_set_label(GTK_BUTTON(t_main.button_send),text_done);
	if(t_main.popover_message && GTK_IS_WIDGET(t_main.popover_message))
		gtk_popover_popdown(GTK_POPOVER(t_main.popover_message));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
	const int p_iter = getter_int(t_peer[global_n].edit_n,t_peer[global_n].edit_i,-1,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	pthread_rwlock_unlock(&mutex_protocols);
	if(null_terminated_len == 1)
	{
		char *message = getter_string(NULL,t_peer[global_n].edit_n,t_peer[global_n].edit_i,-1,offsetof(struct message_list,message));
		gtk_text_buffer_set_text(buffer,message,(int)strlen(message)); // strlen is to avoid null pointer and otherwise reading beyond utf8
		torx_free((void*)&message);
	}
	else
		error_simple(0,"Attempting to edit a message type that hasn't yet been configured for editing");
}

static void ui_message_delete(const gpointer data)
{ // Should work for public, private, etc, messages.
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	message_edit(n,i,NULL);
}

static void ui_message_long_press(GtkGestureLongPress* self,gdouble x,gdouble y,const gpointer data)
{
	(void) x;
	(void) y;
	const int n = vptoii_n(data);
	const int i = vptoii_i(data);
	int f = -1;
	const int p_iter = getter_int(n,i,-1,-1,offsetof(struct message_list,p_iter));
	if(p_iter < 0)
	{
		error_simple(0,"Message's p_iter is <0 which indicates it is deleted or buggy.");
		breakpoint();
		return; // message is deleted or buggy
	}
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint16_t protocol = protocols[p_iter].protocol;
	const uint8_t file_checksum = protocols[p_iter].file_checksum;
	const uint8_t group_msg = protocols[p_iter].group_msg;
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	const uint32_t date_len = protocols[p_iter].date_len;
	const uint32_t signature_len = protocols[p_iter].signature_len;
	pthread_rwlock_unlock(&mutex_protocols);
	char *message = getter_string(NULL,n,i,-1,offsetof(struct message_list,message));
	if(file_checksum == 1)
		f = set_f(n,(const unsigned char*)message,CHECKSUM_BIN_LEN);
	t_main.popover_message = gtk_popover_new();
	gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_message),TRUE);
	GtkWidget *parent = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(self));
	gtk_widget_set_parent(t_main.popover_message,parent);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box, "popover_inner");
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_message),box);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	const uint8_t stat = getter_uint8(n,i,-1,-1,offsetof(struct message_list,stat));
	if(f > -1)
	{ // files:	start/pause,reject/cancel	text_start / text_pause, text_reject (in) / text_cancel (out) // TODO rebuild int_int struct and pass f along with n,i
		int nnn = n;
		if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
		{
			const int g = set_g(n,NULL);
			nnn = getter_group_int(g,offsetof(struct group_list,n));
			f = set_f(nnn,(const unsigned char*)message,CHECKSUM_BIN_LEN);
		}
		const uint8_t status = getter_uint8(nnn,INT_MIN,f,-1,offsetof(struct file_list,status));
		torx_read(nnn) // XXX
		const char *file_path = peer[nnn].file[f].file_path;
		torx_unlock(nnn) // XXX
		if(file_path)
		{
			if(/*status == ENUM_FILE_OUTBOUND_PENDING || */status == ENUM_FILE_INBOUND_PENDING)
				create_button(text_start,ui_message_pause,data)
			else if(status == ENUM_FILE_OUTBOUND_ACCEPTED || status == ENUM_FILE_INBOUND_ACCEPTED)
				create_button(text_pause,ui_message_pause,data)
		}
		if(status == ENUM_FILE_INBOUND_COMPLETED)
			create_button(text_open_folder,ui_open_folder,data)
		if(stat == ENUM_MESSAGE_RECV)
		{
			if(status != ENUM_FILE_INBOUND_REJECTED)
				create_button(text_reject,ui_message_reject,data)
		}
		else
		{
			if(status != ENUM_FILE_OUTBOUND_CANCELLED)
				create_button(text_cancel,ui_message_cancel,data)
		}
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
			if(s > -1 && sticker[s].data && sticker[s].data_len)
				create_button(text_save,ui_sticker_save,itovp(s))
		}
		if(null_terminated_len == 1 && (signature_len == 0 || stat != ENUM_MESSAGE_RECV))
			create_button(text_edit,ui_activity_edit,data)
		if(owner == ENUM_OWNER_GROUP_PEER)
			create_button(text_private_messaging,ui_activity_pm,data)
		if(owner == ENUM_OWNER_GROUP_PEER)
			create_button(text_rename,ui_activity_rename,itovp(n))
	}
	torx_free((void*)&message);
	create_button(text_delete,ui_message_delete,data)
	g_signal_connect(t_main.popover_message, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_message); // XXX necessary or a warning occurs
	gtk_popover_popup(GTK_POPOVER(t_main.popover_message));
}

static GtkWidget *ui_message_info(const int n,const int i)
{ // Build the message header or footer
	GtkWidget *info_message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_ten);


	char *timebuffer = message_time_string(n,i);
	GtkWidget *message_time = gtk_label_new(timebuffer);
	gtk_widget_add_css_class(message_time, "message_info_time");
	torx_free((void*)&timebuffer);

	char peernick[56+1];
	getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
	GtkWidget *peernick_widget = gtk_label_new(peernick);
	sodium_memzero(peernick,sizeof(peernick));
	gtk_widget_add_css_class(peernick_widget, "message_info_peernick");

	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	const uint8_t stat = getter_uint8(n,i,-1,-1,offsetof(struct message_list,stat));
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
	const int p_iter = getter_int(n,i,-1,-1,offsetof(struct message_list,p_iter));
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint16_t protocol = protocols[p_iter].protocol;
	const uint8_t group_pm = protocols[p_iter].group_pm;
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
	const uint8_t group_msg = protocols[p_iter].group_msg;
	pthread_rwlock_unlock(&mutex_protocols);
	torx_read(n) // XXX
	const uint8_t owner = peer[n].owner;
	const uint8_t stat = peer[n].message[i].stat;
	torx_unlock(n) // XXX
	uint32_t message_len;
	char *message = getter_string(&message_len,n,i,-1,offsetof(struct message_list,message));
	int nn = n; // XXX IMPORTANT: usage of n when relating to specific message, nn when relating to group settings or global_n. nn is GROUP_CTRL, if applicable, otherwise is n. XXX
//	int g = -1;
	if(owner == ENUM_OWNER_GROUP_PEER)// && peer[n].message[i].stat == ENUM_MESSAGE_RECV)
	{
		const int group_n = getter_group_int(g,offsetof(struct group_list,n));
		if(group_n > -1)
			nn = group_n;
	}

	GtkWidget *msg = NULL; // must be null intialized
//	uint8_t finished_file = 0;
	uint8_t finished_image = 0;
	char *filename;
	char *file_path;
	uint64_t transferred = 0; // clang wants this initialized, but it doesn't need to be
	if(f > -1) // File
	{
		int tmp_n;
		if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
			tmp_n = nn;
		else
			tmp_n = n;
		filename = getter_string(NULL,tmp_n,INT_MIN,f,offsetof(struct file_list,filename));
		file_path = getter_string(NULL,tmp_n,INT_MIN,f,offsetof(struct file_list,file_path));
		const uint64_t size = getter_uint64(tmp_n,INT_MIN,f,-1,offsetof(struct file_list,size));
		transferred = calculate_transferred(tmp_n,f);
		if(size > 0 && size == transferred && size == get_file_size(file_path)) // is finished file
		{
		//	finished_file = 1;
			finished_image = is_image_file(file_path);
		}
		if(!finished_image)
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
			char peernick[56+1];
			if(group_n > -1)
				getter_array(&peernick,sizeof(peernick),group_n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
			char group_message[256];
			if(group_n > -1 && strlen(peernick))
				snprintf(group_message,sizeof(group_message),"%s\n%s %u\n%s",group_type,text_current_members,peercount,peernick);
			else
			{ // We have not joined yet, so no name. Use encoded GroupID instead
				unsigned char id[GROUP_ID_SIZE];
				pthread_rwlock_rdlock(&mutex_expand_group);
				memcpy(id,group[g].id,sizeof(id));
				pthread_rwlock_unlock(&mutex_expand_group);
				char *group_encoded = b64_encode(id,GROUP_ID_SIZE);
				sodium_memzero(id,sizeof(id));
				snprintf(group_message,sizeof(group_message),"%s\n%s %u\n%s",group_type,text_current_members,peercount,group_encoded);
				torx_free((void*)&group_encoded);
			}
			sodium_memzero(peernick,sizeof(peernick));
			msg = gtk_label_new(group_message);
			sodium_memzero(group_message,sizeof(group_message));
		}
		else if(message_len >= CHECKSUM_BIN_LEN && (protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED))
		{
			const int s = ui_sticker_set((unsigned char*)message);
			if(s < 0 || sticker[s].paintable_animated == NULL || !(msg = ui_sticker_box(sticker[s].paintable_animated,size_sticker_large)))
			{
				if(stat == ENUM_MESSAGE_RECV)
					message_send(n,ENUM_PROTOCOL_STICKER_REQUEST,message,CHECKSUM_BIN_LEN);
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
		else
		{
			char trash[64];
			snprintf(trash,sizeof(trash),"Unrecognized Protocol %d",protocol); // note: or bunk length, see tmp_message_len checks
			msg = gtk_label_new(trash);
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
		int nnn = n;
		if(group_msg && owner == ENUM_OWNER_GROUP_PEER)
			nnn = nn; // group_n
		if(!finished_image)
		{
			file_icon = ui_get_icon_from_filename(filename);
			gtk_widget_set_size_request(file_icon, size_file_icon, size_file_icon);
		}
		if(long_press)
			g_signal_connect(long_press, "cancelled", G_CALLBACK(ui_toggle_file),iitovp(nnn,f)); // clicked proper for buttons; this is proper for boxes  // DO NOT FREE arg because this only gets passed ONCE.

		if(finished_image)
		{
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
		//	const uint64_t transferred = calculate_transferred(nnn,f);
			char *file_size_text = file_progress_string(nnn,f);
			t_peer[nnn].t_file[f].file_size = gtk_label_new(file_size_text);
			torx_free((void*)&file_size_text);
			double fraction = 0;
			const uint64_t size = getter_uint64(nnn,INT_MIN,f,-1,offsetof(struct file_list,size));
			if(size > 0)
				fraction = (1.000 *(double)transferred / (double)size);

			t_peer[nnn].t_file[f].progress_bar = gtk_progress_bar_new();
			t_peer[nnn].t_file[f].n = n;
			t_peer[nnn].t_file[f].i = i;
			gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(t_peer[nnn].t_file[f].progress_bar),TRUE);
			gtk_progress_bar_set_text(GTK_PROGRESS_BAR(t_peer[nnn].t_file[f].progress_bar),NULL);
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(t_peer[nnn].t_file[f].progress_bar),fraction);
			GtkWidget *text_area = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
			gtk_box_append(GTK_BOX(text_area),msg);
			gtk_box_append(GTK_BOX(text_area),t_peer[nnn].t_file[f].file_size);
			gtk_box_append(GTK_BOX(text_area),t_peer[nnn].t_file[f].progress_bar);
			gtk_widget_set_visible(t_peer[nnn].t_file[f].progress_bar,FALSE);

			gtk_grid_attach (GTK_GRID(grid),file_icon,0,0,1,1);
			gtk_grid_attach (GTK_GRID(grid),text_area,1,0,1,1);
		//	gtk_box_append(GTK_BOX(inner_message_box),file_icon);
		//	gtk_box_append(GTK_BOX(inner_message_box),text_area);
		//	printf("Checkpoint ui_print_message nnn==%d f==%d\n",nnn,f);
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
			g_signal_connect_swapped(gesture, "pressed", G_CALLBACK(ui_group_join),iitovp(n,i)); // do not free
			gtk_widget_add_controller(outer_message_box, GTK_EVENT_CONTROLLER(gesture));
		}
	//	GtkWidget *text_area = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	//	gtk_box_append(GTK_BOX(text_area),msg);
		gtk_grid_attach (GTK_GRID(grid),file_icon,0,0,1,1);
		gtk_grid_attach (GTK_GRID(grid),msg,1,0,1,1);
	//	gtk_box_append(GTK_BOX(inner_message_box),file_icon);
	//	gtk_box_append(GTK_BOX(inner_message_box),text_area);
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
{
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
			const uint8_t stat = getter_uint8(n,i,-1,-1,offsetof(struct message_list,stat));
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
		const uint8_t stat = getter_uint8(n,i,-1,-1,offsetof(struct message_list,stat));
		if(stat == ENUM_MESSAGE_RECV)
			gtk_widget_set_halign(message_box, GTK_ALIGN_START);
		else
			gtk_widget_set_halign(message_box, GTK_ALIGN_END);
		gtk_list_item_set_child(list_item, message_box);
		#endif
	}
}

static void ui_print_message(const int n,const int i,const int scroll)
{ // use _idle or _cb unless in main thread // TODO TODO TODO XXX this function is too complex, theorize how to move most of it to library so we don't have to maintain it in UI
	if(n < 0)
	{
		error_simple(0,"Sanity check failed in ui_print_message. Coding error. Report this.");
		breakpoint();
		return;
	}
	int nn = n; // XXX IMPORTANT: usage of n when relating to specific message, nn when relating to group settings or global_n. nn is GROUP_CTRL, if applicable, otherwise is n. XXX
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	const int p_iter = getter_int(n,i,-1,-1,offsetof(struct message_list,p_iter));
	#if GTK_FACTORY_BUG
	if((scroll == 2 || scroll == 3) && t_peer[n].t_message[i].message_box)
	{
		g_object_unref(t_peer[n].t_message[i].message_box);
		t_peer[n].t_message[i].message_box = NULL;
	}
	#endif
	if(p_iter == -1)
	{ // hide or do not print (deleted message)
		if(t_peer[n].t_message[i].visible)
		{
			t_peer[n].t_message[i].visible = 0;
			IntPair *pair = int_pair_new(n,i,-1,-1);
			if(INVERSION_TEST)
				g_list_store_splice (t_main.list_store_chat,t_main.global_pos - t_peer[n].t_message[i].pos,1,(void**)&pair,1);
			else
				g_list_store_splice (t_main.list_store_chat,t_peer[n].t_message[i].pos,1,(void**)&pair,1);
		}
		t_peer[n].t_message[i].pos = 0;
		goto skip_printing;
	}
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint16_t protocol = protocols[p_iter].protocol;
	const uint8_t notifiable = protocols[p_iter].notifiable;
	const uint8_t group_pm = protocols[p_iter].group_pm;
	const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
//	const uint8_t group_msg = protocols[p_iter].group_msg;
	pthread_rwlock_unlock(&mutex_protocols);
	uint32_t message_len;
	char *message = getter_string(&message_len,n,i,-1,offsetof(struct message_list,message));
	const uint8_t stat = getter_uint8(n,i,-1,-1,offsetof(struct message_list,stat));
	if(scroll == 2 && stat != ENUM_MESSAGE_RECV && message_len >= CHECKSUM_BIN_LEN && (protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED))
	{ // THE FOLLOWING IS IMPORTANT TO PREVENT FINGERPRINTING BY STICKER WALLET. XXX NOTE: To allow offline stickers to work for GROUP messages, also permit scroll == 1 (prolly very bad idea thou because unlimitd peers could repeatedly request)
		const int s = ui_sticker_set((unsigned char*)message);
		int iter = 0;
		while(iter < MAX_PEERS && sticker[s].peers[iter] != n && sticker[s].peers[iter] > -1)
			iter++;
		if(iter < MAX_PEERS && sticker[s].peers[iter] < 0) // Register a new recipient of sticker so that they can request data
			sticker[s].peers[iter] = n;
	}
	if(!notifiable)
	{ // not notifiable message type or group peer is ignored. could still flash something.
		if(n == global_n && scroll == 1) // Done printing messages. Last in a list. (Such as, when select_changed() prints a bunch at once). Things that only go once go here. XXX
		{
			if(INVERSION_TEST)
				g_idle_add_full(301,scroll_func_idle_inverted,t_main.scrolled_window_right,NULL);
			else
				g_idle_add_full(301,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
		}
		torx_free((void*)&message);
		return; // do not display
	}
	int g = -1;
	if(owner == ENUM_OWNER_GROUP_PEER)// && peer[n].message[i].stat == ENUM_MESSAGE_RECV)
	{
		if(!group_pm && stat != ENUM_MESSAGE_RECV)
		{
			torx_free((void*)&message);
			return;	// Do not print OUTBOUND messages on GROUP_PEER unless they are private
		}
		const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
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
			char peernick[56+1];
			getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
			if(null_terminated_len == 1)
				ui_notify(peernick,message);
			else if(protocol == ENUM_PROTOCOL_FILE_OFFER || protocol == ENUM_PROTOCOL_FILE_OFFER_PRIVATE)
			{
				const int f = set_f(n,(unsigned char*)message,CHECKSUM_BIN_LEN);
				char *filename = getter_string(NULL,n,INT_MIN,f,offsetof(struct file_list,filename));
				ui_notify(peernick,filename);
				torx_free((void*)&filename);
			}
			else if(protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP || protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP_DATE_SIGNED)
			{ // group, so use group_n
				getter_array(&peernick,sizeof(peernick),nn,INT_MIN,-1,-1,offsetof(struct peer_list,peernick)); // XXX
				const int f = set_f(nn,(unsigned char*)message,CHECKSUM_BIN_LEN);
				char *filename = getter_string(NULL,nn,INT_MIN,f,offsetof(struct file_list,filename));
				ui_notify(peernick,filename);
				torx_free((void*)&filename);
			}
			else if(protocol == ENUM_PROTOCOL_FILE_REQUEST)
				ui_notify(peernick,text_accepted_file);
			else if(protocol == ENUM_PROTOCOL_GROUP_OFFER || protocol == ENUM_PROTOCOL_GROUP_OFFER_FIRST)
				ui_notify(peernick,text_group_offer);
			else if(protocol == ENUM_PROTOCOL_STICKER_HASH || protocol == ENUM_PROTOCOL_STICKER_HASH_PRIVATE || protocol == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED)
				ui_notify(peernick,text_sticker);
			else
				error_printf(0,"A notifiable message was received that we don't know how to print: %u",protocol);
			sodium_memzero(peernick,sizeof(peernick));
			beep();
		}
	}
	if(nn == global_n)
	{ // Print it. DO NOT make this else if (needs to occur even if window not visible)
		int f = -1;
		if(protocol == ENUM_PROTOCOL_FILE_OFFER || protocol == ENUM_PROTOCOL_FILE_OFFER_PRIVATE)
			f = set_f(n,(unsigned char*)message,CHECKSUM_BIN_LEN);
		else if(protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP || protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP_DATE_SIGNED)
			f = set_f(nn,(unsigned char*)message,CHECKSUM_BIN_LEN);
		else if(protocol == ENUM_PROTOCOL_FILE_REQUEST)
		{
			if(scroll == 1) // Done printing messages. Last in a list. (Such as, when select_changed() prints a bunch at once). Things that only go once go here. XXX
			{
				if(INVERSION_TEST)
					g_idle_add_full(301,scroll_func_idle_inverted,t_main.scrolled_window_right,NULL);
				else
					g_idle_add_full(301,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
			}
			torx_free((void*)&message);
			return; // do not display other types of files messages
		}
		// Handle :sent: TODO
		if(scroll == 2 || scroll == 3)
		{ // Update but *do not* actually scroll
		//	ui_message_info(n,i);
//printf("Checkpoint derp 2\n");
			IntPair *pair = int_pair_new(n,i,f,g);
			/* guint n_items = g_list_model_get_n_items(G_LIST_MODEL(t_main.list_store_chat));
			printf("Checkpoint splice: pos=%u + rem=1 <=? n_items=%u\n",t_main.global_pos - t_peer[n].t_message[i].pos,n_items);
			printf("Checkpoint splice: n=%d i=%d global_pos=%u message pos=%u\n",n,i,t_main.global_pos,t_peer[n].t_message[i].pos); */
			if(INVERSION_TEST)
				g_list_store_splice (t_main.list_store_chat,t_main.global_pos - t_peer[n].t_message[i].pos,1,(void**)&pair,1);
			else
				g_list_store_splice (t_main.list_store_chat,t_peer[n].t_message[i].pos,1,(void**)&pair,1);
			torx_free((void*)&message);
			return;
		}
		t_peer[n].t_message[i].visible = 1; // goes before g_list_store_append
		if(INVERSION_TEST)
		{
		/*	IntPair *pair = int_pair_new(n,i,f,g);
			g_list_store_splice(t_main.list_store_chat, 0, 0, (void**)&pair,1); */ // This is the same as g_list_store_insert
			g_list_store_insert(t_main.list_store_chat, 0, int_pair_new(n,i,f,g));
			t_peer[n].t_message[i].pos = ++t_main.global_pos; // goes after g_list_store_append and only here // XXX NOTE THE DIFFERENCE, ++ before
		}
		else
		{
			g_list_store_append(t_main.list_store_chat, int_pair_new(n,i,f,g));
			t_peer[n].t_message[i].pos = t_main.global_pos++; // goes after g_list_store_append and only here // XXX NOTE THE DIFFERENCE, ++ after
		}
		if(scroll == 1 || scroll == 3) // Done printing messages. Last in a list. (Such as, when select_changed() prints a bunch at once). Things that only go once go here. XXX 2024/03/09 note: ==3 because message could change size
		{
			if(INVERSION_TEST)
				g_idle_add_full(301,scroll_func_idle_inverted,t_main.scrolled_window_right,NULL);
			else
				g_idle_add_full(301,scroll_func_idle,t_main.scrolled_window_right,NULL); // TODO should not be necessary to make this idle but we have to delay it somehow?? // scroll_func_idle(t_main.scrolled_window_right);
		}
	}
	torx_free((void*)&message);
	skip_printing: {}
	if(scroll)
	{ // == 1 or == 2 or == 3
		const int max_i = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,max_i));
		if(scroll == 1 || i >= max_i) // TODO 2024/02/22 Minor bug: when deleting the last message (max_i), this if statement is not activated. A bad work-ardound would be to pass scroll==1, but a better one would be to check the validity of max_i and look lower.
		{ // populate_peers if last message or commanded (1)
			if(owner == ENUM_OWNER_GROUP_CTRL || owner == ENUM_OWNER_GROUP_PEER)
				ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL)); // Necessary for last_message and also the order
			else
			{
				const uint8_t status_nn = getter_uint8(nn,INT_MIN,-1,-1,offsetof(struct peer_list,status));
				ui_populate_peers(itovp(status_nn)); // Necessary for last_message and also the order
			}
		}
	}

}

static int print_message_idle(void *arg)
{ // this step is necessary
	struct printing *printing = (struct printing*) arg; // Casting passed struct
	ui_print_message(printing->n,printing->i,printing->scroll);
	torx_free((void*)&printing);
	return 0;
}

void message_new_cb_ui(const int n,const int i)
{ // GUI Callback
	struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
	printing->n = n;
	printing->i = i;
	printing->scroll = 1;
	g_idle_add(print_message_idle,printing);
}

void message_modified_cb_ui(const int n,const int i)
{ // GUI Callback
	struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
	printing->n = n;
	printing->i = i;
	printing->scroll = 2;
	g_idle_add(print_message_idle,printing);
}

void message_deleted_cb_ui(const int n,const int i)
{ // GUI Callback
	struct printing *printing = torx_insecure_malloc(sizeof(struct printing));
	printing->n = n;
	printing->i = i;
	printing->scroll = 3;
	g_idle_add(print_message_idle,printing);
}

void stream_cb_ui(const int n,const int p_iter,char *data,const uint32_t data_len)
{ // XXX WARNING: Not _idle function, DO NOT do UI things here // torx_secure_free required XXX
	if(data == NULL || data_len == 0 || n < 0 || p_iter < 0)
		goto end;
	pthread_rwlock_rdlock(&mutex_protocols);
	const uint16_t protocol = protocols[p_iter].protocol;
	pthread_rwlock_unlock(&mutex_protocols);
	uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
	if((owner == ENUM_OWNER_GROUP_PEER && t_peer[n].mute) || status == ENUM_STATUS_BLOCKED)
		goto end; // ignored or blocked
	if(data_len >= CHECKSUM_BIN_LEN && protocol == ENUM_PROTOCOL_STICKER_REQUEST)
	{
		if(send_sticker_data == 0)
			goto end;
		const int s = ui_sticker_set((unsigned char*)data);
		int relevant_n = n; // TODO for groups, this should be group_n
		try_group_n: {}
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
				owner = getter_uint8(relevant_n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
				goto try_group_n;
			}
			else
				error_simple(0,"Peer requested a sticker they dont have access to (either they are buggy or malicious, or our MAX_PEERS is too small). Report this.");
		}
		else if(s > -1 && sticker[s].paintable_animated != NULL)
		{ // Peer requested a sticker we have
			if(sticker[s].data) // Peer requested an unsaved sticker we have
				message_send(n,ENUM_PROTOCOL_STICKER_DATA_GIF,sticker[s].data,(uint32_t)sticker[s].data_len);
			else
			{ // Peer requested a saved sticker we have
				char *p = b64_encode(data,CHECKSUM_BIN_LEN);
				char query[256]; // somewhat arbitrary size
				snprintf(query,sizeof(query),"sticker-gif-%s",p);
				torx_free((void*)&p);
				size_t setting_value_len;
				unsigned char *setting_value = sql_retrieve(&setting_value_len,0,query);
				sodium_memzero(query,sizeof(query));
			//	printf("Checkpoint setting_value_len: %lu\n",setting_value_len);
				unsigned char *message = torx_secure_malloc(CHECKSUM_BIN_LEN + setting_value_len);
				memcpy(message,data,CHECKSUM_BIN_LEN);
				memcpy(&message[CHECKSUM_BIN_LEN],setting_value,setting_value_len);
				torx_free((void*)&setting_value);
				message_send(n,ENUM_PROTOCOL_STICKER_DATA_GIF,message,CHECKSUM_BIN_LEN + (uint32_t)setting_value_len); // TODO TODO TODO get it from sql_setting
				torx_free((void*)&message);
			}
		}
		else
			error_simple(0,"Peer requested sticker we do not have. Maybe we deleted it.");
	}
	else if(data_len >= CHECKSUM_BIN_LEN && protocol == ENUM_PROTOCOL_STICKER_DATA_GIF)
	{
		const int s_check = ui_sticker_set((unsigned char*)data);
		if(sticker[s_check].paintable_animated)
		{ // Old sticker data, do not print or register (such as re-opening peer route)
			error_simple(0,"We already have this sticker data, do not register or print again.");
		}
		else
		{ // Fresh sticker data. Save it and print it
			const int s = ui_sticker_register((unsigned char*)&data[CHECKSUM_BIN_LEN],data_len - CHECKSUM_BIN_LEN);
			sticker[s].data = torx_secure_malloc(data_len - CHECKSUM_BIN_LEN);
			memcpy(sticker[s].data,(unsigned char*)&data[CHECKSUM_BIN_LEN],data_len - CHECKSUM_BIN_LEN);
			sticker[s].data_len = data_len - CHECKSUM_BIN_LEN;
			if(save_all_stickers)
				ui_sticker_save(itovp(s));
			torx_read(n) // XXX
			for(int i = peer[n].max_i; i > peer[n].min_i - 1 ; i--)
			{ // Rebuild any messages that had this sticker (NOTE: may not be functioning)
				const int p_iter_local = peer[n].message[i].p_iter;
				if(p_iter_local > -1)
				{
					const uint16_t protocol_local = protocols[p_iter_local].protocol;
					if((protocol_local == ENUM_PROTOCOL_STICKER_HASH || protocol_local == ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED || protocol_local == ENUM_PROTOCOL_STICKER_HASH_PRIVATE)
					&& !memcmp(peer[n].message[i].message,sticker[s].checksum,CHECKSUM_BIN_LEN))
					{ // Find the first relevant message and update it TODO this might not work in groups
						torx_unlock(n) // XXX
						printf("Checkpoint should be rebuilding a sticker??\n");
						ui_print_message(n,i,2);
						torx_read(n) // XXX
						break;
					}
				}
			}
			torx_unlock(n) // XXX
		}
	}
/*	else if(protocol == ENUM_PROTOCOL_TEST_STREAM && data_len)
	{
		char *message = torx_secure_malloc(data_len+1);
		memcpy(message,data,data_len);
		message[data_len] = '\0';
		printf("Checkpoint stream_cb %d %d: %s\n",n,p_iter,message);
		torx_free((void*)&message);
	} */
	else
		error_printf(0,"Unknown stream data received: protocol=%u data_len=%u",protocol,data_len);
	torx_free((void*)&data);
	return;
	end: {}
	error_simple(0,"Potential issue in stream_cb.");
	torx_free((void*)&data);
}

static void ui_keypress(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{ // Press key on message entry /* can pass NULL as arg */
	(void) user_data;
	(void) controller;
	(void) keycode;
	(void) state;
//	fprintf(stderr,"%d ",keyval); // TODO with the results of this, we can do ctrl+enter for send,etc (edit: nope, not viable. ctrl+k is same if held or not)
	if(!keyval || ((keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) && !(state & GDK_SHIFT_MASK))) // 0 == pressed enter
	{
		GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t_main.write_message));
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(buffer, &start, &end);
		char *message = gtk_text_buffer_get_text(buffer, &start, &end, FALSE); // false ignores hidden chars
		if(!message || message[0] == '\0') // message[0] == '\n' // removed prohibition against newline start because it was issues.
			return; // Sanity check
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
			if(t_peer[global_n].edit_n > -1 && t_peer[global_n].edit_i > INT_MIN)
			{
				message_edit(t_peer[global_n].edit_n,t_peer[global_n].edit_i,message);
				ui_activity_cancel(NULL);
			}
			else if(t_peer[global_n].edit_n > -1)
			{
				change_nick(t_peer[global_n].edit_n,message);
				ui_activity_cancel(NULL);
				// TODO cycle through displayed messages and call ui_print_message on any that need to be updated
			}
			else if(t_peer[global_n].pm_n > -1)
			{ // send to GROUP_PEER instead of GROUP_CTRL
				message_send(t_peer[global_n].pm_n,ENUM_PROTOCOL_UTF8_TEXT_PRIVATE,message,(uint32_t)strlen(message));
			/*	const char test_stream[] = "i leik catz (pm)\n";
				message_send(pm_n,ENUM_PROTOCOL_TEST_STREAM,test_stream,(uint32_t)strlen(test_stream));	*/
				gtk_text_buffer_set_text(buffer, "", 0); // this occurs in ui_activity_cancel
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
			const uint8_t owner = getter_uint8(global_n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
			if(owner == ENUM_OWNER_GROUP_CTRL)
			{
				g = set_g(global_n,NULL);
				g_invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
			}
			if((force_sign == 2 && owner == ENUM_OWNER_CTRL) || (owner == ENUM_OWNER_GROUP_CTRL && g_invite_required)) // date && sign private group messages
				message_send(global_n,ENUM_PROTOCOL_UTF8_TEXT_DATE_SIGNED,message,(uint32_t)strlen(message));
		//	else if(force_sign == 1 && owner == ENUM_OWNER_CTRL)
		//		message_send(global_n,ENUM_PROTOCOL_UTF8_TEXT_SIGNED,message,(uint32_t)strlen(message));
			else // regular messages, private messages (in authenticated pipes), public messages in public groups (in authenticated pipes)
				message_send(global_n,ENUM_PROTOCOL_UTF8_TEXT,message,(uint32_t)strlen(message));
		/*	const char test_stream[] = "i leik catz (non-pm)\n";
			message_send(global_n,ENUM_PROTOCOL_TEST_STREAM,test_stream,(uint32_t)strlen(test_stream));	*/
			gtk_text_buffer_set_text(buffer, "", 0); // this occurs in ui_activity_cancel
		}
		sodium_memzero(message,buf_len);
		g_free(message);
		message = NULL;
	} /* else Non-enter keypresses are unhandled */
}

static void ui_editing_nick(GtkCellEditable *self,GParamSpec *pspec,gpointer data)
{
	(void) pspec;
	const int n = vptoi(data); // DO NOT FREE ARG
	if(!gtk_editable_label_get_editing(GTK_EDITABLE_LABEL(self)))
	{
		const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
		char freshpeernick[56+1]; // zero'd
		char *p = gtk_editable_get_chars(GTK_EDITABLE(self),0,56);
		if(p)
		{
			snprintf(freshpeernick,sizeof(freshpeernick),"%s",p);
			sodium_memzero(p,strlen(p));
			g_free(p);
			p = NULL;
		}
		char peernick[56+1];
		getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
		if(strlen(freshpeernick) == 0 || !strncmp(freshpeernick," ",1))
		{
			gtk_editable_set_text(GTK_EDITABLE(self),peernick);
			sodium_memzero(peernick,sizeof(peernick));
			return;
		}
		else if(!strncmp(freshpeernick,peernick,56))
		{
			sodium_memzero(peernick,sizeof(peernick));
			return;
		}
		else
		{
			sodium_memzero(peernick,sizeof(peernick));
			change_nick(n,freshpeernick);
			if(owner == ENUM_OWNER_GROUP_CTRL)
				ui_populate_peers(itovp(ENUM_STATUS_GROUP_CTRL));
			else
			{
				const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
				ui_populate_peers(itovp(status));
			}
		}
		sodium_memzero(freshpeernick,sizeof(freshpeernick));
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

static void chat_builder(GtkListItemFactory *factory, GtkListItem *list_item,void (*callback_click)(const void *))
{
	(void) factory;
	gtk_list_item_set_activatable(list_item,TRUE); // XXX this changes whether hover is visible
	IntPair *pair = gtk_list_item_get_item(list_item);
	if(pair)
	{
		GtkWidget *node = ui_add_chat_node(pair->first,callback_click,pair->third);
		gtk_widget_add_css_class(node, "node");
		gtk_list_item_set_child(list_item, node);
	}
}

static int ui_populate_peers(void *arg) // g_idle_add() functions must return int
{ /* Search Letters Entered */ // Takes 0 or ENUM_STATUS_BLOCKED / ENUM_STATUS_FRIEND as argument
// TODO should probably save the scroll point, or not scroll if not at bottom, since this will be triggered on every message in/out
	if(threadsafe_read_int8(&mutex_global_variable,(int8_t*)&lockout))
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
				const uint8_t status_local = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
				if(status == ENUM_STATUS_GROUP_CTRL)
					g_list_store_append(list_store, int_pair_new(array[pos],-1,0,-1));
				else if(status == ENUM_STATUS_BLOCKED && status_local == ENUM_STATUS_BLOCKED)
					g_list_store_append(list_store, int_pair_new(array[pos],-1,0,-1));
				else if(status == ENUM_STATUS_FRIEND && status_local == ENUM_STATUS_FRIEND)
					g_list_store_append(list_store, int_pair_new(array[pos],-1,0,-1));
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
			g_list_store_append(list_store, int_pair_new(array[pos],-1,3,-1));
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
			g_list_store_append(list_store, int_pair_new(array[pos],-1,1,-1));
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
		t_main.popover_invite = gtk_popover_new();
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_invite),TRUE);
		gtk_widget_set_parent(t_main.popover_invite,button_invite);

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
		GFile *path = g_file_new_for_uri("resource:///org/torx/gtk4/other/search_ico.png"); 
		GIcon *icon = g_file_icon_new(path);
		gtk_entry_set_icon_from_gicon(GTK_ENTRY(entry_search_popover),GTK_ENTRY_ICON_SECONDARY, icon);
		gtk_box_append(GTK_BOX(box_popover_outer), entry_search_popover);

		gtk_box_append(GTK_BOX(box_popover_outer), box_popover_inner);
		ui_populate_peer_popover(entry_search_popover,NULL,box_popover_inner);

		g_signal_connect(t_main.popover_invite, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_invite); // XXX necessary or a warning occurs
		gtk_popover_popup(GTK_POPOVER(t_main.popover_invite));
	}
	else
	{ // Public Group, show QR and Group ID TODO consider having also popover_invite as an option
		unsigned char id[GROUP_ID_SIZE];
		pthread_rwlock_rdlock(&mutex_expand_group);
		memcpy(id,group[g].id,sizeof(id));
		pthread_rwlock_unlock(&mutex_expand_group);
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

		t_main.popover_qr = gtk_popover_new();
		gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_qr),TRUE);
		gtk_widget_set_parent (t_main.popover_qr,button_invite);

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
		g_signal_connect(t_main.popover_qr, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_qr); // XXX necessary or a warning occurs
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
	t_main.popover_block = gtk_popover_new();
	gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_block),TRUE);
	gtk_widget_set_parent (t_main.popover_block,button);
	GtkWidget *box_popover = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
	gtk_widget_add_css_class(box_popover, "popover_inner");
	gtk_popover_set_child(GTK_POPOVER(t_main.popover_block),box_popover);
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	if(owner != ENUM_OWNER_GROUP_CTRL)
	{
		gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_ADD_BLOCK,n));
		gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_KILL,n));
	}
	gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_DELETE_CTRL,n));
	gtk_box_append(GTK_BOX(box_popover),ui_button_generate(ENUM_BUTTON_DELETE_LOG,n));
	g_signal_connect(t_main.popover_block, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_block); // XXX necessary or a warning occurs
	gtk_popover_popup(GTK_POPOVER(t_main.popover_block));
}

GtkWidget *ui_button_generate(const int type,const int n)
{
	GtkWidget *button = gtk_button_new();
	gtk_widget_add_css_class(button, "invisible");
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
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
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive_light)));
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
		if(owner != ENUM_OWNER_GROUP_CTRL)
			ui_set_image_block(button,n);
		else
		{
			if(global_theme == DARK_THEME)
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive)));
			else
				gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(delete_inactive_light)));
		//	gtk_widget_set_tooltip_text(button,text_tooltip_blocked_on); // TODO set some tooltip text for this ("Menu"?)
		}
		gtk_widget_set_size_request(button,size_icon_top_right,size_icon_top_right);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_vertical),itovp(n));
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
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(emoji_dark)));
		else
			gtk_button_set_child(GTK_BUTTON(button),gtk_image_new_from_paintable(GDK_PAINTABLE(emoji_light)));
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
		gtk_widget_set_size_request(button, size_sticker_small, size_sticker_small);
		g_signal_connect(button, "clicked", G_CALLBACK(ui_choose_file),itovp(2)); // DO NOT FREE arg because this only gets passed ONCE.
	}
	return button;
}

static void ui_group_peerlist(const gpointer data)
{
	(void) data;
//	const int g = set_g(vptoi(data),NULL); // DO NOT FREE ARG
	t_main.popover_group_peerlist = gtk_popover_new();
//	gtk_popover_set_cascade_popdown(GTK_POPOVER(t_main.popover_group_peerlist),TRUE); // seems doing nothing
	gtk_popover_set_autohide(GTK_POPOVER(t_main.popover_group_peerlist),TRUE);
	gtk_widget_set_parent(t_main.popover_group_peerlist,t_main.last_online);

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
	GFile *path = g_file_new_for_uri("resource:///org/torx/gtk4/other/search_ico.png"); 
	GIcon *icon = g_file_icon_new(path);
	gtk_entry_set_icon_from_gicon(GTK_ENTRY(entry_search_popover),GTK_ENTRY_ICON_SECONDARY, icon);
	gtk_box_append(GTK_BOX(box_popover_outer), entry_search_popover);

	gtk_box_append(GTK_BOX(box_popover_outer), box_popover_inner);
	ui_populate_group_peerlist_popover(entry_search_popover,NULL,box_popover_inner);

	g_signal_connect(t_main.popover_group_peerlist, "closed", G_CALLBACK (gtk_widget_unparent), t_main.popover_group_peerlist); // XXX necessary or a warning occurs
	gtk_popover_popup(GTK_POPOVER(t_main.popover_group_peerlist));
}


static inline void on_file_drop(GtkDropTarget *drop_target, const GValue *value, gpointer arg)
{ // https://gitlab.gnome.org/GNOME/gtk/-/issues/3755
	(void) drop_target;
	const int n = vptoi(arg);
	GFile *file = g_value_get_object(value);
	char *file_path = g_file_get_path(file); // free'd
	file_send(n,file_path);
	g_free(file_path); file_path = NULL;
}

static void ui_select_changed(const void *arg)
{ /* show_peer() Triggered when a peer is selected from the peer list */
	const int n = vptoi(arg); // DO NOT FREE ARG
	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
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
			const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
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
	char peernick[56+1];
	getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
	GtkWidget *group_or_user_name = gtk_editable_label_new(peernick);
	sodium_memzero(peernick,sizeof(peernick));
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
	t_main.global_pos = 0;

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
	GtkDropTarget *drop_target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
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
	g_signal_connect_after(return_controller, "key-released", G_CALLBACK(ui_keypress),NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_widget_add_controller(t_main.write_message,  GTK_EVENT_CONTROLLER(return_controller));
	gtk_box_append(GTK_BOX(write_box), write_message_scroll);

	/* Send Button */ // TODO have a hover gesture too, 	g_signal_connect_after(ev_enter, "enter", G_CALLBACK(enter_mouse), box_arr); 
	t_main.button_send = gtk_button_new();
//	if(global_theme == DARK_THEME)
//		gtk_button_set_child(GTK_BUTTON(t_main.button_send),gtk_image_new_from_paintable(GDK_PAINTABLE(send_inactive)));
//	else // TODO use "Enter Password" as a model instead of an image
//		gtk_button_set_child(GTK_BUTTON(t_main.button_send),gtk_image_new_from_paintable(GDK_PAINTABLE(send_inactive)));
	gtk_widget_set_size_request(t_main.button_send, size_icon_send_width, size_icon_bottom_right);
	gtk_button_set_label(GTK_BUTTON(t_main.button_send),text_button_send);
	gtk_widget_add_css_class(t_main.button_send,"auth_button");
//	gtk_style_context_add_class(gtk_widget_get_style_context(t_main.button_send),"auth_button");	// XXX this is for the :hover functionality. Important.
//	gtk_style_context_add_provider(gtk_widget_get_style_context(t_main.button_send), GTK_STYLE_PROVIDER(t_main.provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	g_signal_connect(t_main.button_send, "clicked", G_CALLBACK(ui_keypress),NULL); // DO NOT FREE arg because this only gets passed ONCE.
	gtk_box_append(GTK_BOX(write_box), t_main.button_send);

	gtk_box_append(GTK_BOX(write_box), ui_button_generate(ENUM_BUTTON_EMOJI,n));
	gtk_box_append(GTK_BOX(write_box), ui_button_generate(ENUM_BUTTON_STICKER,n));
	gtk_box_append(GTK_BOX(write_box), ui_button_generate(ENUM_BUTTON_ATTACH,n));

	/* Set margins on write box */
	gtk_widget_set_margin_top(write_box, size_margin_ten);
	gtk_widget_set_margin_start(write_box, size_margin_ten);
	gtk_widget_set_margin_end(write_box, size_margin_ten);
	gtk_widget_set_margin_bottom(write_box, size_margin_fifteen);

	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.scrolled_window_right);

	// Current activity (Edit, private message)
	t_main.button_activity = gtk_button_new();
	if(t_peer[n].pm_n > -1 || t_peer[n].edit_n > -1)
	{
		if(t_peer[n].pm_n > -1)
		{
			char peernick_local[56+1];
			getter_array(&peernick_local,sizeof(peernick_local),t_peer[n].pm_n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
			char cancel_message[256];
			snprintf(cancel_message,sizeof(cancel_message),"%s %s",text_private_messaging,peernick_local);
			sodium_memzero(peernick_local,sizeof(peernick_local));
			gtk_button_set_label(GTK_BUTTON(t_main.button_activity),cancel_message);
			sodium_memzero(cancel_message,sizeof(cancel_message));
		}
		else if(t_peer[n].edit_n > -1)
		{
			gtk_button_set_label(GTK_BUTTON(t_main.button_activity),text_cancel_editing);
			gtk_button_set_label(GTK_BUTTON(t_main.button_send),text_done);
		}
		g_signal_connect(t_main.button_activity, "clicked", G_CALLBACK(ui_activity_cancel),NULL);
		gtk_widget_set_visible(t_main.button_activity,TRUE);
	}
	else
		gtk_widget_set_visible(t_main.button_activity,FALSE);
	gtk_widget_set_size_request(t_main.button_activity, size_icon_send_width, size_icon_bottom_right);
	gtk_box_append(GTK_BOX(t_main.panel_right), t_main.button_activity);

	gtk_box_append(GTK_BOX(t_main.panel_right), write_box);

	if(owner == ENUM_OWNER_GROUP_CTRL)
	{
		const int g = set_g(n,NULL);
		struct msg_list *page = group[g].msg_first;
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
		const int max_i = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,max_i));
		const int min_i = getter_int(n,INT_MIN,-1,-1,offsetof(struct peer_list,min_i));
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

GtkWidget *ui_add_chat_node(const int n,void (*callback_click)(const void *),const int minimal_size)
{ /* XXX Note: Populate_list() is more refined and flexible than this. This only handles CTRL onions, status 0 or 1. */
	/* Build Invisible Button To Contain Peer Info */
	GtkWidget *button_peer = gtk_button_new();
	if(minimal_size == 0)
		gtk_widget_set_size_request(button_peer,size_peerlist_minimum_width,size_peerlist_button_height);
	else
		gtk_widget_set_size_request(button_peer,size_peerlist_minimum_width,size_peerlist_button_height/2);

	/* Build the button / node */
	GtkWidget *chat_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, size_spacing_zero);
	gtk_button_set_child(GTK_BUTTON (button_peer), chat_info);
	GtkWidget *chat_info_vert = gtk_box_new(GTK_ORIENTATION_VERTICAL, size_spacing_zero);
	gtk_widget_set_valign(chat_info_vert, GTK_ALIGN_CENTER);

	/* Add Click Function Callbacks */

	if(minimal_size == 3)
	{ // This is only passed for Group Peerlist
		// Build popover
		GtkWidget *popover = gtk_popover_new();
		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,size_spacing_zero);
		gtk_widget_add_css_class(box, "popover_inner");
		const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
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
		gtk_popover_set_autohide(GTK_POPOVER(popover),TRUE);
		gtk_widget_set_parent(popover,button_peer);
		g_signal_connect(popover, "closed", G_CALLBACK (gtk_widget_unparent), popover); // XXX necessary or a warning occurs

		// Single click
		GtkGesture *long_press = gtk_gesture_long_press_new(); // NOTE: "cancelled" == single click, "pressed" == long press
		gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(long_press),LONG_PRESS_TIME);
		g_signal_connect_swapped(long_press, "cancelled", G_CALLBACK(callback_click),itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.

		// Long press
	//	struct long_presses *long_presses = torx_insecure_malloc(sizeof(long_presses));
	//	long_presses->button = button_peer;
	//	long_presses->n = n;
	//	g_signal_connect_swapped(long_press, "pressed", G_CALLBACK(ui_group_peerlist_sub),long_presses); // DO NOT FREE arg because this only gets passed ONCE.
		g_signal_connect_swapped(long_press, "pressed", G_CALLBACK(gtk_popover_popup),GTK_POPOVER(popover)); // DO NOT FREE arg because this only gets passed ONCE.

		gtk_widget_add_controller(button_peer, GTK_EVENT_CONTROLLER(long_press));

		// Tooltip
		const char *identifier;
		char onion[56+1];
		if(threadsafe_read_uint8(&mutex_global_variable,&shorten_torxids) == 1)
		{
			identifier = text_torxid;
			getter_array(&onion,52+1,n,INT_MIN,-1,-1,offsetof(struct peer_list,torxid));
		}
		else
		{
			identifier = text_onionid;
			getter_array(&onion,sizeof(onion),n,INT_MIN,-1,-1,offsetof(struct peer_list,onion));
		}
		char tooltip[256];
		const int g = set_g(n,NULL);
		const uint8_t invite_required = getter_group_uint8(g,offsetof(struct group_list,invite_required));
		int invitor_n = -1;
		if(invite_required)
		{
			unsigned char verification_message[56+crypto_sign_PUBLICKEYBYTES];
			torx_read(n) // XXX
			const unsigned char *invitation = peer[n].invitation;
			memcpy(verification_message,peer[n].peeronion,56);
			memcpy(&verification_message[56],peer[n].peer_sign_pk,crypto_sign_PUBLICKEYBYTES);
			torx_unlock(n) // XXX
			invitor_n = group_check_sig(g,(char *)verification_message,sizeof(verification_message),0,invitation,NULL);
			sodium_memzero(verification_message,sizeof(verification_message));
		}
		if(invitor_n > -1)
		{
			torx_read(invitor_n) // XXX
			snprintf(tooltip,sizeof(tooltip),"%s: %s\n%s: %s",identifier,onion,text_invitor,peer[invitor_n].peernick);
			torx_unlock(invitor_n) // XXX
		}
		else
			snprintf(tooltip,sizeof(tooltip),"%s: %s",identifier,onion);
		gtk_widget_set_tooltip_text(button_peer,tooltip);
		sodium_memzero(onion,sizeof(onion));
		sodium_memzero(tooltip,sizeof(tooltip));
	}
	else
	{
		GtkGesture *click = gtk_gesture_click_new();
		g_signal_connect_swapped(click, "pressed", G_CALLBACK(callback_click), itovp(n)); // DO NOT FREE arg because this only gets passed ONCE.
		gtk_widget_add_controller(button_peer, GTK_EVENT_CONTROLLER(click));
	}

	const uint8_t owner = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,owner));
	const uint8_t status = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,status));
	/* Build Avatar or Online Status Indicator Image */
	GtkWidget *image_peerlist;
	if(owner == ENUM_OWNER_GROUP_CTRL) // Handle group TODO should have a > arrow that goes to downward pointing upon click, to show individual GROUP_PEERs
		image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(texture_logo));
	else if(status == ENUM_STATUS_BLOCKED) // Handling blocks second
		image_peerlist = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
	else
	{
		const uint8_t sendfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,sendfd_connected));
		const uint8_t recvfd_connected = getter_uint8(n,INT_MIN,-1,-1,offsetof(struct peer_list,recvfd_connected));
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
			gtk_widget_set_size_request(overlay2,(int)(size_peerlist_icon_size/1.5),(int)(size_peerlist_icon_size/1.5));
			gtk_widget_set_halign(overlay2, GTK_ALIGN_START);
			gtk_widget_set_valign(overlay2, GTK_ALIGN_END);
			GtkWidget *dot = gtk_image_new_from_paintable(GDK_PAINTABLE(dot_red));
			gtk_overlay_set_child(GTK_OVERLAY(overlay2),dot);

			char unread_count[21];
			snprintf(unread_count,sizeof(unread_count),"%lu",t_peer[n].unread);
			GtkWidget *unread_counter = gtk_label_new(unread_count);
			gtk_widget_add_css_class(unread_counter, "unread_count");
			gtk_widget_set_halign(unread_counter, GTK_ALIGN_CENTER);
			gtk_widget_set_valign(unread_counter, GTK_ALIGN_CENTER);
			gtk_overlay_add_overlay(GTK_OVERLAY(overlay2),unread_counter);

			gtk_overlay_add_overlay(GTK_OVERLAY(overlay),overlay2);
		}
	}
	else
		gtk_widget_set_size_request(image_peerlist,size_peerlist_icon_size/2,size_peerlist_icon_size/2);

	/* Assemble everything */
	gtk_box_append (GTK_BOX(chat_info), overlay); // For groups, we'll probably do 3 dots in a triangle. 1 Green for each Green user.

	/* Build Peernick Label */
	char peernick[56+1];
	getter_array(&peernick,sizeof(peernick),n,INT_MIN,-1,-1,offsetof(struct peer_list,peernick));
	GtkWidget *chat_name = gtk_label_new(peernick);
	sodium_memzero(peernick,sizeof(peernick));
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
			snprintf(last_message,sizeof(last_message),"%s%s",text_draft,text);
			g_free(text);
			text = NULL;
		}
		else
		{ // No draft
			int nn;
			int last_message_i = INT_MIN;
			for(int count_back = 0; (last_message_i = set_last_message(&nn,n,count_back)) > INT_MIN ; count_back++)
			{
				if(t_peer[nn].mute && getter_uint8(nn,INT_MIN,-1,-1,offsetof(struct peer_list,owner)) == ENUM_OWNER_GROUP_PEER)
					continue; // do not print, these are hidden messages from ignored users
				else
					break;
			}
			int p_iter;
			if(last_message_i > INT_MIN && (p_iter = getter_int(nn,last_message_i,-1,-1,offsetof(struct message_list,p_iter))) > -1)
			{
				const int max_i = getter_int(nn,INT_MIN,-1,-1,offsetof(struct peer_list,max_i));
				pthread_rwlock_rdlock(&mutex_protocols);
				const uint16_t protocol = protocols[p_iter].protocol;
				const uint8_t file_offer = protocols[p_iter].file_offer;
				const uint32_t null_terminated_len = protocols[p_iter].null_terminated_len;
				pthread_rwlock_unlock(&mutex_protocols);
				const char *message = getter_string(NULL,nn,last_message_i,-1,offsetof(struct message_list,message));
				if(max_i > INT_MIN/* && protocol > 0*/ && message)
				{
					int prefix = 0; // NOTE: could be larger than size due to weird way snprintf returns
					const uint8_t stat = getter_uint8(nn,last_message_i,-1,-1,offsetof(struct message_list,stat));
					if(stat == ENUM_MESSAGE_RECV && t_peer[n].unread > 0) // NOTE: This n needs to be n, not NN. n is the group_ctrl whereas nn is peer
					{}//	prefix = snprintf(last_message,sizeof(last_message),"🔔 "); // this bell is 4char
					else if(stat == ENUM_MESSAGE_FAIL && owner != ENUM_OWNER_GROUP_CTRL)
						prefix = snprintf(last_message,sizeof(last_message),"%s",text_queued);
					else if(stat != ENUM_MESSAGE_RECV)
						prefix = snprintf(last_message,sizeof(last_message),"%s",text_you);
					if(file_offer)
					{ // Last message is file offer
						int nnn = nn;
						if(protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP || protocol == ENUM_PROTOCOL_FILE_OFFER_GROUP_DATE_SIGNED)
						{
							const int g = set_g(nn,NULL);
							nnn = getter_group_int(g,offsetof(struct group_list,n));
						}
						const int f = set_f(nnn,(const unsigned char*)message,CHECKSUM_BIN_LEN);
						torx_read(nnn) // XXX
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",peer[nnn].file[f].filename);
						torx_unlock(nnn) // XXX
					}
					else if(null_terminated_len)
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",message);
					else if(protocol == ENUM_PROTOCOL_GROUP_OFFER || protocol == ENUM_PROTOCOL_GROUP_OFFER_FIRST)
						snprintf(&last_message[prefix],sizeof(last_message)-(size_t)prefix,"%s",text_group_offer);
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

	/* Append it to passed box */
//	gtk_box_append (GTK_BOX(widget), button_peer);
	return button_peer;
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
	GFile *path = g_file_new_for_uri("resource:///org/torx/gtk4/other/search_ico.png"); 
	GIcon *icon = g_file_icon_new(path);
	gtk_entry_set_icon_from_gicon(GTK_ENTRY(t_main.entry_search),GTK_ENTRY_ICON_SECONDARY, icon);
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

static int login_act_idle(void *arg) // g_idle_add() functions must return int
{
	const int value = vptoi(arg);
	if(value == 0)
		ui_show_main_screen(t_main.main_window);
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
	g_idle_add(login_act_idle,itovp(value));
}

static void ui_send_login(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;
	if(threadsafe_read_int8(&mutex_global_variable,(int8_t*)&lockout))
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

static void ui_show_auth_screen(void)
{
	t_main.window = window_auth; // Auth

//	t_main.auth_entry_pass = GTK_PASSWORD_ENTRY(gtk_password_entry_new());	// WORKS 
	t_main.auth_entry_pass = gtk_password_entry_new();				// WORKS
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
	t_main.auth_button = gtk_button_new_with_label (text_enter); // perhaps just leave this blank. people know what its there for.
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

static int icon_failure_idle(void *arg)
{
	const uint16_t icon_port = (uint16_t) vptoi(arg);
	error_printf(0,"Appindicator is NOT functioning on port %u",icon_port);
	appindicator_functioning = 0;
	return 0;
}

static int icon_communicator_idle(void *arg)
{
	const int sockfd = vptoi(arg);
	char message[256];
	int diff,str_len;
//	printf("Checkpoint data: %d = %lu - %lu\n",(int)totalUnreadPeer - (int)last_totalUnreadPeer,totalUnreadPeer,last_totalUnreadPeer);
	if ((diff = (int)totalUnreadPeer - (int)last_totalUnreadPeer) && (diff != 1 ? (str_len = snprintf(message,sizeof(message),"p%lu",totalUnreadPeer)) : (str_len = snprintf(message,sizeof(message),"p"))) > 0)
	{
	//	printf("Checkpoint awesome: %s\n",message);
		if(write(sockfd, message, (size_t)str_len+1) < 0)
			goto end;
		last_totalUnreadPeer = totalUnreadPeer;
	}
	if ((diff = (int)totalUnreadGroup - (int)last_totalUnreadGroup) && (diff != 1 ? (str_len = snprintf(message,sizeof(message),"g%lu",totalUnreadGroup)) : (str_len = snprintf(message,sizeof(message),"g"))) > 0)
	{
		if(write(sockfd, message, (size_t)str_len+1) < 0)
			goto end;
		last_totalUnreadGroup = totalUnreadGroup;
	}
	if ((diff = (int)totalIncoming - (int)last_totalIncoming) && (diff != 1 ? (str_len = snprintf(message,sizeof(message),"i%lu",totalIncoming)) : (str_len = snprintf(message,sizeof(message),"i"))) > 0)
	{
		if(write(sockfd, message, (size_t)str_len+1) < 0)
			goto end;
		last_totalIncoming = totalIncoming;
	}
	return TRUE;
	end: {}
	error_printf(0,"Icon_communicator failed after successful connection");
	if(sockfd > 0)
		close(sockfd);
	icon_failure_idle(arg);
	return FALSE;
}

static void *icon_communicator(void* arg)
{ // NOT IN UI THREAD, but is threadsafe because we don't access any GTK stuff directly
	pusher(zero_pthread,(void*)&thrd_start_tor)
	setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	const uint16_t icon_port = (uint16_t) vptoi(arg);
	int sockfd;
	struct sockaddr_in servaddr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		goto end;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(icon_port);
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
		goto end;
	int ret = -1;
	for (uint8_t attempts = ICON_COMMUNICATION_ATTEMPTS; attempts ; attempts--)
	{
		if ((ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == 0)
			break;
		if(attempts - 1) // don't sleep after last attempt
			sleep(1);
	}
	if(!ret)
	{ // Succssfully connected
		appindicator_functioning = 1; // yes, set here. safe to set despite threaded. no need idle or locks because icon_communicator_idle isn't started yet.
		g_timeout_add(500,icon_communicator_idle,itovp(sockfd));
		return 0;
	}
	end: {}
	error_printf(0,"Icon_communicator failed before successful connection");
	if(sockfd > 0)
		close(sockfd);
	g_idle_add(icon_failure_idle,arg); // XXX
	if(tray_pid > 0)
		kill(tray_pid,SIGTERM);
	pthread_exit(NULL);
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

	getcwd(starting_dir,sizeof(starting_dir));
	/* Options configurable by client */
	debug = 0;
	reduced_memory = 2; // TODO probably remove before release

	/* Utilizing setter functions instead of direct setting (ex: stream_registered = stream_cb_ui;) for typechecking */
	initialize_n_setter(initialize_n_cb_ui);
	initialize_i_setter(initialize_i_cb_ui);
	initialize_f_setter(initialize_f_cb_ui);
	initialize_g_setter(initialize_g_cb_ui);
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

	t_peer = torx_insecure_malloc(sizeof(struct t_peer_list) *11);

	initial();
//	protocol_registration(ENUM_PROTOCOL_TEST_STREAM,"Test stream data","",0,0,0,0,0,0,0,ENUM_EXCLUSIVE_GROUP_MSG,0,1,1);
	protocol_registration(ENUM_PROTOCOL_STICKER_HASH,"Sticker","",0,0,0,1,1,0,0,ENUM_EXCLUSIVE_GROUP_MSG,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_HASH_DATE_SIGNED,"Sticker Date Signed","",0,2*sizeof(uint32_t),crypto_sign_BYTES,1,1,0,0,ENUM_EXCLUSIVE_GROUP_MSG,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_HASH_PRIVATE,"Sticker Private","",0,0,0,1,1,0,0,ENUM_EXCLUSIVE_GROUP_PM,0,1,0);
	protocol_registration(ENUM_PROTOCOL_STICKER_REQUEST,"Sticker Request","",0,0,0,0,0,0,0,ENUM_EXCLUSIVE_NONE,0,1,1); // NOTE: if making !stream, need to move related handler from stream_cb to print_message_idle
	protocol_registration(ENUM_PROTOCOL_STICKER_DATA_GIF,"Sticker data","",0,0,0,0,0,0,0,ENUM_EXCLUSIVE_NONE,0,1,1); // NOTE: if making !stream, need to move related handler from stream_cb to print_message_idle
	snowflake_location = which("snowflake-client"); // TODO move to initial_keyed?
	obfs4proxy_location = which("obfs4proxy"); // TODO move to initial_keyed?

	const char *tdd = "tor_data_directory";
	char current_working_directory[PATH_MAX];
	getcwd(current_working_directory,sizeof(current_working_directory));
	char tdd_full_path[PATH_MAX];
	const int tdd_len = snprintf(tdd_full_path,sizeof(tdd_full_path),"%s%c%s",current_working_directory,platform_slash,tdd) + 1;
	if(tdd_len > 1)
	{
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
	if(!get_file_size(FILENAME_BEEP))
	{
		GBytes *bytes = g_resources_lookup_data("/org/torx/gtk4/other/beep.wav",G_RESOURCE_LOOKUP_FLAGS_NONE,NULL);
		size_t size = 0;
		const void *data = g_bytes_get_data(bytes,&size);
		write_bytes(FILENAME_BEEP,data,size);
	}

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
	fail_ico = gdk_texture_new_from_resource("/org/torx/gtk4/other/fail.png");
	add_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/add_active.png");
	add_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/add.png");
	add_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/add_light.png");
	home_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/home_active.png");
	home_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/home.png");
	home_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/home_light.png");
	settings_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/setting_active.png");
	settings_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/settings.png");
	settings_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/setting_light.png");
	file_file = gdk_texture_new_from_resource("/org/torx/gtk4/other/file.png");
	file_txt = gdk_texture_new_from_resource("/org/torx/gtk4/other/txt.png");
	file_image = gdk_texture_new_from_resource("/org/torx/gtk4/other/image.png");
	file_music = gdk_texture_new_from_resource("/org/torx/gtk4/other/music.png");
	file_zip = gdk_texture_new_from_resource("/org/torx/gtk4/other/zip.png");
	dot_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/red.png");
	dot_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/green.png");
	dot_yellow = gdk_texture_new_from_resource("/org/torx/gtk4/other/yellow.png");
	dot_orange = gdk_texture_new_from_resource("/org/torx/gtk4/other/orange.png");
	dot_grey = gdk_texture_new_from_resource("/org/torx/gtk4/other/grey.png");
	lock_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_red.png");
	lock_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_green.png");
	lock_yellow = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_yellow.png");
	lock_orange = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_orange.png");
	lock_grey = gdk_texture_new_from_resource("/org/torx/gtk4/other/lock_grey.png");
	unlock_red = gdk_texture_new_from_resource("/org/torx/gtk4/other/unlock_red.png");
	unlock_green = gdk_texture_new_from_resource("/org/torx/gtk4/other/unlock_green.png");
	unlock_yellow = gdk_texture_new_from_resource("/org/torx/gtk4/other/unlock_yellow.png");
	unlock_orange = gdk_texture_new_from_resource("/org/torx/gtk4/other/unlock_orange.png");
	unlock_grey = gdk_texture_new_from_resource("/org/torx/gtk4/other/unlock_grey.png");
	logging_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/logging_active.png");
	logging_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/logging.png");
	logging_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/logging_light.png");
	globe_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/globe_active.png");
	globe_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/globe.png");
	globe_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/globe_light.png");
	bell_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/bell_active.png");
	bell_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/bell.png");
	bell_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/bell_light.png");
	block_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/block_active.png");
	block_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/block.png");
	block_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/block_light.png");
	kill_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/kill_active.png");
	kill_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/kill.png");
	kill_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/kill_light.png");
	delete_active = gdk_texture_new_from_resource("/org/torx/gtk4/other/delete_active.png");
	delete_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/delete.png");
	delete_inactive_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/delete_light.png");
	emoji_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/sticker_light.png");
	emoji_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/sticker_dark.png");
//	send_inactive = gdk_texture_new_from_resource("/org/torx/gtk4/other/send_inactive.png");
	attach_light = gdk_texture_new_from_resource("/org/torx/gtk4/other/attach_img_light.png");
	attach_dark = gdk_texture_new_from_resource("/org/torx/gtk4/other/attach_img_dark.png");

	ui_show_auth_screen();
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

	const int8_t lockout_local = threadsafe_read_int8(&mutex_global_variable,(int8_t*)&lockout);
	if(no_password && !lockout_local) // UI setting, relevant to first_run only
		login_start("");

	if(lockout_local)
		gtk_button_set_label(GTK_BUTTON(t_main.auth_button),text_wait);

	#ifdef ENABLE_APPINDICATOR
	const uint16_t icon_port = randport(0);
	char port_array[6];
	snprintf(port_array,sizeof(port_array),"%u",icon_port);
	#ifdef WIN32
	if(!CreateProcess(NULL,"torx-tray",NULL,NULL,TRUE,0,NULL,NULL,NULL,NULL))
	{ // after checking PATH, assuming this isn't running in GDB
		char binary_path_copy[PATH_MAX];
		snprintf(binary_path_copy,sizeof(binary_path_copy),"%s",binary_path);
		char *current_binary_directory = dirname(binary_path_copy); // NECESSARY TO COPY
		char appindicator_path[PATH_MAX];
		snprintf(appindicator_path,sizeof(appindicator_path),"%s\\%s\\torx-tray",starting_dir,current_binary_directory);
		if(!CreateProcess(NULL,appindicator_path,NULL,NULL,TRUE,0,NULL,NULL,NULL,NULL))
		{ // try for GDB
			snprintf(appindicator_path,sizeof(appindicator_path),"%s\\torx-tray",current_binary_directory);
			if(!CreateProcess(NULL,appindicator_path,NULL,NULL,TRUE,0,NULL,NULL,NULL,NULL))
				error_printf(0,"Failed to start appindicator on port %s\n",port_array);
		}
	}
	#else
	if((tray_pid = fork()) == -1)
		error_simple(-1,"fork");
	if(tray_pid == 0)
	{ // Check in path before attempting to check from directory we run from
		if(execlp("torx-tray","torx-tray","-p",port_array,"-P",binary_path,NULL))
		{ // after checking PATH, assuming this isn't running in GDB
			char binary_path_copy[PATH_MAX];
			snprintf(binary_path_copy,sizeof(binary_path_copy),"%s",binary_path);
			char *current_binary_directory = dirname(binary_path_copy); // NECESSARY TO COPY
			char appindicator_path[PATH_MAX];
			snprintf(appindicator_path,sizeof(appindicator_path),"%s%c%s%ctorx-tray",starting_dir,platform_slash,current_binary_directory,platform_slash);
			if(execl(appindicator_path,"torx-tray","-p",port_array,"-P",binary_path,NULL))
			{ // try for GDB
				snprintf(appindicator_path,sizeof(appindicator_path),"%s%ctorx-tray",current_binary_directory,platform_slash);
				if(execl(appindicator_path,"torx-tray","-p",port_array,"-P",binary_path,NULL))
					error_printf(0,"Failed to start appindicator on port %s\n",port_array);
			}
		}
		exit(0);
	}
	#endif
	if(pthread_create(&thread_icon_communicator,&ATTR_DETACHED,&icon_communicator,itovp(icon_port)))
		error_simple(0,"Failed to create thread");
	#endif
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
	binary_path = argv[0];
	binary_name = argv[1];

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
