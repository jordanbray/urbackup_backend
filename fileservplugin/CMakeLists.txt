cmake_minimum_required(VERSION 2.8.11)

project(urbackup_fileserver_plugin_proj)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-DSTATIC_PLUGIN)

add_library(cclient_thread OBJECT CClientThread.cpp)
add_library(ctcp_file_serv OBJECT CTCPFileServ.cpp)
add_library(cudp_thread OBJECT CUDPThread.cpp)
add_library(chunk_send_thread OBJECT ChunkSendThread.cpp)
add_library(critical_section OBJECT CriticalSection.cpp)
add_library(file_metadata_pipe OBJECT FileMetadataPipe.cpp)
add_library(file_serv OBJECT FileServ.cpp)
add_library(file_serv_factory OBJECT FileServFactory.cpp)
add_library(pipe_file OBJECT PipeFile.cpp)
add_library(pipe_file_base OBJECT PipeFileBase.cpp)
add_library(pipe_file_tar OBJECT PipeFileTar.cpp)
if (WIN32)
    add_library(pipe_file_os OBJECT PipeFileWin.cpp)
else()
    add_library(pipe_file_os OBJECT PipeFileUnix.cpp)
endif()
add_library(pipe_sessions OBJECT PipeSessions.cpp)
add_library(fileserv_bufmgr OBJECT bufmgr.cpp)
add_library(fileserver_dllmain OBJECT dllmain.cpp)
add_library(fileserver_log OBJECT log.cpp)
add_library(map_buffer OBJECT map_buffer.cpp)
add_library(fileserv_pluginmgr OBJECT pluginmgr.cpp)

add_definitions(-DSTATIC_PLUGIN)

add_library(fileserv_main OBJECT main.cpp)

add_library(fileserv SHARED $<TARGET_OBJECTS:cclient_thread> $<TARGET_OBJECTS:ctcp_file_serv>
    $<TARGET_OBJECTS:cudp_thread> $<TARGET_OBJECTS:chunk_send_thread> $<TARGET_OBJECTS:critical_section>
    $<TARGET_OBJECTS:file_metadata_pipe> $<TARGET_OBJECTS:file_serv> $<TARGET_OBJECTS:fileserv_main>
    $<TARGET_OBJECTS:pipe_file> $<TARGET_OBJECTS:pipe_file_base> $<TARGET_OBJECTS:pipe_file_tar>
    $<TARGET_OBJECTS:pipe_file_os> $<TARGET_OBJECTS:pipe_sessions> $<TARGET_OBJECTS:fileserv_bufmgr>
    $<TARGET_OBJECTS:fileserver_log> $<TARGET_OBJECTS:map_buffer> $<TARGET_OBJECTS:fileserv_pluginmgr>
    $<TARGET_OBJECTS:file_serv_factory> $<TARGET_OBJECTS:fileserver_dllmain>)

target_link_libraries(fileserv root_main)
