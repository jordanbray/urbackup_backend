cmake_minimum_required(VERSION 2.8.11)

project(snapshot_helper)

add_executable(urbackup_snapshot_helper main.cpp)
target_link_libraries(urbackup_snapshot_helper root_library os_functions_no_server)
