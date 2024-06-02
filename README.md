## TorX GTK4 Client (torx-gtk4)
This page is for developers and contributors. If you are simply looking to download and run TorX, go to [TorX.Chat](https://torx.chat)

#### Build Instructions:
####### Linux:
######### Install build dependencies:
sudo apt install libgtk-4-dev libsodium-dev libevent-dev sqlcipher build-essential

######### For building TorX normally:
cmake -B build && cd build && make && cd ..  && ./build/torx-gtk4

######### For building TorX with debug symbols, for gdb:
cmake -DCMAKE_BUILD_TYPE=Debug -B build && cd build && make && cd ..  && export G_DEBUG=fatal-criticals && gdb -ex "set print thread-events off" -ex "break breakpoint" -ex run ./build/torx-gtk4

######### For building TorX with debug symbols, for valgrind:
cmake -DCMAKE_BUILD_TYPE=Debug -B build && cd build && make && cd ..  && valgrind --track-origins=yes --leak-check=full ./build/torx-gtk4

####### OSX:
See Linux instructions, then modify as appropriate. CMakeLists.txt may need modifications. When successful, contact us so that we can add instructions.

####### Windows:
2024/03/31 See "build_windows.txt". Currently does NOT build because of pipe() calls that need to be replaced
2022/??/?? cannot use popen https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/popen-wpopen?redirectedfrom=MSDN&view=msvc-170 but windows has its own CreatePipe that is availablehttps://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN

#### Contribution Agreement:
All ideas, suggestions, issues, pull requests, etc, are gifted to the primary developer for the purpose of improving the software, for the benefit of all users. Ownership of the contribution is not maintained by the contributor.

#### TODO List
####### Tasks common with flutter
2024/02/25 we should probably have a tooltip that shows TorX-IDs, at least on the group's peerlist
Flash entry box red when no nick is entered for generating (generate_onion) or adding onion (save_peer), or when an invalid onion is passed (bad checksum)
Visual callback for invalid or incomplete. Minor notice for incomplete, major for invalid + counter.
Multi-Select -- Tables should allow multiple selections for deletion. It should hide Show QR and Copy (umm, ok maybe not) showing only Delete (or Accept + Reject)

####### Tasks Unique to GTK
2024/05/17 Images should be clickable (to show larger), should automatically render after reaching 100% transferred (inbound). Stickers + Gifs are a problem if corrupt (because we don't have sufficient sanity checks). Other corrupt images are fine.
2024/04/30 Implement _from_i functions in Gtk/Lib
2024/03/01 *** threadsafety: we have a lot of places where we retrieve pointer locations with locks and then read them without locks. In the vast majority of cases, this will never be a problem, but we should implement getter_string for these cases to eliminate the data race warnings, despite the performance implications. (ctrl+f &mutex_ and torx_unlock, replace with getter_string)
2023/11/22 Generating private group after public group does not hide the existing public group's qr code
2024/02/03 have clang specific compile flags. could add more.
save_dir_always_ask needs toggle
Memory leakage: Free memory. Eliminate memory leaks so that it can valgrind 0. g_object_free()? Unsure how to handle sensitive things.
consider registering a URI?
figure out how to build for windows, which might involve compiling in to a single exe.
consider putting Send button below the emoji and attach buttons.
Pixbuf functions with SVG. Should convert PNG to SVG.
Gnotification can't use gresource icon. That is OK because we need to have a .png for our .desktop file. If we set icon_logo to NULL, the icon registered with DBUS by .desktop will be used.
Change_password and Add/Generate pages need max length for entry fields
Add a dropdown list of options for key macros for "Send". See function keypress().
"if we can get a GtkIconPaintable from .SVG, we can create a symbolic icon, then it can be colored with CSS "color" property"
Bug: Tor log scrolls even if not at the bottom. Should check scroll position before scrolling. Might effect TorX log and Chats too? (note: we might have already fixed this and have a unified scroll function)
Tooltips: Use text_tooltip_ prefix. For flutter, https://material.io/components/tooltips
we don't have an icon in get_icon_from_filename() for video. perhaps on top of file.png we can just overlay the filetype like in txt.png, but in the client?

####### Post-release
2024/03/29 messages larger than the visible window permanently mess up the viewport or scrolled box, causing inability to scroll to the bottom. idk why.
2024/03/21 GStreamer or PortAudio library ( https://github.com/PortAudio/portaudio ) to record .wav type audio for voice messages / streaming chat / walkie mode. Notes elsewhere suggest Vorbis
2024/03/21 Facilitate msg searching via gtk_list_view_scroll_to.
2024/03/22 Colored .svg icons will be available from GTK 4.14. Then search our code for gtk_symbolic_paintable_snapshot_symbolic, where we have an example implementation ready.
2024/03/28 Review any _swapped( and instead set proper args for given ::signal (note: if signal can't be found in gtk docs, check gobject docs)
2024/04/11 String functions in UI should use secure allocators/de-allocators, if possible.
