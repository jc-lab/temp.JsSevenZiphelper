/**
 * @file	WinLibrary.cpp
 * @class	WinLibrary
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/27
 * @brief	Windows Library Load Helper
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#include "WinLibrary.h"

namespace JsCPPUtils
{
	WinLibrary::WinLibrary() : 
		m_hFile(INVALID_HANDLE_VALUE), m_bAutoCloseFileHandle(false)
	{
		m_hModule = NULL;
		m_dwErr = 0;
	}

	WinLibrary::~WinLibrary()
	{
		if(m_hModule != NULL)
		{
			::FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
		if(m_bAutoCloseFileHandle)
		{
			if(m_hFile != INVALID_HANDLE_VALUE)
				CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	WinLibrary::WinLibrary(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags) : 
		m_hFile(INVALID_HANDLE_VALUE), m_bAutoCloseFileHandle(false)
	{
		m_hModule = ::LoadLibraryExA(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
		}else{
			m_dwErr = 0;
		}
	}

	WinLibrary::WinLibrary(LPWSTR szFilePath, HANDLE hFile, DWORD dwFlags) : 
		m_hFile(INVALID_HANDLE_VALUE), m_bAutoCloseFileHandle(false)
	{
		m_hModule = ::LoadLibraryExW(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
		}else{
			m_dwErr = 0;
		}
	}
	
	WinLibrary::WinLibrary(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle) : 
		m_hFile(hFile), m_bAutoCloseFileHandle(bAutoCloseFileHandle)
	{
		m_hModule = ::LoadLibraryExA(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
		}else{
			m_dwErr = 0;
		}
	}

	WinLibrary::WinLibrary(LPWSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle) : 
		m_hFile(hFile), m_bAutoCloseFileHandle(bAutoCloseFileHandle)
	{
		m_hModule = ::LoadLibraryExW(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
		}else{
			m_dwErr = 0;
		}
	}
		
	bool WinLibrary::load(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags)
	{
		m_hFile = INVALID_HANDLE_VALUE;
		m_bAutoCloseFileHandle = false;
		m_hModule = ::LoadLibraryExA(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
			return false;
		}else{
			m_dwErr = 0;
			return true;
		}
	}

	bool WinLibrary::load(LPCWSTR szFilePath, HANDLE hFile, DWORD dwFlags)
	{
		m_hFile = hFile;
		m_bAutoCloseFileHandle = false;
		m_hModule = ::LoadLibraryExW(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
			return false;
		}else{
			m_dwErr = 0;
			return true;
		}
	}
	
	bool WinLibrary::load(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle)
	{
		m_hFile = hFile;
		m_bAutoCloseFileHandle = bAutoCloseFileHandle;
		m_hModule = ::LoadLibraryExA(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
			return false;
		}else{
			m_dwErr = 0;
			return true;
		}
	}

	bool WinLibrary::load(LPCWSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle)
	{
		m_hFile = hFile;
		m_bAutoCloseFileHandle = bAutoCloseFileHandle;
		m_hModule = ::LoadLibraryExW(szFilePath, hFile, dwFlags);
		if(m_hModule == NULL)
		{
			m_dwErr = GetLastError();
			return false;
		}else{
			m_dwErr = 0;
			return true;
		}
	}

	bool WinLibrary::isLoaded() const
	{
		return (m_hModule != NULL);
	}

	DWORD WinLibrary::getErrno() const
	{
		return m_dwErr;
	}

	FARPROC WinLibrary::GetProcAddress(LPCSTR lpProcName)
	{
		if(m_hModule == NULL)
			return NULL;

		return ::GetProcAddress(m_hModule, lpProcName);
	}

}
