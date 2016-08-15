cmake_minimum_required(VERSION 2.8.11)

project(fsimageplugin)

if(WIN32)
    add_library(ntfs OBJECT fs/ntfs_win.cpp)
else()
    add_library(ntfs OBJECT fs/ntfs.cpp)
endif()

add_library(client_bitmap OBJECT ClientBitmap.cpp)
add_library(compressed_file OBJECT CompressedFile.cpp)
add_library(fs_image_factory OBJECT FSImageFactory.cpp)
add_library(file_wrapper OBJECT FileWrapper.cpp)
add_library(lru_mem_cache OBJECT LRUMemCache.cpp)
add_library(cowfile OBJECT cowfile.cpp)
add_library(fs_dllmain OBJECT dllmain.cpp)
add_library(filesystem OBJECT filesystem.cpp)
add_library(fs_pluginmgr OBJECT pluginmgr.cpp)
add_library(vhdfile OBJECT vhdfile.cpp)
add_library(unknown_fs OBJECT fs/unknown.cpp)

add_library(fsimageplugin SHARED $<TARGET_OBJECTS:ntfs> $<TARGET_OBJECTS:client_bitmap> $<TARGET_OBJECTS:compressed_file>
                          $<TARGET_OBJECTS:fs_image_factory> $<TARGET_OBJECTS:file_wrapper> $<TARGET_OBJECTS:lru_mem_cache>
                          $<TARGET_OBJECTS:cowfile> $<TARGET_OBJECTS:fs_dllmain> $<TARGET_OBJECTS:filesystem>
                          $<TARGET_OBJECTS:fs_pluginmgr> $<TARGET_OBJECTS:vhdfile> $<TARGET_OBJECTS:unknown_fs>)

target_link_libraries(fsimageplugin urbackup_common root_library)

