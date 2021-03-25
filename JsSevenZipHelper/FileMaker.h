/**
 * @file	CreateFileInfo.h
 * @class	CreateFileInfo
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/07
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <string>

#include <JsCPPUtils/Common.h>

#include <windows.h>
#include <tchar.h>

namespace JsCPPUtils
{
	class FileMaker
	{
	private:
		bool m_fileExists;

		HANDLE m_hFile;
		WIN32_FILE_ATTRIBUTE_DATA m_fileAttr;

		DWORD m_dwErr;

	public:
		FileMaker();
		FileMaker(HANDLE hFile);
		~FileMaker();

		bool create(const char *szFilePath, bool bNoOverwrite, bool isDirectory);
		bool create(const wchar_t *szFilePath, bool bNoOverwrite, bool isDirectory);
		void close();
		bool save();
		DWORD getErrno();
		
		//std::basic_string<TCHAR> getFullFilePath();

		int64_t getCreationTime();
		int64_t getLastAccessTime();
		int64_t getLastWriteTime();
		int64_t getCreationWinFileTime();
		int64_t getLastAccessWinFileTime();
		int64_t getLastWriteWinFileTime();
		void setCreationTime(int64_t time);
		void setLastAccessTime(int64_t time);
		void setLastWriteTime(int64_t time);
		void setCreationWinFileTime(int64_t time);
		void setLastAccessWinFileTime(int64_t time);
		void setLastWriteWinFileTime(int64_t time);

		void setCreationWinFileTime(FILETIME *pFiletime);
		void setLastAccessWinFileTime(FILETIME *pFiletime);
		void setLastWriteWinFileTime(FILETIME *pFiletime);

		HANDLE getFileHandle();

		uint64_t getFileSize();

	};

}
