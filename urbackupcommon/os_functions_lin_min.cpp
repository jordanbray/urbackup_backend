/*************************************************************************
*    UrBackup - Client/Server backup system
*    Copyright (C) 2011-2016 Martin Raiber
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

#include "os_functions.h"
#include "../stringtools.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(__FreeBSD__)
#define open64 open
#endif


void getMousePos(int &x, int &y)
{
	x=0;
	y=0;
}

bool os_create_reflink(const std::string &linkname, const std::string &fname)
{
#ifndef sun
	int src_desc=open64(fname.c_str(), O_RDONLY);
	if( src_desc<0)
	    return false;

	int dst_desc=open64(linkname.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG);
	if( dst_desc<0 )
	{
	    close(src_desc);
	    return false;
	}

#define BTRFS_IOCTL_MAGIC 0x94
#define BTRFS_IOC_CLONE _IOW (BTRFS_IOCTL_MAGIC, 9, int)
	
	int rc=ioctl(dst_desc, BTRFS_IOC_CLONE, src_desc);

	close(src_desc);
	close(dst_desc);
	
	return rc==0;
#else
	return false;
#endif
}

bool os_create_hardlink(const std::string &linkname, const std::string &fname, bool ioref, bool* too_many_links)
{
	if(too_many_links!=NULL)
		*too_many_links=false;

	if(ioref)
	    return os_create_reflink(linkname, fname);

	int rc=link(fname.c_str(), linkname.c_str());
	return rc==0;
}

std::string os_file_sep(void)
{
	return "/";
}

bool os_remove_dir(const std::string &path)
{
	return rmdir(path.c_str())==0;
}

bool os_create_dir(const std::string &path)
{
	return mkdir(path.c_str(), S_IRWXU | S_IRWXG)==0;
}

