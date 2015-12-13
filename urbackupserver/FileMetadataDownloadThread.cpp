/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011-2015 Martin Raiber
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "FileMetadataDownloadThread.h"
#include "ClientMain.h"
#include "server_log.h"
#include "../urbackupcommon/file_metadata.h"
#include "../common/data.h"
#include <memory>

namespace server
{

const _u32 ID_METADATA_OS_WIN = 1<<0;
const _u32 ID_METADATA_OS_UNIX = 1<<2;
const _u32 ID_METADATA_NOP = 0;
const _u32 ID_METADATA_V1 = 1<<3;

FileMetadataDownloadThread::FileMetadataDownloadThread( FileClient* fc, const std::string& server_token, logid_t logid)
	: fc(fc), server_token(server_token), logid(logid), has_error(false), dry_run(false)
{

}

FileMetadataDownloadThread::FileMetadataDownloadThread(const std::string& server_token, std::string metadata_tmp_fn)
	: fc(NULL), server_token(server_token), has_error(false), metadata_tmp_fn(metadata_tmp_fn), dry_run(true)
{

}

void FileMetadataDownloadThread::operator()()
{
	std::auto_ptr<IFile> tmp_f(ClientMain::getTemporaryFileRetry(true, std::string(), logid));
	
	std::string remote_fn = "SCRIPT|urbackup/FILE_METADATA|"+server_token;

	_u32 rc = fc->GetFile(remote_fn, tmp_f.get(), true, false, false);

	if(rc!=ERR_SUCCESS)
	{
		ServerLogger::Log(logid, "Error getting file metadata. Errorcode: "+FileClient::getErrorString(rc)+" ("+convert(rc)+")", LL_ERROR);
		has_error=true;
	}
	else
	{
		has_error=false;
	}

	metadata_tmp_fn = tmp_f->getFilename();
}

bool FileMetadataDownloadThread::applyMetadata( const std::string& backup_metadata_dir,
	const std::string& backup_dir, INotEnoughSpaceCallback *cb, const std::map<std::string, std::string>& filepath_corrections)
{
	buffer.resize(32768);
	std::auto_ptr<IFile> metadata_f(Server->openFile(metadata_tmp_fn, MODE_READ_SEQUENTIAL));

	if(metadata_f.get()==NULL)
	{
		ServerLogger::Log(logid, "Error opening metadata file. Cannot save file metadata.", LL_ERROR);
		return false;
	}

	ServerLogger::Log(logid, "Saving file metadata...", LL_INFO);

	do 
	{
		char ch;
		if(metadata_f->Read(reinterpret_cast<char*>(&ch), sizeof(ch))!=sizeof(ch))
		{
			bool not_applied=false;
			for(size_t i=0;i<saved_folder_items.size();++i)
			{
				if(saved_folder_items[i].folder_items!=-1)
				{
					not_applied = true;
					break;
				}
			}
			if(not_applied)
			{
				ServerLogger::Log(logid, "Not all folder metadata could be applied. Metadata was inconsistent.", LL_WARNING);
			}
			return true;
		}

		if(ch==ID_METADATA_NOP)
		{
			continue;
		}
		
		if(ch & ID_METADATA_V1)
		{
			unsigned int curr_fn_size =0;
			if(metadata_f->Read(reinterpret_cast<char*>(&curr_fn_size), sizeof(curr_fn_size))!=sizeof(curr_fn_size))
			{
				ServerLogger::Log(logid, "Error saving metadata. Filename size could not be read.", LL_ERROR);
				return false;
			}
			
			curr_fn_size = little_endian(curr_fn_size);

			std::string curr_fn;
			curr_fn.resize(curr_fn_size);

			if(curr_fn_size>1)
			{
				if(metadata_f->Read(&curr_fn[0], static_cast<_u32>(curr_fn.size()))!=curr_fn.size())
				{
					ServerLogger::Log(logid, "Error saving metadata. Filename could not be read. Size: "+convert(curr_fn_size), LL_ERROR);
					return false;
				}
			}
			else
			{
				ServerLogger::Log(logid, "Error saving metadata. Filename is empty.", LL_ERROR);
				return false;
			}				

			bool is_dir = (curr_fn[0]=='d' || curr_fn[0]=='l');
			bool is_dir_symlink = curr_fn[0]=='l';

			std::string os_path_metadata;
			std::string os_path;
			std::vector<std::string> fs_toks;
			TokenizeMail(curr_fn.substr(1), fs_toks, "/");

			std::string curr_path;
			for(size_t i=0;i<fs_toks.size();++i)
			{
				if(fs_toks[i]!="." && fs_toks[i]!="..")
				{
					if(!os_path_metadata.empty())
					{
						os_path_metadata+=os_file_sep();
						os_path+=os_file_sep();
					}

					std::string path_component = (fs_toks[i]);

					curr_path += "/" + path_component;

					str_map::const_iterator it_correction = filepath_corrections.find(curr_path);
					if(it_correction!=filepath_corrections.end())
					{
						path_component = it_correction->second;
					}

					os_path+=path_component;

					if(i==fs_toks.size()-1)
					{
						os_path_metadata += escape_metadata_fn(path_component);

						if(is_dir && !is_dir_symlink)
						{
							os_path_metadata+=os_file_sep()+metadata_dir_fn;
						}
					}
					else
					{						
						os_path_metadata += path_component;
					}					
				}
			}

			std::auto_ptr<IFile> output_f;
			bool new_metadata_file=false;

			if(!dry_run)
			{
				output_f.reset(Server->openFile(os_file_prefix(backup_metadata_dir+os_file_sep()+os_path_metadata), MODE_RW));

				
				if(output_f.get()==NULL)
				{
					output_f.reset(Server->openFile(os_file_prefix(backup_metadata_dir+os_file_sep()+os_path_metadata), MODE_RW_CREATE));
					new_metadata_file=true;
				}

				if(output_f.get()==NULL)
				{
					ServerLogger::Log(logid, "Error saving metadata. Filename could not open output file at \"" + backup_metadata_dir+os_file_sep()+os_path_metadata + "\"", LL_ERROR);
					return false;
				}
			}			

			unsigned int common_metadata_size =0;
			if(metadata_f->Read(reinterpret_cast<char*>(&common_metadata_size), sizeof(common_metadata_size))!=sizeof(common_metadata_size))
			{
				ServerLogger::Log(logid, "Error saving metadata. Common metadata size could not be read.", LL_ERROR);
				return false;
			}
			
			common_metadata_size = little_endian(common_metadata_size);

			std::vector<char> common_metadata;
			common_metadata.resize(common_metadata_size);

			if(metadata_f->Read(&common_metadata[0], static_cast<_u32>(common_metadata.size()))!=common_metadata.size())
			{
				ServerLogger::Log(logid, "Error saving metadata. Common metadata could not be read.", LL_ERROR);
				return false;
			}

			CRData common_data(common_metadata.data(), common_metadata.size());
			char common_version;
			common_data.getChar(&common_version);
			int64 created;
			int64 modified;
			int64 accessed;
			int64 folder_items;
			std::string permissions;
			if(common_version!=1
				|| !common_data.getVarInt(&created)
				|| !common_data.getVarInt(&modified)
				|| !common_data.getVarInt(&accessed)
				|| !common_data.getVarInt(&folder_items)
				|| !common_data.getStr(&permissions) )
			{
				ServerLogger::Log(logid, "Error saving metadata. Cannot parse common metadata.", LL_ERROR);
				return false;
			}

			FileMetadata curr_metadata;
			if(!dry_run && !new_metadata_file && !read_metadata(output_f.get(), curr_metadata))
			{
				ServerLogger::Log(logid, "Error reading current metadata", LL_WARNING);
			}

			curr_metadata.exist=true;
			curr_metadata.created=created;
			curr_metadata.last_modified = modified;
			curr_metadata.file_permissions = permissions;
			curr_metadata.accessed = accessed;

			int64 offset = 0;

			if(!dry_run)
			{
				int64 truncate_to_bytes;
				if(!write_file_metadata(output_f.get(), cb, curr_metadata, true, truncate_to_bytes))
				{
					ServerLogger::Log(logid, "Error saving metadata. Cannot write common metadata.", LL_ERROR);
					return false;
				}

				offset = os_metadata_offset(output_f.get());

				if(offset==-1)
				{
					ServerLogger::Log(logid, "Error saving metadata. Metadata offset cannot be calculated at \"" + backup_metadata_dir+os_file_sep()+os_path_metadata + "\"", LL_ERROR);
					return false;
				}

				if(!output_f->Seek(offset))
				{
					ServerLogger::Log(logid, "Error saving metadata. Could not seek to end of file \"" + backup_metadata_dir+os_file_sep()+os_path_metadata + "\"", LL_ERROR);
					return false;
				}
			}			

			int64 metadata_size=0;
			bool ok=false;
			if(ch & ID_METADATA_OS_WIN)
			{
				ok = applyWindowsMetadata(metadata_f.get(), output_f.get(), metadata_size, cb, offset);
			}
            else if(ch & ID_METADATA_OS_UNIX)
            {
                ok = applyUnixMetadata(metadata_f.get(), output_f.get(), metadata_size, cb, offset);
            }

			if(!ok)
			{
				ServerLogger::Log(logid, "Error saving metadata. Could not save OS specific metadata to \"" + backup_metadata_dir+os_file_sep()+os_path_metadata + "\"", LL_ERROR);
				return false;
			}
			else if(!dry_run && offset+metadata_size<output_f->Size())
			{
				output_f.reset();
				if(!os_file_truncate(os_file_prefix(backup_metadata_dir+os_file_sep()+os_path_metadata),
					offset+metadata_size))
				{
					ServerLogger::Log(logid, "Error saving metadata. Could not truncate file \"" + backup_metadata_dir+os_file_sep()+os_path_metadata + "\"", LL_ERROR);
					return false;
				}
			}

			if(!dry_run && !is_dir && !os_set_file_time(os_file_prefix(backup_dir+os_file_sep()+os_path), created, modified, accessed))
			{
				ServerLogger::Log(logid, "Error setting file time of "+backup_dir+os_file_sep()+os_path, LL_WARNING);
			}

			if(!dry_run)
			{
				addFolderItem(curr_fn.substr(1), backup_dir+os_file_sep()+os_path, is_dir, created, modified, accessed, folder_items);
			}
		}
		else
		{
			ServerLogger::Log(logid, "Error applying meta data. Unknown meta data.", LL_ERROR);
			return false;
		}

	} while (true);

	assert(false);
	return true;
}

namespace
{
	struct WIN32_STREAM_ID_INT
	{
		_u32 dwStreamId;
		_u32 dwStreamAttributes;
		int64 Size;
		_u32 dwStreamNameSize;
	};

	const size_t metadata_id_size = 4+4+8+4;
	const int64 win32_meta_magic = little_endian(0x320FAB3D119DCB4AULL);
    const int64 unix_meta_magic =  little_endian(0xFE4378A3467647F0ULL);
}

bool FileMetadataDownloadThread::applyWindowsMetadata( IFile* metadata_f, IFile* output_f, int64& metadata_size, INotEnoughSpaceCallback *cb, int64 output_offset)
{
	int64 win32_magic_and_size[2];
	win32_magic_and_size[1]=win32_meta_magic;
	
	if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(win32_magic_and_size), sizeof(win32_magic_and_size), cb))
	{
		ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (beg)", LL_ERROR);
		return false;
	}

	metadata_size=sizeof(win32_magic_and_size);

	_u32 stat_data_size;
	if(metadata_f->Read(reinterpret_cast<char*>(&stat_data_size), sizeof(stat_data_size))!=sizeof(stat_data_size))
	{
		ServerLogger::Log(logid, "Error reading stat data size from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
		return false;
	}

	if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&stat_data_size), sizeof(stat_data_size), cb))
	{
		ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (stat_data_size)", LL_ERROR);
		return false;
	}

	metadata_size+=sizeof(_u32);

	stat_data_size = little_endian(stat_data_size);

	if(stat_data_size<1)
	{
		ServerLogger::Log(logid, "stat data size is zero", LL_ERROR);
		return false;
	}

	char version;
	if(metadata_f->Read(&version, 1)!=1)
	{
		ServerLogger::Log(logid, "Error reading windows metadata version from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
		return false;
	}

	if(version!=1)
	{
		ServerLogger::Log(logid, "Unknown windows metadata version +"+convert((int)version)+" in \"" + output_f->getFilename() + "\"", LL_ERROR);
		return false;
	}

	if(!dry_run && !writeRepeatFreeSpace(output_f, &version, sizeof(version), cb))
	{
		ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (ver)", LL_ERROR);
		return false;
	}

	metadata_size+=1;

	std::vector<char> stat_data;
	stat_data.resize(stat_data_size-1);

	if(metadata_f->Read(stat_data.data(), static_cast<_u32>(stat_data.size()))!= stat_data.size())
	{
		ServerLogger::Log(logid, "Error reading windows stat data from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
		return false;
	}

	if(!dry_run && !writeRepeatFreeSpace(output_f, stat_data.data(), stat_data.size(), cb))
	{
		ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (stat_data)", LL_ERROR);
		return false;
	}

	metadata_size+=stat_data.size();
	
	while(true) 
	{
		char cont = 0;
		if(metadata_f->Read(reinterpret_cast<char*>(&cont), sizeof(cont))!=sizeof(cont))
		{
			ServerLogger::Log(logid, "Error reading  \"" + metadata_f->getFilename() + "\"", LL_ERROR);
			return false;
		}

		if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&cont), sizeof(cont), cb))
		{
			ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (cont)", LL_ERROR);
			return false;
		}
		++metadata_size;

		if(cont==0)
		{
			break;
		}
	
		WIN32_STREAM_ID_INT stream_id;

		if(metadata_f->Read(reinterpret_cast<char*>(&stream_id), metadata_id_size)!=metadata_id_size)
		{
			ServerLogger::Log(logid, "Error reading  \"" + metadata_f->getFilename() + "\"", LL_ERROR);
			return false;
		}

		if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&stream_id), metadata_id_size, cb))
		{
			ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\"", LL_ERROR);
			return false;
		}

		metadata_size+=metadata_id_size;

		if(stream_id.dwStreamNameSize>0)
		{
			std::vector<char> stream_name;
			stream_name.resize(stream_id.dwStreamNameSize);

			if(metadata_f->Read(stream_name.data(), static_cast<_u32>(stream_name.size()))!=stream_name.size())
			{
				ServerLogger::Log(logid, "Error reading  \"" + metadata_f->getFilename() + "\" -2", LL_ERROR);
				return false;
			}

			if(!dry_run && !writeRepeatFreeSpace(output_f, stream_name.data(), stream_name.size(), cb))
			{
				ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" -2", LL_ERROR);
				return false;
			}

			metadata_size+=stream_name.size();
		}	

		int64 curr_pos=0;

		while(curr_pos<stream_id.Size)
		{
			_u32 toread = static_cast<_u32>((std::min)(static_cast<int64>(buffer.size()), stream_id.Size-curr_pos));

			if(metadata_f->Read(buffer.data(), toread)!=toread)
			{
				ServerLogger::Log(logid, "Error reading  \"" + metadata_f->getFilename() + "\" -3", LL_ERROR);
				return false;
			}

			if(!dry_run && !writeRepeatFreeSpace(output_f, buffer.data(), toread, cb))
			{
				ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" -3", LL_ERROR);
				return false;
			}

			metadata_size+=toread;

			curr_pos+=toread;
		}
	}

	if(!dry_run && !output_f->Seek(output_offset))
	{
		ServerLogger::Log(logid, "Error seeking to  \"" + convert(output_offset) + "\" -5 in output_f", LL_ERROR);
		return false;
	}

	win32_magic_and_size[0]=little_endian(metadata_size);

	if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&win32_magic_and_size[0]), sizeof(win32_magic_and_size[0]), cb))
	{
		ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (end)", LL_ERROR);
		return false;
	}

	return true;
}

bool FileMetadataDownloadThread::applyUnixMetadata(IFile* metadata_f, IFile* output_f, int64& metadata_size, INotEnoughSpaceCallback* cb, int64 output_offset)
{
    int64 unix_magic_and_size[2];
    unix_magic_and_size[1]=unix_meta_magic;

    if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(unix_magic_and_size), sizeof(unix_magic_and_size), cb))
    {
        ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (beg, unix)", LL_ERROR);
        return false;
    }

    metadata_size=sizeof(unix_magic_and_size);

	_u32 stat_data_size;
	if(metadata_f->Read(reinterpret_cast<char*>(&stat_data_size), sizeof(stat_data_size))!=sizeof(stat_data_size))
	{
		ServerLogger::Log(logid, "Error reading stat data size from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
		return false;
	}

	if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&stat_data_size), sizeof(stat_data_size), cb))
	{
		ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (stat_data_size)", LL_ERROR);
		return false;
	}

	metadata_size+=sizeof(_u32);

	stat_data_size = little_endian(stat_data_size);

	if(stat_data_size<1)
	{
		ServerLogger::Log(logid, "stat data size is zero", LL_ERROR);
		return false;
	}

    char version;
    if(metadata_f->Read(&version, 1)!=1)
    {
        ServerLogger::Log(logid, "Error reading unix metadata version from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
        return false;
    }

    if(version!=1)
    {
        ServerLogger::Log(logid, "Unknown unix metadata version +"+convert((int)version)+" in \"" + output_f->getFilename() + "\"", LL_ERROR);
        return false;
    }

    if(!dry_run && !writeRepeatFreeSpace(output_f, &version, sizeof(version), cb))
    {
        ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (ver)", LL_ERROR);
        return false;
    }

    metadata_size+=1;

    std::vector<char> stat_data;
	stat_data.resize(stat_data_size-1);

    if(metadata_f->Read(stat_data.data(), static_cast<_u32>(stat_data.size()))!= stat_data.size())
    {
        ServerLogger::Log(logid, "Error reading stat data from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
        return false;
    }

    if(!dry_run && !writeRepeatFreeSpace(output_f, stat_data.data(), stat_data.size(), cb))
    {
        ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (stat_data)", LL_ERROR);
        return false;
    }

    metadata_size+=stat_data.size();

    int64 num_eattr_keys;
    if(metadata_f->Read(reinterpret_cast<char*>(&num_eattr_keys), sizeof(num_eattr_keys))!=sizeof(num_eattr_keys))
    {
        ServerLogger::Log(logid, "Error reading eattr num from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
        return false;
    }

    if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&num_eattr_keys), sizeof(num_eattr_keys), cb))
    {
        ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (num_eattr_keys)", LL_ERROR);
        return false;
    }

    metadata_size+=sizeof(num_eattr_keys);

    num_eattr_keys = little_endian(num_eattr_keys);

    for(int64 i=0;i<num_eattr_keys;++i)
    {
        unsigned int key_size;
        if(metadata_f->Read(reinterpret_cast<char*>(&key_size), sizeof(key_size))!=sizeof(key_size))
        {
            ServerLogger::Log(logid, "Error reading eattr key size from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
            return false;
        }

        if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&key_size), sizeof(key_size), cb))
        {
            ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (key_size)", LL_ERROR);
            return false;
        }

        metadata_size+=sizeof(key_size);
        key_size = little_endian(key_size);

        std::string eattr_key;
        eattr_key.resize(key_size);

        if(metadata_f->Read(&eattr_key[0], key_size)!=key_size)
        {
            ServerLogger::Log(logid, "Error reading eattr key from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
            return false;
        }

        if(!dry_run && !writeRepeatFreeSpace(output_f, eattr_key.data(), eattr_key.size(), cb))
        {
            ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (eattr_key)", LL_ERROR);
            return false;
        }

        metadata_size+=eattr_key.size();

        unsigned int val_size;
        if(metadata_f->Read(reinterpret_cast<char*>(&val_size), sizeof(val_size))!=sizeof(val_size))
        {
            ServerLogger::Log(logid, "Error reading eattr value size from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
            return false;
        }

        if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&val_size), sizeof(val_size), cb))
        {
            ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (val_size)", LL_ERROR);
            return false;
        }

        metadata_size+=sizeof(val_size);
        val_size = little_endian(val_size);

        std::string eattr_val;
        eattr_val.resize(val_size);

        if(metadata_f->Read(&eattr_val[0], val_size)!=val_size)
        {
            ServerLogger::Log(logid, "Error reading eattr value from \"" + metadata_f->getFilename() + "\"", LL_ERROR);
            return false;
        }

        if(!dry_run && !writeRepeatFreeSpace(output_f, eattr_val.data(), eattr_val.size(), cb))
        {
            ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (eattr_val)", LL_ERROR);
            return false;
        }

        metadata_size+=eattr_val.size();
    }

    if(!dry_run && !output_f->Seek(output_offset))
    {
        ServerLogger::Log(logid, "Error seeking to  \"" + convert(output_offset) + "\" -6 in output_f", LL_ERROR);
        return false;
    }

    unix_magic_and_size[0]=little_endian(metadata_size);

    if(!dry_run && !writeRepeatFreeSpace(output_f, reinterpret_cast<char*>(&unix_magic_and_size[0]), sizeof(unix_magic_and_size[0]), cb))
    {
        ServerLogger::Log(logid, "Error writing to  \"" + output_f->getFilename() + "\" (end,unix)", LL_ERROR);
        return false;
    }

    return true;
}

bool FileMetadataDownloadThread::getHasError()
{
	return has_error;
}

FileMetadataDownloadThread::~FileMetadataDownloadThread()
{
	if(!dry_run && !metadata_tmp_fn.empty())
	{
		Server->deleteFile(metadata_tmp_fn);
	}
}

void FileMetadataDownloadThread::shutdown()
{
	fc->Shutdown();
}

void FileMetadataDownloadThread::addFolderItem(std::string path, const std::string& os_path, bool is_dir, int64 created, int64 modified, int64 accessed, int64 folder_items)
{
	std::vector<std::string> toks;
	TokenizeMail(path, toks, "/");

	std::string curr_path;
	for(size_t i=0;i<toks.size()-1;++i)
	{
		if(!curr_path.empty())
		{
			curr_path+="/";
		}
		curr_path+=toks[i];
		addSingleFileItem(curr_path);
	}
	
	if(is_dir)
	{
		if(folder_items==0)
		{
			if(!os_set_file_time(os_file_prefix(os_path), created, modified, accessed))
			{
				ServerLogger::Log(logid, "Error setting file time of "+os_path, LL_WARNING);
			}
			return;
		}

		for(size_t i=0;i<saved_folder_items.size();++i)
		{
			if(saved_folder_items[i].path==path)
			{
				if(saved_folder_items[i].counted_items==folder_items)
				{
					if(!os_set_file_time(os_file_prefix(os_path), created, modified, accessed))
					{
						ServerLogger::Log(logid, "Error setting file time of "+os_path, LL_WARNING);
					}
					saved_folder_items.erase(saved_folder_items.begin()+i);
				}
				else
				{
					saved_folder_items[i].folder_items = folder_items;
					saved_folder_items[i].os_path = os_path;
					saved_folder_items[i].accessed = accessed;
					saved_folder_items[i].created = created;
					saved_folder_items[i].modified = modified;
				}
				return;
			}
		}
	}
	
}

void FileMetadataDownloadThread::addSingleFileItem( std::string dir_path )
{
	for(size_t i=0;i<saved_folder_items.size();++i)
	{
		if(saved_folder_items[i].path==dir_path)
		{
			++saved_folder_items[i].counted_items;

			if(saved_folder_items[i].counted_items==saved_folder_items[i].folder_items)
			{
				if(!os_set_file_time(os_file_prefix(saved_folder_items[i].os_path), saved_folder_items[i].created, saved_folder_items[i].modified, saved_folder_items[i].accessed))
				{
					ServerLogger::Log(logid, "Error setting file time of "+saved_folder_items[i].os_path, LL_WARNING);
				}
				saved_folder_items.erase(saved_folder_items.begin()+i);
			}

			return;
		}
	}

	SFolderItem new_folder_item;
	new_folder_item.counted_items=1;
	new_folder_item.path = dir_path;
	saved_folder_items.push_back(new_folder_item);
}

int check_metadata()
{
	std::string metadata_file = Server->getServerParameter("metadata_file");

	std::string dummy_server_token;
	FileMetadataDownloadThread metadata_thread(dummy_server_token, (metadata_file));

	str_map corrections;
	return metadata_thread.applyMetadata(std::string(), std::string(), NULL, corrections)?0:1;
}

} //namespace server
