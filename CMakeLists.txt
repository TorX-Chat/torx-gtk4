cmake_minimum_required(VERSION 3.14)
project(torx-gtk4 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_TLS_VERIFY TRUE)

if(DEFINED ENV{BUILD_SHARED_LIBS} AND NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS $ENV{BUILD_SHARED_LIBS})
elseif(NOT DEFINED BUILD_SHARED_LIBS) # Set default as static. Static works, shared works other than libcrypto.a (which we statically compile on android/linux).
	set(BUILD_SHARED_LIBS 0)
endif()
message(STATUS "TorX shared libs: ${BUILD_SHARED_LIBS}")

if(DEFINED ENV{BUILD_TORX_DEPS} AND NOT DEFINED BUILD_TORX_DEPS)
	set(BUILD_TORX_DEPS $ENV{BUILD_TORX_DEPS})
elseif(NOT DEFINED BUILD_TORX_DEPS) # Set default
	set(BUILD_TORX_DEPS 0)
endif()
message(STATUS "TorX dependencies: ${BUILD_TORX_DEPS}")

if(DEFINED ENV{TORX_TAG} AND NOT DEFINED TORX_TAG)
	set(TORX_TAG $ENV{TORX_TAG})
elseif(NOT DEFINED TORX_TAG) # Set default
	set(TORX_TAG "main")
endif()
message(STATUS "TorX tag: ${TORX_TAG}")

if(DEFINED ENV{LAN_REPO} AND NOT DEFINED LAN_REPO)
	set(LAN_REPO $ENV{LAN_REPO})
	set(REPO_TORX "${LAN_REPO}/libtorx.git")
elseif(NOT DEFINED LAN_REPO) # Set default
	set(REPO_TORX "https://github.com/TorX-Chat/libtorx/")
else()
	set(REPO_TORX "${LAN_REPO}/libtorx.git")
endif()
message(STATUS "TorX repository: ${LAN_REPO}")

## START BLOCK: Try to keep this the same in torx-gtk4 and libtorx. XXX It WILL be passed to libtorx, but libtorx also has a default in case of independent build. XXX
	#### C90 crap: -Wvla -Wdeclaration-after-statement
	#### Unimportant: -Waggregate-return
	#### Heavily CPU + RAM + time intensive junk static analyzer that throws mainly "possible null dereference" warnings (requires 9+gb ram): -fanalyzer
	#### other static analyzer binaries: cppcheck clang-tidy
if(NOT DEFINED CMAKE_C_FLAGS OR CMAKE_C_FLAGS STREQUAL "")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug") ## XXX Dev builds can add either of: -fsanitize=thread -fsanitize=address
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=0 -O0")
		if(NOT WIN32)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero")
		endif()
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -O2")
	endif()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra  -fwrapv -mshstk -Wstrict-overflow=5 -Wshadow -Wpedantic -Wcast-qual -Wno-overlength-strings -Wwrite-strings -Wnull-dereference -Wstrict-prototypes -Wstrict-aliasing -Wmissing-prototypes -Wmissing-declarations -fstack-protector-strong -Wconversion -Wformat-security")
	string(FIND ${CMAKE_C_COMPILER} "clang" found_clang)
	if(found_clang GREATER -1)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Weverything -Wno-missing-variable-declarations -Wno-declaration-after-statement -Wno-cast-function-type-strict -Wno-reserved-identifier -Wno-padded -Wno-vla -Wno-used-but-marked-unused -Wno-disabled-macro-expansion -Wno-documentation -Wno-documentation-unknown-command -Wno-double-promotion -Wno-unsafe-buffer-usage")
	else() # GCC
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align=strict -Wformat-signedness") # Dropped -fwhole-program because it caused linking errors when passed to libtorx. Likely responsible for other linking abnormalities.
	endif()
endif()
message(STATUS "${PROJECT_NAME} C_FLAGS being set to: ${CMAKE_C_FLAGS}")
## END BLOCK

find_library(torx NAMES torx PATHS ${CMAKE_INSTALL_PREFIX}/lib)
find_path(TORX_INCLUDE_DIR NAMES torx.h PATHS ${CMAKE_INSTALL_PREFIX}/include)

if(TORX_INCLUDE_DIR AND torx)
	message(STATUS "TorX library found; NOT building.") # TODO maybe verify that it is appropriately shared / static if BUILD_SHARED_LIBS was passed?
else()
	message(STATUS "TorX library NOT found; Building.")
	include(FetchContent)
	FetchContent_Declare(
		torx
		GIT_REPOSITORY ${REPO_TORX}
		GIT_TAG ${TORX_TAG}
	)
	set(QR_GENERATOR 1)
	FetchContent_MakeAvailable(torx)
endif()

add_subdirectory(appindicator)
set_target_properties(torx-tray PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable( ${PROJECT_NAME} main_gtk4.c )

execute_process(COMMAND glib-compile-resources --generate-source torx-gtk4.gresource.xml --target ${CMAKE_CURRENT_BINARY_DIR}/torx-gtk4.c WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)	# Requires GTK-4-dev to exist on local system
pkg_check_modules(GTK4 REQUIRED gtk4 gstreamer-1.0 gstreamer-app-1.0)

add_library(gifanimation MODULE gif_animation.c) # XXX XXX XXX BEWARE: The contents of this file *may* be constrained by the LGPLv2 license, therefore it is compiled seperately
target_include_directories(gifanimation PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(gifanimation ${GTK4_LIBRARIES}) # necessary on Windows, unsure why it works without on Linux

target_include_directories(${PROJECT_NAME} PRIVATE ${GTK4_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ) # CMAKE_CURRENT_BINARY_DIR is required to pick up the file resulting from glib-compile-resources
target_compile_options(${PROJECT_NAME} PRIVATE ${GTK4_CFLAGS_OTHER} ${GSTREAMER_CFLAGS_OTHER})
add_compile_definitions(SQLITE_HAS_CODEC QR_GENERATOR SECURE_MALLOC) # necessary, not redundant (NOTE: at least QR_GENERATOR is NOT redundant because it affects torx.h)

if(WIN32)
message(STATUS "TorX Win32")
target_link_libraries(
	${PROJECT_NAME} PRIVATE
		Winmm		## For Wave audio output
		${GTK4_LIBRARIES}
		${GSTREAMER_LIBRARY_DIRS}
		torx
		png
		sodium
	##	event
		event_core
	##	event_pthreads
		sqlcipher
		Ws2_32		## This MUST be last, or linking errors WILL occur.
)
else()
message(STATUS "TorX Unix")
target_link_libraries(
	${PROJECT_NAME} PRIVATE
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

# Install dynamically linked module library files (if they exist), in a subdirectory to comply with https://udd.debian.org/lintian-tag/custom-library-search-path
install(TARGETS gifanimation DESTINATION lib/${PROJECT_NAME})

# Install the desktop file
install(FILES org.torx.gtk4.desktop DESTINATION share/applications)

# Install the icon
install(FILES other/org.torx.gtk4.png DESTINATION share/icons/hicolor/128x128/apps)
