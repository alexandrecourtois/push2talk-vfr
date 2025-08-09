cmake_minimum_required(VERSION 3.12)
project(push2talk-vfr_plugin VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(TARGET_XP11 ON)

file(GLOB SRC_FILES "../src/*.cpp")
list(FILTER SRC_FILES EXCLUDE REGEX ".*main\\.cpp$")

add_library(push2talk-vfr_plugin SHARED "push2talk-vfr_plugin.cpp" ${SRC_FILES})

target_include_directories(push2talk-vfr_plugin PRIVATE /opt/vosk-api/src SDK/CHeaders/XPLM ../include)

if(TARGET_XP11)
    message(STATUS "Building for X-Plane 11")
    add_library(XPLM_64_lib SHARED IMPORTED)
    set_target_properties(XPLM_64_lib PROPERTIES IMPORTED_LOCATION "/opt/X-Plane 11/Resources/plugins/XPLM_64.so")
else()
    message(STATUS "Building for X-Plane 12")
    add_library(XPLM_64_lib SHARED IMPORTED)
    set_target_properties(XPLM_64_lib PROPERTIES IMPORTED_LOCATION "/home/alexandre/Prog/push2talk-vfr/libs/XPLM_64.so")
endif()

target_link_libraries(push2talk-vfr_plugin PRIVATE zmq XPLM_64_lib /opt/vosk-api/src/libvosk.so SDL2 SDL2_mixer curl unwind)

if (TARGET_XP11)
    target_compile_definitions(push2talk-vfr_plugin PRIVATE TARGET_XP11 APL=3 PLATFORM_LINUX=1)
else()
    target_compile_definitions(push2talk-vfr_plugin PRIVATE APL=3 PLATFORM_LINUX=1)
endif()

set_target_properties(push2talk-vfr_plugin PROPERTIES PREFIX "")

set_target_properties(push2talk-vfr_plugin PROPERTIES SUFFIX ".xpl")