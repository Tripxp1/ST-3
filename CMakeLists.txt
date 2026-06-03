cmake_minimum_required (VERSION 3.10.2)
project (ST-3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -pthread")

set(tool_dest "bin")
set(lib_dest "lib")
set(include_dest "include/")

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
