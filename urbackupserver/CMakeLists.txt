cmake_minimum_required(VERSION 2.8.11)

project(urbackupserver)

add_library(backup OBJECT Backup.cpp)
add_library(chunk_patcher OBJECT ChunkPatcher.cpp)
add_library(client_main OBJECT ClientMain.cpp)
add_library(continuous_backup OBJECT ContinuousBackup.cpp)
add_library(file_backup OBJECT FileBackup.cpp)
add_library(file_index OBJECT FileIndex.cpp)
add_library(server_file_metadata_download_thread OBJECT FileMetadataDownloadThread.cpp)
add_library(full_file_backup OBJECT FullFileBackup.cpp)
add_library(image_backup OBJECT ImageBackup.cpp)
add_library(incr_file_backup OBJECT IncrFileBackup.cpp)
add_library(internet_service_connector OBJECT InternetServiceConnector.cpp)
add_library(lmdb_file_index OBJECT LMDBFileIndex.cpp)
add_library(server_download_thread OBJECT ServerDownloadThread.cpp)
add_library(throttle_updater OBJECT ThrottleUpdater.cpp)
add_library(wal_checkpoint_thread OBJECT WalCheckpointThread.cpp)
add_library(check_files_index OBJECT apps/check_files_index.cpp)
add_library(cleanup_cmd OBJECT apps/cleanup_cmd.cpp)
add_library(export_auth_log OBJECT apps/export_auth_log.cpp)
add_library(md5sum_check OBJECT apps/md5sum_check.cpp)
add_library(repair_cmd OBJECT apps/repair_cmd.cpp)
add_library(skiphash_copy OBJECT apps/skiphash_copy.cpp)
add_library(cmdline_preprocessor OBJECT cmdline_preprocessor.cpp)
add_library(create_files_index OBJECT create_files_index.cpp)
add_library(server_backup_dao OBJECT dao/ServerBackupDao.cpp)
add_library(server_cleanup_dao OBJECT dao/ServerCleanupDao.cpp)
add_library(server_files_dao OBJECT dao/ServerFilesDao.cpp)
add_library(server_link_dao OBJECT dao/ServerLinkDao.cpp)
add_library(server_link_journal_dao OBJECT dao/ServerLinkJournalDao.cpp)
add_library(server_dllmain OBJECT dllmain.cpp)
add_library(filedownload OBJECT filedownload.cpp)
add_library(restore_client OBJECT restore_client.cpp)
add_library(server_server OBJECT server.cpp)
add_library(server_archive OBJECT server_archive.cpp)
add_library(server_channel OBJECT server_channel.cpp)
add_library(server_cleanup OBJECT server_cleanup.cpp)
add_library(server_continuous OBJECT server_continuous.cpp)
add_library(server_dir_links OBJECT server_dir_links.cpp)
add_library(server_hash OBJECT server_hash.cpp)
add_library(server_log OBJECT server_log.cpp)
add_library(server_ping OBJECT server_ping.cpp)
add_library(server_prepare_hash OBJECT server_prepare_hash.cpp)
add_library(server_running OBJECT server_running.cpp)
add_library(server_settings OBJECT server_settings.cpp)
add_library(server_status OBJECT server_status.cpp)
add_library(server_update OBJECT server_update.cpp)
add_library(server_update_stats OBJECT server_update_stats.cpp)
add_library(server_writer OBJECT server_writer.cpp)
add_library(add_client OBJECT serverinterface/add_client.cpp)
add_library(backups OBJECT serverinterface/backups.cpp)
add_library(create_zip OBJECT serverinterface/create_zip.cpp)
add_library(download_client OBJECT serverinterface/download_client.cpp)
add_library(getimage OBJECT serverinterface/getimage.cpp)
add_library(helper OBJECT serverinterface/helper.cpp)
add_library(lastacts OBJECT serverinterface/lastacts.cpp)
add_library(livelog OBJECT serverinterface/livelog.cpp)
add_library(login OBJECT serverinterface/login.cpp)
add_library(logs OBJECT serverinterface/logs.cpp)
add_library(piegraph OBJECT serverinterface/piegraph.cpp)
add_library(progress OBJECT serverinterface/progress.cpp)
add_library(restore_prepare_wait OBJECT serverinterface/restore_prepare_wait.cpp)
add_library(salt OBJECT serverinterface/salt.cpp)
add_library(settings OBJECT serverinterface/settings.cpp)
add_library(shutdown OBJECT serverinterface/shutdown.cpp)
add_library(start_backup OBJECT serverinterface/start_backup.cpp)
add_library(status OBJECT serverinterface/status.cpp)
add_library(usage OBJECT serverinterface/usage.cpp)
add_library(usagegraph OBJECT serverinterface/usagegraph.cpp)
add_library(users OBJECT serverinterface/users.cpp)
add_library(snapshot_helper OBJECT snapshot_helper.cpp)
add_library(treediff OBJECT treediff/TreeDiff.cpp)
add_library(treenode OBJECT treediff/TreeNode.cpp)
add_library(treereader OBJECT treediff/TreeReader.cpp)
add_library(verify_hashes OBJECT verify_hashes.cpp)
add_library(mdb OBJECT lmdb/mdb.c)
add_library(midl OBJECT lmdb/midl.c)

add_definitions(-DSTATIC_PLUGIN)

add_library(urbackup_server_lib SHARED $<TARGET_OBJECTS:backup> $<TARGET_OBJECTS:chunk_patcher> $<TARGET_OBJECTS:client_main>
                               $<TARGET_OBJECTS:continuous_backup> $<TARGET_OBJECTS:file_backup> $<TARGET_OBJECTS:file_index>
                               $<TARGET_OBJECTS:server_file_metadata_download_thread> $<TARGET_OBJECTS:full_file_backup>
                               $<TARGET_OBJECTS:image_backup> $<TARGET_OBJECTS:incr_file_backup>
                               $<TARGET_OBJECTS:internet_service_connector> $<TARGET_OBJECTS:lmdb_file_index>
                               $<TARGET_OBJECTS:server_download_thread> $<TARGET_OBJECTS:throttle_updater>
                               $<TARGET_OBJECTS:wal_checkpoint_thread> $<TARGET_OBJECTS:check_files_index>
                               $<TARGET_OBJECTS:cleanup_cmd> $<TARGET_OBJECTS:export_auth_log> $<TARGET_OBJECTS:md5sum_check>
                               $<TARGET_OBJECTS:repair_cmd> $<TARGET_OBJECTS:skiphash_copy> $<TARGET_OBJECTS:server_dllmain>
                               $<TARGET_OBJECTS:create_files_index>
                               $<TARGET_OBJECTS:server_backup_dao> $<TARGET_OBJECTS:server_cleanup_dao>
                               $<TARGET_OBJECTS:server_files_dao> $<TARGET_OBJECTS:server_link_dao>
                               $<TARGET_OBJECTS:server_link_journal_dao> 
                               $<TARGET_OBJECTS:filedownload> $<TARGET_OBJECTS:restore_client> $<TARGET_OBJECTS:server_server>
                               $<TARGET_OBJECTS:server_archive> $<TARGET_OBJECTS:server_channel>
                               $<TARGET_OBJECTS:server_cleanup> $<TARGET_OBJECTS:server_continuous>
                               $<TARGET_OBJECTS:server_dir_links> $<TARGET_OBJECTS:server_hash>
                               $<TARGET_OBJECTS:server_log> $<TARGET_OBJECTS:server_ping>
                               $<TARGET_OBJECTS:server_prepare_hash> $<TARGET_OBJECTS:server_running>
                               $<TARGET_OBJECTS:server_settings> $<TARGET_OBJECTS:server_status>
                               $<TARGET_OBJECTS:server_update> $<TARGET_OBJECTS:server_update_stats>
                               $<TARGET_OBJECTS:server_writer> $<TARGET_OBJECTS:add_client> $<TARGET_OBJECTS:backups>
                               $<TARGET_OBJECTS:create_zip> $<TARGET_OBJECTS:download_client> $<TARGET_OBJECTS:getimage>
                               $<TARGET_OBJECTS:helper> $<TARGET_OBJECTS:lastacts> $<TARGET_OBJECTS:livelog>
                               $<TARGET_OBJECTS:login> $<TARGET_OBJECTS:logs> $<TARGET_OBJECTS:piegraph>
                               $<TARGET_OBJECTS:progress> $<TARGET_OBJECTS:restore_prepare_wait> $<TARGET_OBJECTS:salt>
                               $<TARGET_OBJECTS:settings> $<TARGET_OBJECTS:shutdown> $<TARGET_OBJECTS:start_backup>
                               $<TARGET_OBJECTS:status> $<TARGET_OBJECTS:usage> $<TARGET_OBJECTS:usagegraph>
                               $<TARGET_OBJECTS:users> $<TARGET_OBJECTS:snapshot_helper> $<TARGET_OBJECTS:treediff>
                               $<TARGET_OBJECTS:treenode> $<TARGET_OBJECTS:treereader> $<TARGET_OBJECTS:verify_hashes>
                               $<TARGET_OBJECTS:mdb> $<TARGET_OBJECTS:midl>)

target_link_libraries(urbackup_server_lib urbackup_common root_main httpserver fsimageplugin fileserv)

add_executable(urbackup_server $<TARGET_OBJECTS:cmdline_preprocessor>)
target_link_libraries(urbackup_server urbackup_server_lib)

