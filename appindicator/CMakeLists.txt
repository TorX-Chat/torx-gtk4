cmake_minimum_required(VERSION 3.14)
project(torx-tray LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

execute_process(COMMAND glib-compile-resources --generate-source torx-tray.gresource.xml --target ${CMAKE_CURRENT_BINARY_DIR}/torx-tray.c WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} appindicator.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${GTK3_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}) # CMAKE_CURRENT_BINARY_DIR is required to pick up the file resulting from glib-compile-resources

target_compile_options(${PROJECT_NAME} PRIVATE ${GTK3_CFLAGS_OTHER})

if(WIN32)
	target_link_libraries(
		${PROJECT_NAME} PRIVATE
		${GTK3_LIBRARIES}
		event_core
		Ws2_32
	)
else()
	target_link_libraries(
		${PROJECT_NAME} PRIVATE
		${GTK3_LIBRARIES}
		event_core
	)
endif()
