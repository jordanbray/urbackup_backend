cmake_minimum_required(VERSION 2.8.11)

project(fuseplugin)

if(WIN32)
else()
    add_library(fuseplugin SHARED dllmain.cpp)
    target_link_libraries(fuseplugin urbackup_common root_library)
endif()

