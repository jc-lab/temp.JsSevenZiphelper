/**
 * @file	FileMaker.cpp
 * @class	FileMaker
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/07
 * @brief	FileMaker
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#include "FileMaker.h"

#include <stdlib.h>
#include <string.h>

#include "../JsCPPUtils/StringEncoding.h"

namespace JsCPPUtils
{
	FileMaker::FileMaker() : 
		m_hFile(INVALID_HANDLE_VALUE)
	{
		memset(&m_fileAttr, 0, sizeof(m_fileAttr));
	}
		
	FileMaker::FileMaker(HANDLE hFile)
	{
		char szRealPath[MAX_PATH] = {0};
		FILE_NAME_INFO filenameInfo;

		m_hFile = hFile;

		memset(&filenameInfo, 0, sizeof(filenameInfo));
		if(!GetFileInformationByHandleEx(hFile, FileNameInfo, &filenameInfo, sizeof(filenameInfo)))
		{
			m_dwErr = GetLastError();
		}else{
			m_fileFullPath = JsCPPUtils::StringEncoding::UnicodeToString(filenameInfo.FileName, filenameInfo.FileNameLength);
		}
	}

	FileMaker::~FileMaker()
	{
		close();
	}

	bool FileMaker::create(const char *szFilePath, bool bNoOverwrite, bool isDirectory)
	{
		bool retval;
		char szRealPath[MAX_PATH] = {0};
		
		GetFullPathNameA(szFilePath, MAX_PATH, szRealPath, NULL);

		m_hFile = CreateFileA(szRealPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, bNoOverwrite ? CREATE_NEW : CREATE_ALWAYS, isDirectory ? FILE_ATTRIBUTE_DIRECTORY : 0, NULL);
		if(m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwErr = GetLastError();
			retval = false;
		}else
			retval = true;

		m_fileFullPath = std::string(szRealPath);

		return retval;
	}

	void FileMaker::close()
	{
		if(m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	
	bool FileMaker::save()
	{
		bool retval;
		m_dwErr = 0;
		//return SetFileAttributesA(m_fileFullPath.c_str(), m_fileAttr.dwFileAttributes) ? true : false;
		retval = SetFileTime(m_hFile, &m_fileAttr.ftCreationTime, &m_fileAttr.ftLastAccessTime, &m_fileAttr.ftLastWriteTime) ? true : false;
		if(!retval)
		{
			m_dwErr = GetLastError();
		}
		return retval;
	}

	HANDLE FileMaker::getFileHandle()
	{
		return m_hFile;
	}

	DWORD FileMaker::getErrno()
	{
		return m_dwErr;
	}
	
	std::string FileMaker::getFullFilePath()
	{
		return m_fileFullPath;
	}

	int64_t FileMaker::getCreationTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftCreationTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftCreationTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x / 10000000LL - 11644473600LL;
	}

	int64_t FileMaker::getLastAccessTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastAccessTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastAccessTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x / 10000000LL - 11644473600LL;
	}

	int64_t FileMaker::getLastWriteTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastWriteTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastWriteTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x / 10000000LL - 11644473600LL;
	}

	int64_t FileMaker::getCreationWinFileTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftCreationTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftCreationTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x;
	}

	int64_t FileMaker::getLastAccessWinFileTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastAccessTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastAccessTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x;
	}

	int64_t FileMaker::getLastWriteWinFileTime()
	{
		int64_t x;
		x = (((int64_t)m_fileAttr.ftLastWriteTime.dwHighDateTime) & 0xFFFFFFFFLL) << 32;
		x |= (((int64_t)m_fileAttr.ftLastWriteTime.dwLowDateTime) & 0xFFFFFFFFLL);
		return x;
	}

	uint64_t FileMaker::getFileSize()
	{
		uint64_t x;
		x = m_fileAttr.nFileSizeLow;
		x |= ((uint64_t)m_fileAttr.nFileSizeHigh)<<32;
		return x;
	}

	void FileMaker::setCreationTime(int64_t time)
	{
		time += 11644473600LL;
		time *= 10000000LL;
		m_fileAttr.ftCreationTime.dwHighDateTime = (DWORD)((time >> 32) & 0xFFFFFFFF);
		m_fileAttr.ftCreationTime.dwLowDateTime = (DWORD)(time & 0xFFFFFFFF);
	}

	void FileMaker::setLastAccessTime(int64_t time)
	{
		time += 11644473600LL;
		time *= 10000000LL;
		m_fileAttr.ftLastAccessTime.dwHighDateTime = (DWORD)((time >> 32) & 0xFFFFFFFF);
		m_fileAttr.ftLastAccessTime.dwLowDateTime = (DWORD)(time & 0xFFFFFFFF);
	}

	void FileMaker::setLastWriteTime(int64_t time)
	{
		time += 11644473600LL;
		time *= 10000000LL;
		m_fileAttr.ftLastWriteTime.dwHighDateTime = (DWORD)((time >> 32) & 0xFFFFFFFF);
		m_fileAttr.ftLastWriteTime.dwLowDateTime = (DWORD)(time & 0xFFFFFFFF);
	}

	void FileMaker::setCreationWinFileTime(int64_t time)
	{
		m_fileAttr.ftCreationTime.dwHighDateTime = (DWORD)((time >> 32) & 0xFFFFFFFF);
		m_fileAttr.ftCreationTime.dwLowDateTime = (DWORD)(time & 0xFFFFFFFF);
	}

	void FileMaker::setLastAccessWinFileTime(int64_t time)
	{
		m_fileAttr.ftLastAccessTime.dwHighDateTime = (DWORD)((time >> 32) & 0xFFFFFFFF);
		m_fileAttr.ftLastAccessTime.dwLowDateTime = (DWORD)(time & 0xFFFFFFFF);
	}

	void FileMaker::setLastWriteWinFileTime(int64_t time)
	{
		m_fileAttr.ftLastWriteTime.dwHighDateTime = (DWORD)((time >> 32) & 0xFFFFFFFF);
		m_fileAttr.ftLastWriteTime.dwLowDateTime = (DWORD)(time & 0xFFFFFFFF);
	}

	void FileMaker::setCreationWinFileTime(FILETIME *pFileTime)
	{
		m_fileAttr.ftCreationTime.dwHighDateTime = pFileTime->dwHighDateTime;
		m_fileAttr.ftCreationTime.dwLowDateTime = pFileTime->dwLowDateTime;
	}

	void FileMaker::setLastAccessWinFileTime(FILETIME *pFileTime)
	{
		m_fileAttr.ftLastAccessTime.dwHighDateTime = pFileTime->dwHighDateTime;
		m_fileAttr.ftLastAccessTime.dwLowDateTime = pFileTime->dwLowDateTime;
	}

	void FileMaker::setLastWriteWinFileTime(FILETIME *pFileTime)
	{
		m_fileAttr.ftLastWriteTime.dwHighDateTime = pFileTime->dwHighDateTime;
		m_fileAttr.ftLastWriteTime.dwLowDateTime = pFileTime->dwLowDateTime;
	}

}