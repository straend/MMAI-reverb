cmake_minimum_required(VERSION 3.7)
project(mmai)

set(CMAKE_C_STANDARD 11)

find_library(sndfile IMPORTED REQUIRED)
find_library(portaudio IMPORTED REQUIRED)

set(SOURCE_FILES src/main.c)
add_executable(mmai ${SOURCE_FILES})

target_link_libraries(mmai sndfile portaudio)
