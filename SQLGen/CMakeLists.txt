cmake_minimum_required(VERSION 2.8.11)

project(sqlgen C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(sqlgen_lib sqlgen.cpp)
add_executable(sqlgen main.cpp)

target_link_libraries(sqlgen_lib root_main)
target_link_libraries(sqlgen sqlgen_lib)

