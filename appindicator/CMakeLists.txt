cmake_minimum_required(VERSION 3.14)
project(torx-tray LANGUAGES C)

set(CMAKE_C_COMPILER "gcc") ## gcc / clang / TODO x86_64-w64-mingw32-gcc / TODO i686-w64-mingw32-gcc
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -D_FORTIFY_SOURCE=0 -O0 -fsanitize=undefined -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fwrapv -Wstrict-overflow=5 -Wshadow -Wpedantic -Wcast-qual -Wno-overlength-strings -Wwrite-strings -Wnull-dereference -Wstrict-prototypes -Wstrict-aliasing -Wmissing-prototypes -Wmissing-declarations -fstack-protector-strong -Wconversion")

if(${CMAKE_C_COMPILER} STREQUAL "clang") # TODO To reveal 1300+ thread-unsafe reads, remove -Wno-unsafe-buffer-usage
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Weverything -Wno-missing-variable-declarations -Wno-declaration-after-statement -Wno-cast-function-type-strict -Wno-reserved-identifier -Wno-padded -Wno-vla -Wno-used-but-marked-unused -Wno-disabled-macro-expansion -Wno-duplicate-enum -Wno-documentation -Wno-documentation-unknown-command -Wno-double-promotion -Wno-unsafe-buffer-usage")
else() # GCC
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fwhole-program -Wcast-align=strict -Wformat-signedness")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

execute_process(COMMAND glib-compile-resources --generate-source torx-tray.gresource.xml WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} appindicator.c)

target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})