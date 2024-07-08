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

set(BUILD_SHARED_LIBS 0) ## 2024: static works, shared works other than libcrypto.a (which we statically compile on android/linux)
set(BUILD_TORX_DEPS 0) ## WORKS !!!

if(TORX_TAG)
	set(TAG_REPO "${TORX_TAG}")
else()
	set(TAG_REPO "main")
endif()
if(LAN_REPO)
	message(STATUS "Using LAN repository: ${LAN_REPO}")
	set(REPO_TORX "${LAN_REPO}/libtorx.git")
else()
	message(STATUS "Using default remote repositories instead of LAN (GTK).")
	set(REPO_TORX "https://github.com/TorX-Chat/libtorx/")
endif()
#set(CMAKE_C_COMPILER "clang") ## Enabling this breaks add_subdirectory, so would have to be placed after add_subdirectory. gcc / clang / TODO x86_64-w64-mingw32-gcc / TODO i686-w64-mingw32-gcc
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
if(${CMAKE_C_COMPILER} STREQUAL "clang") # TODO To reveal 1300+ thread-unsafe reads, remove -Wno-unsafe-buffer-usage
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Weverything -Wno-missing-variable-declarations -Wno-declaration-after-statement -Wno-cast-function-type-strict -Wno-reserved-identifier -Wno-padded -Wno-vla -Wno-used-but-marked-unused -Wno-disabled-macro-expansion -Wno-duplicate-enum -Wno-documentation -Wno-documentation-unknown-command -Wno-double-promotion -Wno-unsafe-buffer-usage")
else() # GCC
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwhole-program -Wcast-align=strict -Wformat-signedness")
endif()

add_subdirectory(appindicator)
set_target_properties(torx-tray PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(BUILD_SHARED_LIBS)
	add_library(torx SHARED IMPORTED)
	set(TORX_BUILD_BYPRODUCT ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download-build/libtorx.so)
else()
	add_library(torx STATIC IMPORTED)
	set(TORX_BUILD_BYPRODUCT ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download-build/libtorx.a)
endif()

ExternalProject_Add(
	torx-download
	GIT_REPOSITORY ${REPO_TORX}
	GIT_TAG ${TAG_REPO}
	CMAKE_ARGS ${CMAKE_ARGS} -D BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -D BUILD_TORX_DEPS=${BUILD_TORX_DEPS} -D LAN_REPO=${LAN_REPO}
	INSTALL_COMMAND "" ## TODO should install the .so/.a and place the torx.h
	BUILD_BYPRODUCTS ${TORX_BUILD_BYPRODUCT}
)

execute_process(COMMAND glib-compile-resources --generate-source torx-gtk4.gresource.xml)

find_package(PkgConfig REQUIRED)	# Requires GTK-4-dev to exist on local system
#find_package(PNG REQUIRED)		# Requires libpng-dev to exist on local system
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download/include/ ${CMAKE_CURRENT_SOURCE_DIR}/include/)
#include_directories(${PNG_INCLUDE_DIRS} ${GTK4_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/torx-download-prefix/src/torx-download/include/)
link_directories(${GTK4_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${GTK4_CFLAGS_OTHER})

add_library(gif_animation SHARED gif_animation.c) # XXX XXX XXX BEWARE: The contents of this file *may* be constrained by the LGPLv2 license, therefore it is compiled seperately

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # Critical for install commands later
add_executable( ${PROJECT_NAME} main_gtk4.c )
add_compile_definitions(SQLITE_HAS_CODEC QR_GENERATOR SECURE_MALLOC) # necessary, not redundant
target_link_libraries(
	${PROJECT_NAME}
		gif_animation
		${GTK4_LIBRARIES}
		sodium
	#	event
		event_core
		event_pthreads
		png
		sqlcipher
)

# Install the executables
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(TARGETS torx-tray DESTINATION bin)

# Install dynamically linked library files (if they exist)
install(TARGETS gif_animation LIBRARY DESTINATION lib)
# install(TARGETS torx LIBRARY DESTINATION lib) ## TODO if BUILD_SHARED_LIBS, we need to install this

# Install the desktop file
install(FILES org.torx.gtk4.desktop DESTINATION share/applications)

# Install the icon
install(FILES other/org.torx.gtk4.png DESTINATION share/icons/hicolor/48x48/apps)
