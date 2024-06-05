<img alt="Logo" width="200" height="200" src="https://raw.githubusercontent.com/TorX-Chat/torx-gtk4/main/other/scalable/apps/logo-torx-symbolic.svg" align="right" style="position: relative; top: 0; left: 0;">

### TorX GTK4 Client (torx-gtk4)
This page is primarily for developers and contributors.
If you are simply looking to download and run TorX, go to [Download](https://torx.chat/#download)
If you want to contribute, see [Contribute](https://torx.chat/#contribute) and our [TODO Lists](https://torx.chat/todo.html)

#### Build Instructions:
##### Linux:
NOTICE: GTK4 version >= 4.10 required (libgtk-4-dev). The GTK project implemented major breaking changes to GTK4 at version 4.10, and we are early adopters of those changes.
<br>If you are using Debian, you need Trixie (Debian 13) repositories or above.
<br>If you are using Ubuntu, this means Mantic (Ubuntu 23) repositories or above.
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
NOTICE: Currently does NOT build because of fork() calls that need to be replaced, but you can try anyway.
<br><a href="https://www.msys2.org/">Install MSYS2</a> then open a terminal by clicking "MSYS2 MINGW64"
```
pacman -Syu && exit
pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk4 mingw-w64-x86_64-libsodium mingw-w64-x86_64-libevent mingw-w64-x86_64-sqlcipher mingw-w64-x86_64-cmake mingw-w64-x86_64-toolchain base-devel
git clone https://github.com/TorX-Chat/torx-gtk4 && cd torx-gtk
cmake -G "Unix Makefiles" -B build/ && cd build && make clean && make
```

#### License:
To discourage pre-release distribution of unsafe builds, source code is currently licensed as follows: Attribution-NonCommercial-NoDerivatives 4.0 International (CC BY-NC-ND 4.0)

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
