cmake_minimum_required(VERSION 3.20)

project(splog 
    VERSION 1.0.0 
    DESCRIPTION "A C++ HTTP server core project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# for gdb debug
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(splog log.cpp file.cpp main.cpp)
