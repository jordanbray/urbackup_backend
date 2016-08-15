cmake_minimum_required(VERSION 2.8.11)

project(piped_process)

if(WIN32)
    add_library(piped_process_os OBJECT piped_process_win.cpp)
else()
    add_library(piped_process_os OBJECT piped_process_lin.cpp)
endif()

add_library(piped_process_pluginmgr OBJECT pluginmgr.cpp)
add_library(piped_process_getpid OBJECT getpid.cpp)
add_library(piped_process_dllmain OBJECT dllmain.cpp)
add_library(piped_process_factory OBJECT PipedProcessFactory.cpp)

add_library(piped_process piped_process.cpp $<TARGET_OBJECTS:piped_process_os> $<TARGET_OBJECTS:piped_process_pluginmgr>
                          $<TARGET_OBJECTS:piped_process_getpid> $<TARGET_OBJECTS:piped_process_dllmain>
                          $<TARGET_OBJECTS:piped_process_factory>)

target_link_libraries(piped_process cryptopp os_specific root_main urbackup_common)
