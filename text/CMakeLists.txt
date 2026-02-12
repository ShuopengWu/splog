set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (text "text.cpp")
target_link_libraries (text PRIVATE splog)

add_executable (text_c_abi "text_c_abi.c")
target_link_libraries (text_c_abi PRIVATE splog)