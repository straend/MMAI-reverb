cmake_minimum_required(VERSION 2.8)
project(mmai)

set(CMAKE_C_STANDARD 11)

find_library(REQUIRED sndfile IMPORTED )
find_library(REQUIRED portaudio IMPORTED )

include_directories(${SNDFILE_INCLUDE_DIRS})
include_directories(${PORTAUDIO_INCLUDE_DIRS})


set(SOURCE_FILES src/main.c src/functions.c src/reverb_fake.c src/reverb_fake.h src/delay_stuff.c src/delay_stuff.h)
add_executable(mmai ${SOURCE_FILES})

target_link_libraries(mmai sndfile portaudio)
