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
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <libgen.h>	// for basename
#include <unistd.h>
#include <event2/event.h> // for evutil_socket_t

#ifdef WIN32
#define SOCKET_CAST_IN (evutil_socket_t)
#define SOCKET_CAST_OUT (SOCKET)
#else
#define SOCKET_CAST_IN
#define SOCKET_CAST_OUT
#endif

#ifdef WIN32 // XXX
#include <winsock2.h> // necessary
#include <ws2tcpip.h> // necessary
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

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
	struct data *data = (struct data*) arg;
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	if(data->transparent)
	{
		gtk_status_icon_set_from_gicon(data->tray_icon,data->icon_logo_transparent);
		data->transparent = FALSE;
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
	struct data *data = (struct data*) arg;
	const evutil_socket_t sockfd = SOCKET_CAST_IN socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		fprintf(stderr,"Checkpoint status icon: failed to get socket.\n");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in serv_addr, cli_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serv_addr.sin_port = htons(data->port);
	if (bind(SOCKET_CAST_OUT sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr,"Checkpoint status icon: failed to bind. Port must be in use..\n");
		exit(EXIT_SUCCESS); // success because we want to distinguish unavailable port from broader unavailability
	}
	listen(SOCKET_CAST_OUT sockfd, 5);
	socklen_t clilen = sizeof(cli_addr);
	const evutil_socket_t newsockfd = SOCKET_CAST_IN accept(SOCKET_CAST_OUT sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
	{
		fprintf(stderr,"Checkpoint status icon: failed to accept.\n");
		exit(EXIT_FAILURE);
	}
	char buffer[256];
	while (1)
	{
	//	fprintf(stderr,"Checkpoint status icon: Starting to recv\n");
		const ssize_t bytes_recieved = recv(SOCKET_CAST_OUT newsockfd, buffer, sizeof(buffer)-1,0);
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
	evutil_closesocket(newsockfd);
	evutil_closesocket(sockfd);
//	fprintf(stderr,"Checkpoint status icon: parent software closed connection.\n");
	exit(EXIT_SUCCESS); // parent closed
}

static int blinker_idle(void *arg)
{ // We have a dedicated blinker timer in UI thread to keep the timing consistent.
	struct data *data = (struct data*) arg;
	if(data->blinks_left > 0)
	{
		data->blinks_left--;
		data->transparent = TRUE;
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
	#ifdef WIN32
	STARTUPINFO siStartInfo;
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION process_info;
	if(!CreateProcess(NULL,data->path,NULL,NULL,TRUE,0,NULL,NULL,&siStartInfo,&process_info))
	{
		fprintf(stderr,"Failed to activate parent program. Possibly a bad path: %s\n",data->path);
	/*	char swapping_slashes[PATH_MAX];
		snprintf(swapping_slashes,sizeof(swapping_slashes),"%s",data->path);
		for(int i = 0; swapping_slashes[i] != '\0'; i++)
			if(swapping_slashes[i] == '\\')
				swapping_slashes[i] = '/';
			else if(swapping_slashes[i] == '/')
				swapping_slashes[i] = '\\';
		if(!CreateProcess(NULL,swapping_slashes,NULL,NULL,TRUE,0,NULL,NULL,&siStartInfo,&process_info))
			fprintf(stderr,"Failed to activate parent program. Possibly a bad path: %s\n",swapping_slashes); */
	}
	#else
	if(fork() == 0)
	{
		if(execl(data->path,basename(data->path),NULL)) // note: different memory space, safe to call basename without copying here
			fprintf(stderr,"Failed to activate parent program. Possibly a bad path: %s\n",data->path);
		exit(EXIT_SUCCESS);
	}
	#endif
}

int main(int argc, char *argv[]) {
	#ifndef WIN32
	signal(SIGCHLD, SIG_IGN); // XXX prevent zombies
	#endif
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

	data->icon_logo_default = g_file_icon_new (file_default);
	data->icon_logo_transparent = g_file_icon_new (file_transparent);
	data->icon_logo_unread = g_file_icon_new (file_unread);

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
