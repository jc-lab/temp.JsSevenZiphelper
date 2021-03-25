/**
 * @file	Archive.cpp
 * @class	Archive
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/28
 * @brief	JsSevenZipHelper Archive class
 */

#include <iostream>

#include <atlbase.h>

#include "Archive.h"

namespace JsSevenZipHelper
{
	
	Archive::Archive()
	{
#if defined(WIN64) || defined(_WIN64)
		m_sevenZipLibSP = new JsCPPUtils::WinLibrary("7z64.dll", NULL, 0),
#else
		m_sevenZipLibSP = new JsCPPUtils::WinLibrary("7z32.dll", NULL, 0),
#endif
		m_sevenZipLibPtr = m_sevenZipLibSP.getPtr();
		postLibLoaded();
	}

	
	Archive::Archive(const Archive& refobj)
	{
		m_sevenZipLibPtr = refobj.m_sevenZipLibPtr;
		m_sevenZipLibSP = refobj.m_sevenZipLibSP;
		postLibLoaded();
	}

	Archive::Archive(JsCPPUtils::WinLibrary *pSevenZipLib)
	{
		m_sevenZipLibSP = NULL;
		m_sevenZipLibPtr = pSevenZipLib;
		postLibLoaded();
	}

	Archive::Archive(JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> spSevenZipLib)
	{
		m_sevenZipLibSP = spSevenZipLib;
		m_sevenZipLibPtr = spSevenZipLib.getPtr();
		postLibLoaded();
	}

	Archive::~Archive()
	{
		
	}

	void Archive::setSevenLib(JsCPPUtils::WinLibrary *pSevenZipLib)
	{
		m_sevenZipLibSP = NULL;
		m_sevenZipLibPtr = pSevenZipLib;
		postLibLoaded();
	}

	void Archive::setSevenLib(JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> spSevenZipLib)
	{
		m_sevenZipLibSP = spSevenZipLib;
		m_sevenZipLibPtr = spSevenZipLib.getPtr();
		postLibLoaded();
	}

	bool Archive::isLibLoaded() const
	{
		return m_sevenZipLibPtr->isLoaded();
	}

	DWORD Archive::getLibErrno() const
	{
		return m_sevenZipLibPtr->getErrno();
	}

	void Archive::postLibLoaded()
	{
		if(!isLibLoaded())
		{
			m_fpCreateObject = NULL;
			return ;
		}

		m_fpCreateObject = (CreateObjectFunc)m_sevenZipLibPtr->GetProcAddress("CreateObject");
	}

	const GUID *Archive::getFormatClsidByExt(const char *szExt)
	{
		const GUID * pFormatGUID = NULL;
		char szExtBuffer[32] = { 0 };

		int i;
		size_t slen = strlen(szExt);

		if (szExt == NULL)
			return NULL;
		if ((slen <= 0) || (slen >= 32))
			return NULL;

		strncpy_s(szExtBuffer, szExt, slen);
		for (i = 0; i<slen; i++)
		{
			szExtBuffer[i] = (char)tolower(szExtBuffer[i]);
		}
		szExtBuffer[i] = 0;

		if (strncmp(szExtBuffer, "zip", slen) == 0)
			pFormatGUID = &CLSID_CFormatZip;
		else if (strncmp(szExtBuffer, "bz2", slen) == 0)
			pFormatGUID = &CLSID_CFormatBZ2;
		else if (strncmp(szExtBuffer, "rar", slen) == 0)
			pFormatGUID = &CLSID_CFormatRar;
		else if (strncmp(szExtBuffer, "arj", slen) == 0)
			pFormatGUID = &CLSID_CFormatArj;
		else if (strncmp(szExtBuffer, "z", slen) == 0)
			pFormatGUID = &CLSID_CFormatZ;
		else if (strncmp(szExtBuffer, "lzh", slen) == 0)
			pFormatGUID = &CLSID_CFormatLzh;
		else if (strncmp(szExtBuffer, "7z", slen) == 0)
			pFormatGUID = &CLSID_CFormat7z;
		else if (strncmp(szExtBuffer, "cab", slen) == 0)
			pFormatGUID = &CLSID_CFormatCab;
		else if (strncmp(szExtBuffer, "nsis", slen) == 0)
			pFormatGUID = &CLSID_CFormatNsis;
		else if (strncmp(szExtBuffer, "lzma", slen) == 0)
			pFormatGUID = &CLSID_CFormatLzma;
		else if (strncmp(szExtBuffer, "compound", slen) == 0)
			pFormatGUID = &CLSID_CFormatCompound;
		else if (strncmp(szExtBuffer, "wim", slen) == 0)
			pFormatGUID = &CLSID_CFormatWim;
		else if (strncmp(szExtBuffer, "iso", slen) == 0)
			pFormatGUID = &CLSID_CFormatIso;
		else if (strncmp(szExtBuffer, "bkf", slen) == 0)
			pFormatGUID = &CLSID_CFormatBkf;
		else if (strncmp(szExtBuffer, "chm", slen) == 0)
			pFormatGUID = &CLSID_CFormatChm;
		else if (strncmp(szExtBuffer, "split", slen) == 0)
			pFormatGUID = &CLSID_CFormatSplit;
		else if (strncmp(szExtBuffer, "rpm", slen) == 0)
			pFormatGUID = &CLSID_CFormatRpm;
		else if (strncmp(szExtBuffer, "deb", slen) == 0)
			pFormatGUID = &CLSID_CFormatDeb;
		else if (strncmp(szExtBuffer, "cpio", slen) == 0)
			pFormatGUID = &CLSID_CFormatCpio;
		else if (strncmp(szExtBuffer, "tar", slen) == 0)
			pFormatGUID = &CLSID_CFormatTar;
		else if (strncmp(szExtBuffer, "gzip", slen) == 0)
			pFormatGUID = &CLSID_CFormatGZip;

		return pFormatGUID;
	}

	const GUID *Archive::getFormatClsidByExt(const wchar_t *szExt)
	{
		const GUID * pFormatGUID = NULL;
		wchar_t szExtBuffer[32] = { 0 };

		int i;
		size_t slen = wcslen(szExt);

		if (szExt == NULL)
			return NULL;
		if ((slen <= 0) || (slen >= 32))
			return NULL;

		wcsncpy_s(szExtBuffer, szExt, slen);
		for (i = 0; i<slen; i++)
		{
			szExtBuffer[i] = (char)tolower(szExtBuffer[i]);
		}
		szExtBuffer[i] = 0;

		if (wcsncmp(szExtBuffer, L"zip", slen) == 0)
			pFormatGUID = &CLSID_CFormatZip;
		else if (wcsncmp(szExtBuffer, L"bz2", slen) == 0)
			pFormatGUID = &CLSID_CFormatBZ2;
		else if (wcsncmp(szExtBuffer, L"rar", slen) == 0)
			pFormatGUID = &CLSID_CFormatRar;
		else if (wcsncmp(szExtBuffer, L"arj", slen) == 0)
			pFormatGUID = &CLSID_CFormatArj;
		else if (wcsncmp(szExtBuffer, L"z", slen) == 0)
			pFormatGUID = &CLSID_CFormatZ;
		else if (wcsncmp(szExtBuffer, L"lzh", slen) == 0)
			pFormatGUID = &CLSID_CFormatLzh;
		else if (wcsncmp(szExtBuffer, L"7z", slen) == 0)
			pFormatGUID = &CLSID_CFormat7z;
		else if (wcsncmp(szExtBuffer, L"cab", slen) == 0)
			pFormatGUID = &CLSID_CFormatCab;
		else if (wcsncmp(szExtBuffer, L"nsis", slen) == 0)
			pFormatGUID = &CLSID_CFormatNsis;
		else if (wcsncmp(szExtBuffer, L"lzma", slen) == 0)
			pFormatGUID = &CLSID_CFormatLzma;
		else if (wcsncmp(szExtBuffer, L"compound", slen) == 0)
			pFormatGUID = &CLSID_CFormatCompound;
		else if (wcsncmp(szExtBuffer, L"wim", slen) == 0)
			pFormatGUID = &CLSID_CFormatWim;
		else if (wcsncmp(szExtBuffer, L"iso", slen) == 0)
			pFormatGUID = &CLSID_CFormatIso;
		else if (wcsncmp(szExtBuffer, L"bkf", slen) == 0)
			pFormatGUID = &CLSID_CFormatBkf;
		else if (wcsncmp(szExtBuffer, L"chm", slen) == 0)
			pFormatGUID = &CLSID_CFormatChm;
		else if (wcsncmp(szExtBuffer, L"split", slen) == 0)
			pFormatGUID = &CLSID_CFormatSplit;
		else if (wcsncmp(szExtBuffer, L"rpm", slen) == 0)
			pFormatGUID = &CLSID_CFormatRpm;
		else if (wcsncmp(szExtBuffer, L"deb", slen) == 0)
			pFormatGUID = &CLSID_CFormatDeb;
		else if (wcsncmp(szExtBuffer, L"cpio", slen) == 0)
			pFormatGUID = &CLSID_CFormatCpio;
		else if (wcsncmp(szExtBuffer, L"tar", slen) == 0)
			pFormatGUID = &CLSID_CFormatTar;
		else if (wcsncmp(szExtBuffer, L"gzip", slen) == 0)
			pFormatGUID = &CLSID_CFormatGZip;

		return pFormatGUID;
	}
}
