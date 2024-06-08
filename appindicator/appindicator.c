#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <libgen.h>	// for basename
#include "torx-tray.c"	// gresource

/* TODO
	- The following causes crashes:
		(torx-icon:49906): Gdk-CRITICAL **: 14:06:23.621: gdk_window_thaw_toplevel_updates: assertion 'window->update_and_descendants_freeze_count > 0' failed
	- GTK docs say it might support windows https://docs.gtk.org/gtk3/class.StatusIcon.html
	- exit(EXIT_SUCCESS) means port is unavailable:
		Try again.
	- exit(EXIT_FAILURE) means status icons are unavailable:
		Have parent set daemonize_upon_close to 0 and disable the toggle.

NOTES:
	We are now threadsafe!
*/

#define TITLE "TorX"
#define HIDE_AFTER_CLICK 0
#define BLINK_TIME_MS 800

#define UNREAD_PEER	'p' // p20 to set, p0 to clear, p to iterate++ (includes blink)
#define UNREAD_GROUP	'g' // g20 to set, g0 to clear, g to iterate++ (includes blink)
#define UNREAD_INCOMING	'i' // i20 to set, i0 to clear, i to iterate++ (includes blink)
#define BLINK		'b' // b20 to set, b0 to clear, b to iterate++
#define SHOW_ICON	's' // s to show
#define HIDE_ICON	'h' // h to hide

struct data {
	GtkStatusIcon *tray_icon;
	GIcon *icon_logo_default;
	GIcon *icon_logo_transparent;
	GIcon *icon_logo_unread;
	uint32_t unread_peer;
	uint32_t unread_group;
	uint32_t incoming;
	uint16_t port;
	char *path;
	char *tooltip;
	gboolean transparent;
	gboolean visibility;
	uint32_t blinks_left;
	uint32_t fail_count;
};

struct fresh_data {
	struct data *data;
	char *buffer;
	size_t bytes_recieved;
};

static int rebuild(void *arg)
{
	printf("Checkpoint rebuilding\n");
	struct data *data = (struct data*) arg;
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	if(data->transparent)
	{
		gtk_status_icon_set_from_gicon(data->tray_icon,data->icon_logo_transparent);
		data->transparent = FALSE;
		printf("Checkpoint transparent false\n");
	}
	else if(data->unread_peer + data->unread_group + data->incoming)
		gtk_status_icon_set_from_gicon(data->tray_icon,data->icon_logo_unread);
	else
		gtk_status_icon_set_from_gicon(data->tray_icon,data->icon_logo_default);
	if(data->tooltip)
	{
		free(data->tooltip);
		data->tooltip = NULL;
	}
	if(data->unread_peer + data->unread_group + data->incoming)
	{
		data->tooltip = malloc(128);
		int pos = 0;
		if(data->unread_peer)
			pos += snprintf(&data->tooltip[pos],128-(size_t)pos,"Unread Peer: %u\n",data->unread_peer);
		if(data->unread_group)
			pos += snprintf(&data->tooltip[pos],128-(size_t)pos,"Unread Group: %u\n",data->unread_group);
		if(data->incoming)
			pos += snprintf(&data->tooltip[pos],128-(size_t)pos,"Friend Requests: %u\n",data->incoming);
		data->tooltip[pos-1] = '\0'; // remove trailing newline, from whatever line printed
	}
	else
	{
		data->tooltip = malloc(sizeof(TITLE));
		snprintf(data->tooltip,sizeof(TITLE),"%s",TITLE);
	}
	gtk_status_icon_set_tooltip_text(data->tray_icon,data->tooltip);
	gtk_status_icon_set_visible(data->tray_icon,data->visibility);
	if(!gtk_status_icon_is_embedded(data->tray_icon) && data->fail_count++ > 1) // gtk_status_icon_is_embedded is correct, not gtk_status_icon_get_visible, according to docs
	{ // note: its normal to fail once on startup
		fprintf(stderr,"Checkpoint status icon: gtk_status_icon_is_embedded failed more than permitted.\n");
		exit(EXIT_FAILURE); // status icons not supported by GUI
	}
	G_GNUC_END_IGNORE_DEPRECATIONS
	return FALSE;
}

static int handle_fresh_data(void *arg)
{
	struct fresh_data *fresh_data = (struct fresh_data*) arg;
	struct data *data = fresh_data->data;
	char *buffer = fresh_data->buffer;
	const size_t bytes_recieved = fresh_data->bytes_recieved;
	for(size_t iter = 0,str_len = strlen(buffer); iter + str_len <= bytes_recieved - 1; str_len = strlen(&buffer[(iter += str_len+1)]))
	{
		gboolean iterate = FALSE;
		uint32_t requested = 0;
		if(str_len > 1)
			requested = (uint32_t)atoll(&buffer[iter+1]);
		else
			iterate = TRUE;
		switch (buffer[iter+0])
		{
			case UNREAD_PEER:
				if(iterate)
				{
					data->unread_peer++;
					data->blinks_left++;
				}
				else
				{
					data->unread_peer = requested;
					rebuild(data);
				}
				break;
			case UNREAD_GROUP:
				if(iterate)
				{
					data->unread_group++;
					data->blinks_left++;
				}
				else
				{
					data->unread_group = requested;
					rebuild(data);
				}
				break;
			case UNREAD_INCOMING:
				if(iterate)
				{
					data->incoming++;
					data->blinks_left++;
				}
				else
				{
					data->incoming = requested;
					rebuild(data);
				}
				break;
			case BLINK:
				if(iterate)
					data->blinks_left++;
				else
					data->blinks_left = requested;
				break;
			case SHOW_ICON:
				data->visibility = TRUE;
				rebuild(data);
				break;
			case HIDE_ICON:
				data->visibility = FALSE;
				rebuild(data);
				break;
			default:
				fprintf(stderr,"Checkpoint unknown option passed: %c\n",buffer[iter+0]);
				break;
		}
		if(iter + str_len == (size_t)bytes_recieved - 1)
			break; // reached the end of our packet. avoid illegal reads into buffer by strlen call.
	}
	free(fresh_data->buffer);
	fresh_data->buffer = NULL;
	free(arg);
	arg = NULL;
	return FALSE;
}

static void *start_listener(void *arg)
{
	fprintf(stderr,"Checkpoint status icon: starting listener1.\n");
	struct data *data = (struct data*) arg;
	const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		fprintf(stderr,"Checkpoint status icon: failed to get socket.\n");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in serv_addr, cli_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(data->port);
	fprintf(stderr,"Checkpoint status icon: starting listener2.\n");
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr,"Checkpoint status icon: failed to bind. Port must be in use..\n");
		exit(EXIT_SUCCESS); // success because we want to distinguish unavailable port from broader unavailability
	}
	listen(sockfd, 5);
	socklen_t clilen = sizeof(cli_addr);
	fprintf(stderr,"Checkpoint status icon: starting listener3.\n");
	const int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
	{
		fprintf(stderr,"Checkpoint status icon: failed to accept.\n");
		exit(EXIT_FAILURE);
	}
	char buffer[256];
	while (1)
	{
		fprintf(stderr,"Checkpoint status icon: starting listener5.\n");
		const ssize_t bytes_recieved = read(newsockfd, buffer, sizeof(buffer)-1);
		fprintf(stderr,"Checkpoint status icon: starting listener6.\n");
		if(bytes_recieved < 1)
			break; // parent closed
		buffer[bytes_recieved] = '\0';

		struct fresh_data *fresh_data = malloc(sizeof(struct fresh_data));
		fresh_data->data = data;
		fresh_data->buffer = malloc((size_t)bytes_recieved+1);
		memcpy(fresh_data->buffer,buffer,(size_t)bytes_recieved+1);
		fresh_data->bytes_recieved = (size_t)bytes_recieved;

		g_idle_add(handle_fresh_data, fresh_data);
	}
	close(newsockfd);
	close(sockfd);
	fprintf(stderr,"Checkpoint status icon: parent software closed connection.\n");
	exit(EXIT_SUCCESS); // parent closed
}

static int blinker_idle(void *arg)
{ // We have a dedicated blinker timer in UI thread to keep the timing consistent.
	struct data *data = (struct data*) arg;
	if(data->blinks_left > 0)
	{
		data->blinks_left--;
		data->transparent = TRUE;
		printf("Checkpoint transparent true\n");
		rebuild(data);
		g_timeout_add(BLINK_TIME_MS,rebuild,data);
	}
	return TRUE; // repeat forever
}

static void status_icon_activate(GtkStatusIcon *tray_icon, void *arg)
{ // Occurs upon click of tray icon
	(void) tray_icon;
	struct data *data = (struct data*) arg;
	if(HIDE_AFTER_CLICK)
	{
		data->visibility = FALSE;
		rebuild(data);
	}
	if(fork() < 1)
	{
		execl(data->path,basename(data->path),NULL); // note: different memory space, safe to call basename without copying here
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char *argv[]) {
	signal(SIGCHLD, SIG_IGN); // XXX prevent zombies
	// Initialize GTK
	gtk_init(&argc, &argv);

	struct data *data = malloc(sizeof(struct data));
	data->port = 0;
	data->path = NULL;

	for(int opt; (opt = getopt(argc, argv, "p:P:")) != -1;)
	{
		switch (opt)
		{
			case 'p':
				data->port = (uint16_t)atoi(optarg);
				break;
			case 'P':
				data->path = optarg;
				break;
			default:
				break;
		}
	}
	if(data->port < 1024 || !data->path)
	{
		fprintf(stderr, "Usage: %s -p port -P path\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	G_GNUC_BEGIN_IGNORE_DEPRECATIONS

	GtkStatusIcon *tray_icon = gtk_status_icon_new();

//	gtk_status_icon_set_title(tray_icon,"test1"); // Serves no purpose
//	gtk_status_icon_set_name(tray_icon,"test2"); // Serves no purpose // DO NOT SET, causes a critical that was really hard to track down https://github.com/dorkbox/SystemTray/issues/19

	GFile *file_default = g_file_new_for_uri("resource:///org/torx/tray/icon_square.png");
	GFile *file_transparent = g_file_new_for_uri("resource:///org/torx/tray/logo-torx-symbolic.svg");
	GFile *file_unread = g_file_new_for_uri("resource:///org/torx/tray/icon_square_unread.png");

	data->icon_logo_default = g_file_icon_new (file_default);;
	data->icon_logo_transparent = g_file_icon_new (file_transparent);;
	data->icon_logo_unread = g_file_icon_new (file_unread);;

	G_GNUC_END_IGNORE_DEPRECATIONS

	g_signal_connect(tray_icon, "activate", G_CALLBACK(status_icon_activate), data);

	data->tray_icon = tray_icon;
	data->unread_peer = 0;
	data->unread_group = 0;
	data->incoming = 0;
	data->transparent = FALSE;
	data->tooltip = NULL;
	data->visibility = TRUE; // must default to true
	data->blinks_left = 0;
	data->fail_count = 0;

	rebuild(data);

	pthread_t thread1 = {0};
	if(pthread_create(&thread1,NULL,&start_listener,data))
		fprintf(stderr,"Failed to create thread1\n");

	g_timeout_add(BLINK_TIME_MS,blinker_idle, data);

	gtk_main();

	return 0;
}
