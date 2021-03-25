/**
 * @file	WinLibrary.h
 * @class	WinLibrary
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/27
 * @brief	Windows Library Load Helper
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <string>

#include <windows.h>

namespace JsCPPUtils
{

	class WinLibrary
	{
	private:
		HMODULE m_hModule;
		DWORD   m_dwErr;
		HANDLE  m_hFile;
		bool    m_bAutoCloseFileHandle;

	public:
		WinLibrary();
		~WinLibrary();
		WinLibrary(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags);
		WinLibrary(LPWSTR szFilePath, HANDLE hFile, DWORD dwFlags);
		WinLibrary(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle);
		WinLibrary(LPWSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle);
		bool load(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags);
		bool load(LPCWSTR szFilePath, HANDLE hFile, DWORD dwFlags);
		bool load(LPCSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle);
		bool load(LPCWSTR szFilePath, HANDLE hFile, DWORD dwFlags, bool bAutoCloseFileHandle);
		bool isLoaded() const;
		DWORD getErrno() const;
		FARPROC GetProcAddress(LPCSTR lpProcName);
	};

}