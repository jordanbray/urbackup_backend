cmake_minimum_required(VERSION 2.8.11)

project(urbackup CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_compile_options(-fPIC)

SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG          "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_AR      "/usr/bin/llvm-ar")
SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
SET (CMAKE_NM      "/usr/bin/llvm-nm")
SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")


add_definitions(-DVARDIR="/usr/var")
add_definitions(-DSYSCONFDIR="/etc")
add_definitions(-DDATADIR="/usr/share")
add_definitions(-DBINDIR="/usr/bin")
add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-DPACKAGE_VERSION="3.3.3")

if(WIN32)
    add_library(condition OBJECT Condition_std.cpp)
    add_library(mutex OBJECT Mutex_std.cpp)
    add_library(shared_mutex OBJECT SharedMutex_std.cpp)
    add_library(file_os OBJECT file_win.cpp)
    add_library(server_os OBJECT ServerWin32.cpp)
    add_definitions(-D_WIN32)
else()
    add_library(condition OBJECT Condition_lin.cpp)
    add_library(mutex OBJECT Mutex_lin.cpp)
    add_library(shared_mutex OBJECT SharedMutex_lin.cpp)
    add_library(file_os OBJECT file_linux.cpp)
    add_library(server_os OBJECT ServerLinux.cpp)
    add_definitions(-DLINUX)
endif()

add_library(accept_thread OBJECT AcceptThread.cpp)
add_library(rootclient OBJECT Client.cpp)
add_library(db_settings_reader OBJECT DBSettingsReader.cpp)
add_library(database OBJECT Database.cpp)
add_library(database_cursor OBJECT DatabaseCursor.cpp)
add_library(file_settings_reader OBJECT FileSettingsReader.cpp)
add_library(loadbalancer_client OBJECT LoadbalancerClient.cpp)
add_library(lookup_service OBJECT LookupService.cpp)
add_library(memory_pipe OBJECT MemoryPipe.cpp)
add_library(memory_settings_reader OBJECT MemorySettingsReader.cpp)
add_library(output_stream OBJECT OutputStream.cpp)
add_library(pipe_throttler OBJECT PipeThrottler.cpp)
add_library(query OBJECT Query.cpp)
add_library(sqlite_factory OBJECT SQLiteFactory.cpp)
add_library(select_thread OBJECT SelectThread.cpp)
add_library(server OBJECT Server.cpp)
add_library(service_acceptor OBJECT ServiceAcceptor.cpp)
add_library(service_worker OBJECT ServiceWorker.cpp)
add_library(session_mgr OBJECT SessionMgr.cpp)
add_library(settings_reader OBJECT SettingsReader.cpp)
add_library(static_plugin_registration OBJECT StaticPluginRegistration.cpp)
add_library(stream_pipe OBJECT StreamPipe.cpp)
add_library(table OBJECT Table.cpp)
add_library(template OBJECT Template.cpp)
add_library(thread_pool OBJECT ThreadPool.cpp)
add_library(worker_thread OBJECT WorkerThread.cpp)
add_library(file_common OBJECT file_common.cpp)
add_library(file_fstream OBJECT file_fstream.cpp)
add_library(file_memory OBJECT file_memory.cpp)
add_library(main OBJECT main.cpp)
add_library(md5_again OBJECT md5.cpp)
add_library(mt19937ar OBJECT mt19937ar.cpp)
add_library(stringtools OBJECT stringtools.cpp)

add_library(root_library $<TARGET_OBJECTS:condition> $<TARGET_OBJECTS:mutex> $<TARGET_OBJECTS:shared_mutex> $<TARGET_OBJECTS:file_os>
                         $<TARGET_OBJECTS:server_os> $<TARGET_OBJECTS:accept_thread> $<TARGET_OBJECTS:rootclient>
                         $<TARGET_OBJECTS:db_settings_reader> $<TARGET_OBJECTS:database> $<TARGET_OBJECTS:database_cursor>
                         $<TARGET_OBJECTS:file_settings_reader> $<TARGET_OBJECTS:loadbalancer_client>
                         $<TARGET_OBJECTS:lookup_service> $<TARGET_OBJECTS:memory_pipe> $<TARGET_OBJECTS:memory_settings_reader>
                         $<TARGET_OBJECTS:output_stream> $<TARGET_OBJECTS:pipe_throttler> $<TARGET_OBJECTS:query>
                         $<TARGET_OBJECTS:sqlite_factory> $<TARGET_OBJECTS:select_thread> $<TARGET_OBJECTS:server>
                         $<TARGET_OBJECTS:service_acceptor> $<TARGET_OBJECTS:service_worker> $<TARGET_OBJECTS:session_mgr>
                         $<TARGET_OBJECTS:settings_reader> $<TARGET_OBJECTS:static_plugin_registration> $<TARGET_OBJECTS:stream_pipe>
                         $<TARGET_OBJECTS:table> $<TARGET_OBJECTS:template> $<TARGET_OBJECTS:thread_pool>
                         $<TARGET_OBJECTS:worker_thread> $<TARGET_OBJECTS:file_common> $<TARGET_OBJECTS:file_fstream>
                         $<TARGET_OBJECTS:file_memory> $<TARGET_OBJECTS:md5_again>
                         $<TARGET_OBJECTS:mt19937ar> $<TARGET_OBJECTS:stringtools>)

target_link_libraries(root_library ursqlite dl pthread fastcgi)

add_library(root_main main.cpp)
target_link_libraries(root_main root_library)

add_subdirectory(urbackupcommon)
add_subdirectory(clientctl)
add_subdirectory(cryptoplugin)
add_subdirectory(urbackupclient)
add_subdirectory(libfastcgi)
add_subdirectory(sqlite)
add_subdirectory(SQLGen)
add_subdirectory(piped_process)
add_subdirectory(fileservplugin)
add_subdirectory(fsimageplugin)
add_subdirectory(fuseplugin)
add_subdirectory(httpserver)
add_subdirectory(urlplugin)
add_subdirectory(urbackupserver)

