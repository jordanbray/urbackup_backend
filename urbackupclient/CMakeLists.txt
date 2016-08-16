cmake_minimum_required(VERSION 2.8.11)

project(urbackup_client)

if(WIN32)
    add_library(win_sysvol OBJECT win_sysvol.cpp)
    add_library(win_tokens OBJECT win_tokens.cpp)
    add_library(win_ver OBJECT win_ver.cpp)
    add_library(win_all_volumes OBJECT win_all_volumes.cpp)
    add_library(win_disk_mon OBJECT win_disk_mon.cpp)
    add_library(change_journal_watcher OBJECT ChangeJournalWatcher.cpp)
    add_library(directory_watcher_thread OBJECT DirectoryWatcherThread.cpp)
    add_library(os_specific $<TARGET_OBJECTS:win_sysvol> $<TARGET_OBJECTS:win_tokens> $<TARGET_OBJECTS:win_ver>
        $<TARGET_OBJECTS:win_all_volumes> $<TARGET_OBJECTS:win_disk_mon> $<TARGET_OBJECTS:change_journal_watcher>
        $<TARGET_OBJECTS:directory_watcher>)
else()
    add_library(lin_tokens OBJECT lin_tokens.cpp)
    add_library(lin_ver OBJECT lin_ver.cpp)
    add_library(os_specific $<TARGET_OBJECTS:lin_tokens> $<TARGET_OBJECTS:lin_ver>)
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

add_executable(urbackup_client cmdline_preprocessor.cpp)
add_executable(urbackup_restore_client cmdline_preprocessor.cpp)

add_library(urbackup_client_library $<TARGET_OBJECTS:image_thread>
                                    $<TARGET_OBJECTS:token_callback> $<TARGET_OBJECTS:client_dllmain> $<TARGET_OBJECTS:client_send>
                                    $<TARGET_OBJECTS:internet_client> $<TARGET_OBJECTS:client> $<TARGET_OBJECTS:file_permissions>
                                    $<TARGET_OBJECTS:client_service> $<TARGET_OBJECTS:persistent_open_files>
                                    $<TARGET_OBJECTS:client_restore> $<TARGET_OBJECTS:client_service_cmd>
                                    $<TARGET_OBJECTS:restore_download_thread> $<TARGET_OBJECTS:clientdao>
                                    $<TARGET_OBJECTS:restore_files>
                                    $<TARGET_OBJECTS:file_metadata_download_thread> $<TARGET_OBJECTS:server_identity_mgr>
                                    $<TARGET_OBJECTS:common_tokens>)

target_link_libraries(urbackup_client_library cryptopp os_specific root_main urbackup_common fileserv)
target_link_libraries(urbackup_client root_main urbackup_common cryptopp urbackup_client_library)
target_link_libraries(urbackup_restore_client root_main urbackup_common cryptopp urbackup_client_library)

set_target_properties(urbackup_restore_client PROPERTIES COMPILE_DEFINITIONS "DRESTORE_CLIENT")
