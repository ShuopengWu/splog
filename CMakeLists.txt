cmake_minimum_required(VERSION 3.20)

project(splog 
    VERSION 1.0.0 
    DESCRIPTION "Design of a C++ Log Repository Based on C++20"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# for gdb debug
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_library(splog SHARED splog.h splog_p.h splog_p.cpp file.h file.cpp
    splog.cpp)

set_target_properties(splog PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
)

target_include_directories(splog PUBLIC ${CMAKE_CURRENT_SOURCE_STRING})

add_executable(text main.cpp)
target_link_libraries(text PRIVATE splog)
