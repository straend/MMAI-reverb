cmake_minimum_required(VERSION 2.8)
project(mmai)

set(CMAKE_C_STANDARD 11)

find_library(REQUIRED sndfile IMPORTED )
find_library(REQUIRED portaudio IMPORTED )

include_directories(${SNDFILE_INCLUDE_DIRS})
include_directories(${PORTAUDIO_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

set(SOURCE_FILES src/main.c src/reverb_fake.c src/reverb_fake.h src/delay_stuff.c src/delay_stuff.h src/main.h src/GUI.c)
add_executable(mmai ${SOURCE_FILES})

target_link_libraries(mmai sndfile portaudio m ${GTK3_LIBRARIES})
