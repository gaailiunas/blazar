cmake_minimum_required(VERSION 3.15)
project(blazar)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(sidecar)
add_subdirectory(control_plane)
add_subdirectory(common)
