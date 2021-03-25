/**
 * @file	FileInfo.h
 * @class	FileInfo
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/27
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <string>

#include "Common.h"

#if defined(JSCUTILS_OS_WINDOWS)
#include <windows.h>
#elif defined(JSCUTILS_OS_LINUX)
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace JsCPPUtils
{
	// For Linux, should CHARTYPE is char
	template<typename CHARTYPE>
	class basic_FileInfo
	{
	private:
		std::basic_string<CHARTYPE> m_fileFullPath;
		bool m_fileExists;

#if defined(JSCUTILS_OS_WINDOWS)
		WIN32_FILE_ATTRIBUTE_DATA m_fileAttr;
#elif defined(JSCUTILS_OS_LINUX)
		struct stat m_fileStat;
#endif
		
		void init(const CHARTYPE *szFilePath)
		{
			#if defined(JSCUTILS_OS_WINDOWS)
			BOOL bRst;
			CHARTYPE szRealPath[MAX_PATH] = {0};
			memset(&m_fileAttr, 0, sizeof(m_fileAttr));

			if(typeid(CHARTYPE) == typeid(wchar_t))
			{
				GetFullPathNameW((LPCWSTR)szFilePath, MAX_PATH, (LPWSTR)szRealPath, NULL);
				bRst = GetFileAttributesExW((LPCWSTR)szRealPath, GetFileExInfoStandard, (LPVOID)&m_fileAttr);
			}else{
				GetFullPathNameA((LPCSTR)szFilePath, MAX_PATH, (LPSTR)szRealPath, NULL);
				bRst = GetFileAttributesExA((LPCSTR)szRealPath, GetFileExInfoStandard, (LPVOID)&m_fileAttr);
			}

			if(bRst)
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
			m_fileFullPath = szRealPath;
		}

	public:
		basic_FileInfo(const CHARTYPE *szFilePath)
		{
			init(szFilePath);
		}
	
		basic_FileInfo(const std::basic_string<CHARTYPE> &strFileName)
		{
			init(strFileName.c_str());
		}

		~basic_FileInfo()
		{
		
		}
	
		bool exists()
		{
			return m_fileExists;
		}

		std::basic_string<CHARTYPE> getFullFilePath()
		{
			return m_fileFullPath;
		}

#if defined(JSCUTILS_OS_WINDOWS)
		int64_t getCreationTime()
		{
			int64_t x;
			x = (((int64_t)m_fileAttr.ftCreationTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
			x |= (((int64_t)m_fileAttr.ftCreationTime.dwLowDateTime) & 0xFFFFFFFFLL);
			return x / 10000000LL;
		}

		int64_t getLastAccessTime()
		{
			int64_t x;
			x = (((int64_t)m_fileAttr.ftLastAccessTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
			x |= (((int64_t)m_fileAttr.ftLastAccessTime.dwLowDateTime) & 0xFFFFFFFFLL);
			return x / 10000000LL;
		}

		int64_t getLastWriteTime()
		{
			int64_t x;
			x = (((int64_t)m_fileAttr.ftLastWriteTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
			x |= (((int64_t)m_fileAttr.ftLastWriteTime.dwLowDateTime) & 0xFFFFFFFFLL);
			return x / 10000000LL;
		}

		int64_t getCreationWinFileTime()
		{
			int64_t x;
			x = (((int64_t)m_fileAttr.ftCreationTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
			x |= (((int64_t)m_fileAttr.ftCreationTime.dwLowDateTime) & 0xFFFFFFFFLL);
			return x;
		}

		int64_t getLastAccessWinFileTime()
		{
			int64_t x;
			x = (((int64_t)m_fileAttr.ftLastAccessTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
			x |= (((int64_t)m_fileAttr.ftLastAccessTime.dwLowDateTime) & 0xFFFFFFFFLL);
			return x;
		}

		int64_t getLastWriteWinFileTime()
		{
			int64_t x;
			x = (((int64_t)m_fileAttr.ftLastWriteTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
			x |= (((int64_t)m_fileAttr.ftLastWriteTime.dwLowDateTime) & 0xFFFFFFFFLL);
			return x;
		}

		uint64_t getFileSize()
		{
			uint64_t x;
			x = m_fileAttr.nFileSizeLow;
			x |= ((uint64_t)m_fileAttr.nFileSizeHigh)<<32;
			return x;
		}

#elif defined(JSCUTILS_OS_LINUX)
		int64_t getCreationTime() const
		{
			return m_fileStat.st_atime;
		}

		int64_t getLastAccessTime() const
		{
			return m_fileStat.st_ctime;
		}

		int64_t getLastWriteTime() const
		{
			return m_fileStat.st_mtime;
		}

		int64_t getCreationWinFileTime()
		{
			return getCreationTime() * 10000000LL;
		}

		int64_t getLastAccessWinFileTime()
		{
			return getLastAccessTime() * 10000000LL;
		}

		int64_t getLastWriteWinFileTime()
		{
			return getLastWriteTime() * 10000000LL;
		}

		uint64_t getFileSize()
		{
			return m_fileStat.st_size;
		}

#endif
	};


#if defined(JSCUTILS_OS_WINDOWS)
	typedef basic_FileInfo<TCHAR> FileInfo;
	typedef basic_FileInfo<char> FileInfoA;
	typedef basic_FileInfo<wchar_t> FileInfoW;
#else
	typedef basic_FileInfo<char> FileInfo;
#endif
}
