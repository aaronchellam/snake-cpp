cmake_minimum_required(VERSION 3.28)
project(Snake)

set(raylib_DIR "C:/raylib/raylib/cmake")
find_package(raylib REQUIRED)  # Version requirement removed

message(STATUS "raylib found: ${raylib_FOUND}")
message(STATUS "raylib version: ${raylib_VERSION}")

set(CMAKE_CXX_STANDARD 17)
add_executable(Snake main.cpp)
target_link_libraries(Snake PRIVATE raylib)