cmake_minimum_required(VERSION 2.8.11)

project(clientctl)

add_library(connector Connector.cpp)
add_library(jsoncpp jsoncpp.cpp)
add_library(tcpstack tcpstack.cpp)

target_link_libraries(connector os_functions_no_server jsoncpp stringtools tcpstack escape)

add_executable(clientctl main.cpp)

target_link_libraries(clientctl connector)

