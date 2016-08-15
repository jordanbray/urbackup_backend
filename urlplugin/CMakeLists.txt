cmake_minimum_required(VERSION 2.8.11)

project(urlplugin CXX)

add_library(url_factory OBJECT UrlFactory.cpp)
add_library(url_dllmain OBJECT dllmain.cpp)
add_library(url_pluginmgr OBJECT pluginmgr.cpp)
add_definitions(-DSTATIC_PLUGIN)

add_library(urlplugin SHARED $<TARGET_OBJECTS:url_factory> $<TARGET_OBJECTS:url_dllmain> $<TARGET_OBJECTS:url_pluginmgr>)

target_link_libraries(urlplugin root_main curl)
