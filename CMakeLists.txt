cmake_minimum_required(VERSION 3.14)
project(torx-gtk4 LANGUAGES C)
#[[ # Works but unsure how to pass args. Can we just declare and have them used?
include(FetchContent)
FetchContent_Declare(
	torx
	GIT_REPOSITORY https://github.com/TorX-Chat/libtorx/
	# GIT_REPOSITORY http://10.8.88.11/libtorx.git
	OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(torx)
]]
include(ExternalProject)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_TLS_VERIFY TRUE)

if(DEFINED ENV{BUILD_SHARED_LIBS} AND NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS $ENV{BUILD_SHARED_LIBS})
elseif(NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS 0) ## 2024: static works, shared works other than libcrypto.a (which we statically compile on android/linux)
endif()

if(DEFINED ENV{BUILD_TORX_DEPS} AND NOT DEFINED BUILD_TORX_DEPS)
	set(BUILD_TORX_DEPS $ENV{BUILD_TORX_DEPS})
elseif(NOT DEFINED BUILD_TORX_DEPS)
	set(BUILD_TORX_DEPS 0)
endif()

if(DEFINED ENV{TORX_TAG} AND NOT DEFINED TORX_TAG)
	set(TORX_TAG $ENV{TORX_TAG})
elseif(NOT DEFINED TORX_TAG)
	set(TORX_TAG "main")
endif()

if(DEFINED ENV{LAN_REPO} AND NOT DEFINED LAN_REPO)
	set(LAN_REPO $ENV{LAN_REPO})
	message(STATUS "Using LAN repository: ${LAN_REPO}")
	set(REPO_TORX "${LAN_REPO}/libtorx.git")
elseif(NOT DEFINED LAN_REPO)
	message(STATUS "Using default remote repositories instead of LAN (GTK).")
	set(REPO_TORX "https://github.com/TorX-Chat/libtorx/")
else()
	message(STATUS "Using LAN repository: ${LAN_REPO}")
	set(REPO_TORX "${LAN_REPO}/libtorx.git")
endif()

	#### C90 crap: -Wvla -Wdeclaration-after-statement
	#### Unimportant: -Waggregate-return
	#### Heavily CPU + RAM + time intensive junk static analyzer that throws mainly "possible null dereference" warnings (requires 9+gb ram): -fanalyzer
	#### other static analyzer binaries: cppcheck clang-tidy
	## XXX For dev builds: -D_FORTIFY_SOURCE=0 -fsanitize=thread -fsanitize=address -fsanitize=undefined -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero
	## XXX For release: -D_FORTIFY_SOURCE=2 -O2 and REMOVE ALL -fsanitize
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -D_FORTIFY_SOURCE=0 -O0 -fwrapv -mshstk -Wstrict-overflow=5 -Wshadow -Wpedantic -Wcast-qual -Wno-overlength-strings -Wwrite-strings -Wnull-dereference -Wstrict-prototypes -Wstrict-aliasing -Wmissing-prototypes -Wmissing-declarations -fstack-protector-strong -Wconversion")
if(NOT WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero")
endif()

message(STATUS "Using compiler: ${CMAKE_C_COMPILER}")
string(FIND ${CMAKE_C_COMPILER} "clang" found_clang)
if(found_clang GREATER -1)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Weverything -Wno-missing-variable-declarations -Wno-declaration-after-statement -Wno-cast-function-type-strict -Wno-reserved-identifier -Wno-padded -Wno-vla -Wno-used-but-marked-unused -Wno-disabled-macro-expansion -Wno-documentation -Wno-documentation-unknown-command -Wno-double-promotion -Wno-unsafe-buffer-usage")
else() # GCC
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwhole-program -Wcast-align=strict -Wformat-signedness")
endif()

add_subdirectory(appindicator)
set_target_properties(torx-tray PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # Critical for install commands later

add_executable( ${PROJECT_NAME} main_gtk4.c )

find_path(TORX_INCLUDE_DIR NAMES torx.h PATHS /usr/local/include /usr/include)
find_library(torx NAMES torx PATHS /usr/local/lib /usr/lib)

if(TORX_INCLUDE_DIR AND torx)
	message(STATUS "Existing TorX library found; NOT building.") # TODO maybe verify that it is appropriately shared / static if BUILD_SHARED_LIBS was passed?
else()
	message(STATUS "Existing TorX library NOT found; Building.")
	if(BUILD_SHARED_LIBS)
		add_library(torx SHARED IMPORTED)
		set(TORX_BUILD_BYPRODUCT ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download-build/libtorx.so)
		set_target_properties(torx PROPERTIES IMPORTED_LOCATION ${TORX_BUILD_BYPRODUCT})
		install(PROGRAMS ${TORX_BUILD_BYPRODUCT} DESTINATION lib) # MUST be "PROGRAMS" not TARGETS or fatal error will occur
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download/include/torx.h DESTINATION include)
	else()
		add_library(torx STATIC IMPORTED)
		set(TORX_BUILD_BYPRODUCT ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download-build/libtorx.a)
		set_target_properties(torx PROPERTIES IMPORTED_LOCATION ${TORX_BUILD_BYPRODUCT})
	endif()
	ExternalProject_Add(
		torx-download
		GIT_REPOSITORY ${REPO_TORX}
		GIT_TAG ${TORX_TAG}
		CMAKE_ARGS ${CMAKE_ARGS} -D BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -D BUILD_TORX_DEPS=${BUILD_TORX_DEPS} -D QR_GENERATOR=1 -D LAN_REPO=${LAN_REPO}
		INSTALL_COMMAND "" ## it doesn't wait for `make install`
		BUILD_BYPRODUCTS ${TORX_BUILD_BYPRODUCT}
	)
	add_dependencies(torx torx-download)
	set(TORX_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download/include/)
endif()

execute_process(COMMAND glib-compile-resources --generate-source torx-gtk4.gresource.xml --target ${CMAKE_CURRENT_BINARY_DIR}/torx-gtk4.c WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)	# Requires GTK-4-dev to exist on local system
pkg_check_modules(GTK4 REQUIRED gtk4 gstreamer-1.0 gstreamer-app-1.0)

add_library(gif_animation SHARED gif_animation.c) # XXX XXX XXX BEWARE: The contents of this file *may* be constrained by the LGPLv2 license, therefore it is compiled seperately
target_include_directories(gif_animation PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(gif_animation ${GTK4_LIBRARIES}) # necessary on Windows, unsure why it works without on Linux

target_include_directories(${PROJECT_NAME} PRIVATE ${GTK4_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${TORX_INCLUDE_DIR}) # CMAKE_CURRENT_BINARY_DIR is required to pick up the file resulting from glib-compile-resources
target_compile_options(${PROJECT_NAME} PRIVATE ${GTK4_CFLAGS_OTHER} ${GSTREAMER_CFLAGS_OTHER})
add_compile_definitions(SQLITE_HAS_CODEC QR_GENERATOR SECURE_MALLOC) # necessary, not redundant (NOTE: at least QR_GENERATOR is NOT redundant because it affects torx.h)

if(WIN32)
target_link_libraries(
	${PROJECT_NAME} PRIVATE
		Winmm
		Ws2_32
		gif_animation
		${GTK4_LIBRARIES}
		${GSTREAMER_LIBRARY_DIRS}
		torx
		png
		sodium
	##	event
		event_core
	##	event_pthreads
		sqlcipher
)
else()
target_link_libraries(
	${PROJECT_NAME} PRIVATE
		gif_animation
		${GTK4_LIBRARIES}
		${GSTREAMER_LIBRARY_DIRS}
		torx
		png		## TODO required ONLY when BUILD_SHARED_LIBS=0
		sodium		## TODO required ALWAYS for sodium_memzero
	##	event
		event_core	## TODO required ALWAYS for evutil_closesocket (and probably things like evutil_socket_t)
		event_pthreads	## TODO required ONLY when BUILD_SHARED_LIBS=0
		sqlcipher	## TODO required ONLY when BUILD_SHARED_LIBS=0
)
endif()

# Install the executables
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(TARGETS torx-tray DESTINATION bin)

# Install dynamically linked library files (if they exist)
install(TARGETS gif_animation DESTINATION lib)

# Install the desktop file
install(FILES org.torx.gtk4.desktop DESTINATION share/applications)

# Install the icon
install(FILES other/org.torx.gtk4.png DESTINATION share/icons/hicolor/48x48/apps)
