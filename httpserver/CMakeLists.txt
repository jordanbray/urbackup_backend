cmake_minimum_required(VERSION 2.8.11)

project(httpserver CXX)

add_library(http_action OBJECT HTTPAction.cpp)
add_library(http_client OBJECT HTTPClient.cpp)
add_library(http_file OBJECT HTTPFile.cpp)
add_library(http_proxy OBJECT HTTPProxy.cpp)
add_library(http_service OBJECT HTTPService.cpp)
add_library(index_files OBJECT IndexFiles.cpp)
add_library(mime_type OBJECT MIMEType.cpp)
add_library(http_dllmain OBJECT dllmain.cpp)
add_definitions(-DSTATIC_PLUGIN)

add_library(httpserver SHARED $<TARGET_OBJECTS:http_action> $<TARGET_OBJECTS:http_client> $<TARGET_OBJECTS:http_file>
                              $<TARGET_OBJECTS:http_proxy> $<TARGET_OBJECTS:http_service> $<TARGET_OBJECTS:index_files>
                              $<TARGET_OBJECTS:mime_type> $<TARGET_OBJECTS:http_dllmain>)

target_link_libraries(httpserver root_main urlplugin)
