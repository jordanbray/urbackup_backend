cmake_minimum_required(VERSION 2.8.11)

project(ursqlite C)

add_definitions(-DSQLITE_ENABLE_UNLOCK_NOTIFY=1)

add_library(ursqlite sqlite3.c shell.c)
