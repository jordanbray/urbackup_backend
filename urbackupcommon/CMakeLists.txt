cmake_minimum_required(VERSION 2.8.11)

project(urbackup_common)

if(WIN32)
    add_library(os_functions OBJECT os_functions_win.cpp)
    add_library(os_functions_no_server os_functions_win.cpp $<TARGET_OBJECTS:escape>)
else()
    add_library(os_functions OBJECT os_functions_lin.cpp)
    add_library(os_functions_no_server os_functions_lin.cpp $<TARGET_OBJECTS:escape>)
endif()
set_target_properties(os_functions_no_server PROPERTIES COMPILE_FLAGS -DOS_FUNC_NO_SERVER)


add_library(compressed_pipe OBJECT CompressedPipe.cpp)
add_library(compressed_pipe2 OBJECT CompressedPipe2.cpp)
add_library(extent_iterator OBJECT ExtentIterator.cpp)
add_library(internet_service_pipe OBJECT InternetServicePipe.cpp)
add_library(internet_service_pipe2 OBJECT InternetServicePipe2.cpp)
add_library(sparse_file OBJECT SparseFile.cpp)
add_library(tree_hash OBJECT TreeHash.cpp)
add_library(bufmgr OBJECT bufmgr.cpp)
add_library(chunk_hasher OBJECT chunk_hasher.cpp)
add_library(escape OBJECT escape.cpp)
add_library(file_metadata OBJECT file_metadata.cpp)
add_library(filelist_utils OBJECT filelist_utils.cpp)
add_library(glob OBJECT glob.cpp)
add_library(json OBJECT json.cpp)
add_library(settingslist OBJECT settingslist.cpp)
add_library(file_client OBJECT fileclient/FileClient.cpp)
add_library(file_client_chunked OBJECT fileclient/FileClientChunked.cpp)
add_library(file_tcpstack OBJECT fileclient/tcpstack.cpp)
add_library(sha2 OBJECT sha2/sha2.c)
add_library(common_adler32 OBJECT adler32.cpp)
add_library(data OBJECT data.cpp)
add_library(miniz OBJECT miniz.c)
    
    
add_library(urbackup_common $<TARGET_OBJECTS:compressed_pipe> $<TARGET_OBJECTS:compressed_pipe2> $<TARGET_OBJECTS:extent_iterator>
                            $<TARGET_OBJECTS:internet_service_pipe> $<TARGET_OBJECTS:internet_service_pipe2>
                            $<TARGET_OBJECTS:sparse_file> $<TARGET_OBJECTS:tree_hash> $<TARGET_OBJECTS:bufmgr>
                            $<TARGET_OBJECTS:chunk_hasher> $<TARGET_OBJECTS:escape> $<TARGET_OBJECTS:file_metadata>
                            $<TARGET_OBJECTS:filelist_utils> $<TARGET_OBJECTS:glob> $<TARGET_OBJECTS:json>
                            $<TARGET_OBJECTS:settingslist> $<TARGET_OBJECTS:file_client> $<TARGET_OBJECTS:file_client_chunked>
                            $<TARGET_OBJECTS:file_tcpstack> $<TARGET_OBJECTS:sha2> $<TARGET_OBJECTS:common_adler32>
                            $<TARGET_OBJECTS:data> $<TARGET_OBJECTS:os_functions> $<TARGET_OBJECTS:miniz>)

target_link_libraries(urbackup_common root_library cryptopp z)
