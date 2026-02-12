add_library (splog SHARED "splog.cpp" )

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories (splog PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_definitions (splog PRIVATE SPLOG_EXPORT)