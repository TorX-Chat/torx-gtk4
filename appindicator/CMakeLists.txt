cmake_minimum_required(VERSION 3.14)
project(torx-tray LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

execute_process(COMMAND glib-compile-resources --generate-source torx-tray.gresource.xml WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} appindicator.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${GTK3_INCLUDE_DIRS})

target_compile_options(${PROJECT_NAME} PRIVATE ${GTK3_CFLAGS_OTHER})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES})
