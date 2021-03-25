/**
 * @file	FileInfo.cpp
 * @class	FileInfo
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/27
 * @brief	FileInfo
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#include "FileInfo.h"

#include <stdlib.h>
#include <string.h>

namespace JsCPPUtils
{

	FileInfo::FileInfo(const char *szFilePath)
	{
		init(szFilePath);
	}
	
	FileInfo::FileInfo(std::string strFileName)
	{
		init(strFileName.c_str());
	}

	FileInfo::~FileInfo()
	{
		
	}
	
	void FileInfo::init(const char *szFilePath)
	{
		#if defined(JSCUTILS_OS_WINDOWS)
		char szRealPath[MAX_PATH] = {0};
		GetFullPathNameA(szFilePath, MAX_PATH, szRealPath, NULL);
		memset(&m_fileAttr, 0, sizeof(m_fileAttr));
		if(GetFileAttributesExA(szRealPath, GetFileExInfoStandard, (LPVOID)&m_fileAttr))
		{
			m_fileExists = true;
		}else{
			m_fileExists = false;
		}
#elif defined(JSCUTILS_OS_LINUX)
		char szRealPath[PATH_MAX] = {0};
		if(realpath(szFilePath, szRealPath) == NULL)
		{
			m_fileExists = false;
		}else{
			memset(&m_fileStat, 0, sizeof(m_fileStat));
			if(stat(szRealPath, &m_fileStat) == 0)
			{
				m_fileExists = true;
			}else{
				m_fileExists = false;
			}
		}
#endif
		m_fileFullPath = std::string(szRealPath);
	}

	bool FileInfo::exists()
	{
		return m_fileExists;
	}

	std::string FileInfo::getFullFilePath()
	{
		return m_fileFullPath;
	}

#if defined(JSCUTILS_OS_WINDOWS)
	int64_t FileInfo::getCreationTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftCreationTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftCreationTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x / 10000000LL;
	}

	int64_t FileInfo::getLastAccessTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastAccessTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastAccessTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x / 10000000LL;
	}

	int64_t FileInfo::getLastWriteTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastWriteTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastWriteTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x / 10000000LL;
	}

	int64_t FileInfo::getCreationWinFileTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftCreationTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftCreationTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x;
	}

	int64_t FileInfo::getLastAccessWinFileTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastAccessTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastAccessTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x;
	}

	int64_t FileInfo::getLastWriteWinFileTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastWriteTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastWriteTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x;
	}

	uint64_t FileInfo::getFileSize()
	{
		uint64_t x;
		x = m_fileAttr.nFileSizeLow;
		x |= ((uint64_t)m_fileAttr.nFileSizeHigh)<<32;
		return x;
	}

#elif defined(JSCUTILS_OS_LINUX)
	int64_t FileInfo::getCreationTime() const
	{
		return m_fileStat.st_atime;
	}

	int64_t FileInfo::getLastAccessTime() const
	{
		return m_fileStat.st_ctime;
	}

	int64_t FileInfo::getLastWriteTime() const
	{
		return m_fileStat.st_mtime;
	}

	int64_t FileInfo::getCreationWinFileTime()
	{
		return getCreationTime() * 10000000LL;
	}

	int64_t FileInfo::getLastAccessWinFileTime()
	{
		return getLastAccessTime() * 10000000LL;
	}

	int64_t FileInfo::getLastWriteWinFileTime()
	{
		return getLastWriteTime() * 10000000LL;
	}

	uint64_t FileInfo::getFileSize()
	{
		return m_fileStat.st_size;
	}

#endif


	

}