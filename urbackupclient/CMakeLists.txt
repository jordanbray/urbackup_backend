cmake_minimum_required(VERSION 2.8.11)

project(urbackup_client)

add_definitions(-DSTATIC_PLUGIN)


if(WIN32)
    add_library(os_specific OBJECT win_sysvol.cpp win_tokens.cpp win_ver.cpp win_all_volumes.cpp win_disk_mon.cpp
                                  ChangeJournalWatcher.cpp DirectoryWatcherThread.cpp)
else()
    add_library(os_specific OBJECT lin_tokens.cpp lin_ver.cpp)
endif()

add_library(image_thread OBJECT ImageThread.cpp)
add_library(token_callback OBJECT TokenCallback.cpp)
add_library(client_dllmain OBJECT dllmain.cpp)
add_library(client_send OBJECT ClientSend.cpp)
add_library(internet_client OBJECT InternetClient.cpp)
add_library(client OBJECT client.cpp)
add_library(file_permissions OBJECT file_permissions.cpp)
add_library(client_service OBJECT ClientService.cpp)
add_library(persistent_open_files OBJECT PersistentOpenFiles.cpp)
add_library(client_restore OBJECT client_restore.cpp)
add_library(client_service_cmd OBJECT ClientServiceCMD.cpp)
add_library(restore_download_thread OBJECT RestoreDownloadThread.cpp)
add_library(clientdao OBJECT clientdao.cpp)
add_library(restore_files OBJECT RestoreFiles.cpp)
add_library(file_metadata_download_thread OBJECT FileMetadataDownloadThread.cpp)
add_library(server_identity_mgr OBJECT ServerIdentityMgr.cpp)
add_library(common_tokens OBJECT common_tokens.cpp)
add_library(urbackup_client_obj OBJECT cmdline_preprocessor.cpp)
#add_executable(urbackup_restore_client cmdline_preprocessor.cpp)

add_executable(urbackup_client $<TARGET_OBJECTS:image_thread> $<TARGET_OBJECTS:urbackup_client_obj>
                               $<TARGET_OBJECTS:token_callback> $<TARGET_OBJECTS:client_dllmain> $<TARGET_OBJECTS:client_send>
                               $<TARGET_OBJECTS:internet_client> $<TARGET_OBJECTS:client> $<TARGET_OBJECTS:file_permissions>
                               $<TARGET_OBJECTS:client_service> $<TARGET_OBJECTS:persistent_open_files>
                               $<TARGET_OBJECTS:client_restore> $<TARGET_OBJECTS:client_service_cmd>
                               $<TARGET_OBJECTS:restore_download_thread> $<TARGET_OBJECTS:clientdao>
                               $<TARGET_OBJECTS:restore_files>
                               $<TARGET_OBJECTS:file_metadata_download_thread> $<TARGET_OBJECTS:server_identity_mgr>
                               $<TARGET_OBJECTS:common_tokens> $<TARGET_OBJECTS:os_specific>)

target_link_libraries(urbackup_client urbackup_common root_main fsimageplugin fileserv)
#target_link_libraries(urbackup_restore_client root_main urbackup_common cryptopp urbackup_client_library)

#set_target_properties(urbackup_restore_client PROPERTIES COMPILE_DEFINITIONS "DRESTORE_CLIENT")

