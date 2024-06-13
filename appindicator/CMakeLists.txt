cmake_minimum_required(VERSION 3.14)
project(torx-tray LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

execute_process(COMMAND glib-compile-resources --generate-source torx-tray.gresource.xml WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} appindicator.c)

target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})