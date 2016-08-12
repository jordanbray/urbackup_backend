cmake_minimum_required(VERSION 2.8.11)

project(urbackup_common CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-DVARDIR="/usr/var")
add_definitions(-DSYSCONFDIR="/etc/")
add_definitions(-DDATADIR="/usr/share/")

add_library(stringtools stringtools.cpp)

add_subdirectory(urbackupcommon)
add_subdirectory(clientctl)
add_subdirectory(cryptoplugin)
add_subdirectory(urbackupclient)

