<img alt="Logo" width="256" height="256" src="https://raw.githubusercontent.com/TorX-Chat/torx-gtk4/main/other/scalable/apps/logo-torx-symbolic.svg" align="right" style="position: relative; top: 0; left: 0;">

### TorX GTK4 Client (torx-gtk4)
This page is for developers and contributors. If you are simply looking to download and run TorX, go to [TorX.Chat](https://torx.chat/#download)

#### Build Instructions:
##### Linux:
NOTICE: GTK4 version >= 4.10 required (libgtk-4-dev). If you are using Debian, you need Trixie (Debian 13) repositories or above. For Ubuntu, this means Ubuntu Mantic (23) repositories or above. The reason is that the GTK project implemented major breaking changes to GTK4 at version 4.10, and we are early adoptors of those changes.
<br>If you want to try on a LiveCD, <a href="https://get.debian.org/images/weekly-live-builds/amd64/iso-hybrid/">here are some Trixie liveCDs</a>

###### Install build dependencies:
`sudo apt install git cmake pkg-config libgtk-3-dev libgtk-4-dev libsodium-dev libevent-dev libsqlcipher-dev build-essential`

###### Clone the repository
`git clone https://github.com/TorX-Chat/torx-gtk4 && cd torx-gtk`

###### For building TorX normally:
`cmake -B build && cd build && make && cd ..  && ./build/torx-gtk4`

###### For building TorX with debug symbols, for gdb:
`cmake -DCMAKE_BUILD_TYPE=Debug -B build && cd build && make && cd ..  && export G_DEBUG=fatal-criticals && gdb -ex "set print thread-events off" -ex "break breakpoint" -ex run ./build/torx-gtk4`

###### For building TorX with debug symbols, for valgrind:
`cmake -DCMAKE_BUILD_TYPE=Debug -B build && cd build && make && cd ..  && valgrind --track-origins=yes --leak-check=full ./build/torx-gtk4`

##### OSX:
See Linux instructions, then modify as appropriate. CMakeLists.txt may need modifications. When successful, contact us so that we can add instructions.

##### Windows:
NOTICE: Currently does NOT build because of pipe() calls that need to be replaced (with CreatePipe?), but you can try anyway.
<br><a href="https://www.msys2.org/">Install MSYS2</a> then open a terminal by clicking "MSYS2 MINGW64"
`pacman -Syu && exit`
`pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk4 mingw-w64-x86_64-libsodium mingw-w64-x86_64-libevent mingw-w64-x86_64-sqlcipher mingw-w64-x86_64-cmake mingw-w64-x86_64-toolchain base-devel`
`git clone https://github.com/TorX-Chat/torx-gtk4 && cd torx-gtk`
`cmake -G "Unix Makefiles" -B build/ && cd build && make clean && make`

#### Contribution Agreement:
All ideas, suggestions, issues, pull requests, contributions of any kind, etc, are gifted to the original TorX developer without condition nor consideration, for the purpose of improving the software, for the benefit of all users, current and future.

#### Screenshots:
<a href="https://torx-chat.github.io/images/desktop_auth_screen.png"><img src="https://torx-chat.github.io/images/desktop_auth_screen.png" alt="Screenshot" style="max-height:400px;"></a>
<a href="https://torx-chat.github.io/images/desktop_add.png"><img src="https://torx-chat.github.io/images/desktop_add.png" alt="Screenshot" style="max-height:400px;"></a>
<a href="https://torx-chat.github.io/images/desktop_home.png"><img src="https://torx-chat.github.io/images/desktop_home.png" alt="Screenshot" style="max-height:400px;"></a>
<a href="https://torx-chat.github.io/images/desktop_grandma.png"><img src="https://torx-chat.github.io/images/desktop_grandma.png" alt="Screenshot" style="max-height:400px;"></a>
<a href="https://torx-chat.github.io/images/desktop_add_group.png"><img src="https://torx-chat.github.io/images/desktop_add_group.png" alt="Screenshot" style="max-height:400px;"></a>
<a href="https://torx-chat.github.io/images/desktop_group.png"><img src="https://torx-chat.github.io/images/desktop_group.png" alt="Screenshot" style="max-height:400px;"></a>
<a href="https://torx-chat.github.io/images/desktop_settings.png"><img src="https://torx-chat.github.io/images/desktop_settings.png" alt="Screenshot" style="max-height:400px;"></a>

#### Legacy TODO List (needs review / cleanup)
###### Tasks common with flutter
2024/02/25 we should probably have a tooltip that shows TorX-IDs, at least on the group's peerlist
<br>2023/??/?? Flash entry box red when no nick is entered for generating (generate_onion) or adding onion (save_peer), or when an invalid onion is passed (bad checksum)
<br>2023/??/?? Visual callback for invalid or incomplete. Minor notice for incomplete, major for invalid + counter.
<br>2023/??/?? Multi-Select -- Tables should allow multiple selections for deletion. It should hide Show QR and Copy (umm, ok maybe not) showing only Delete (or Accept + Reject)

###### Tasks Unique to GTK
2024/05/17 Images should be clickable (to show larger), should automatically render after reaching 100% transferred (inbound). Stickers + Gifs are a problem if corrupt (because we don't have sufficient sanity checks). Other corrupt images are fine.
<br>2024/04/30 Implement _from_i functions in Gtk/Lib
<br>2024/03/01 *** threadsafety: we have a lot of places where we retrieve pointer locations with locks and then read them without locks. In the vast majority of cases, this will never be a problem, but we should implement getter_string for these cases to eliminate the data race warnings, despite the performance implications. (ctrl+f &mutex_ and torx_unlock, replace with getter_string)
<br>2023/11/22 Generating private group after public group does not hide the existing public group's qr code
<br>2024/02/03 have clang specific compile flags. could add more.
<br>2023/??/?? save_dir_always_ask needs toggle
<br>2023/??/?? Memory leakage: Free memory. Eliminate memory leaks so that it can valgrind 0. g_object_free()? Unsure how to handle sensitive things.
<br>2023/??/?? consider registering a URI?
<br>2023/??/?? figure out how to build for windows, which might involve compiling in to a single exe.
<br>2023/??/?? consider putting Send button below the emoji and attach buttons.
<br>2023/??/?? Pixbuf functions with SVG. Should convert PNG to SVG.
<br>2023/??/?? Gnotification can't use gresource icon. That is OK because we need to have a .png for our .desktop file. If we set icon_logo to NULL, the icon registered with DBUS by .desktop will be used.
<br>2023/??/?? Change_password and Add/Generate pages need max length for entry fields
<br>2023/??/?? Add a dropdown list of options for key macros for "Send". See function keypress().
<br>2023/??/?? "if we can get a GtkIconPaintable from .SVG, we can create a symbolic icon, then it can be colored with CSS "color" property"
<br>2023/??/?? Bug: Tor log scrolls even if not at the bottom. Should check scroll position before scrolling. Might effect TorX log and Chats too? (note: we might have already fixed this and have a unified scroll function)
<br>2023/??/?? Tooltips: Use text_tooltip_ prefix. For flutter, https://material.io/components/tooltips
<br>2023/??/?? we don't have an icon in get_icon_from_filename() for video. perhaps on top of file.png we can just overlay the filetype like in txt.png, but in the client?

###### Post-release
2024/03/29 messages larger than the visible window permanently mess up the viewport or scrolled box, causing inability to scroll to the bottom. idk why.
<br>2024/03/21 GStreamer or PortAudio library ( https://github.com/PortAudio/portaudio ) to record .wav type audio for voice messages / streaming chat / walkie mode. Notes elsewhere suggest Vorbis
<br>2024/03/21 Facilitate msg searching via gtk_list_view_scroll_to.
<br>2024/03/22 Colored .svg icons will be available from GTK 4.14. Then search our code for gtk_symbolic_paintable_snapshot_symbolic, where we have an example implementation ready.
<br>2024/03/28 Review any _swapped( and instead set proper args for given ::signal (note: if signal can't be found in gtk docs, check gobject docs)
<br>2024/04/11 String functions in UI should use secure allocators/de-allocators, if possible.
