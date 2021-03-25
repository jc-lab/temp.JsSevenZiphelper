/**
 * @file	Extract.cpp
 * @class	Extract
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/06
 * @brief	JsSevenZipHelper Extract class
 */

#include <iostream>
#include <atlbase.h>
#include "PropVariant.h"
#include "Extract.h"

#include <JsCPPUtils/StringEncoding.h>
#include "FileMaker.h"

namespace JsSevenZipHelper
{

	// ========== MyArchiveExtractCallback =========
	template <typename CHARTYPE>
	class MyArchiveExtractCallback :
		public IArchiveExtractCallback,
		public ICryptoGetTextPassword,
		public ICryptoGetTextPassword2,
		public CMyUnknownImp
	{
	private:
		CMyComPtr<IInArchive> m_inArc;

		bool m_isDefinedPassword;
		std::basic_string<wchar_t> m_strPassword;

		std::basic_string<CHARTYPE> m_strRootDir;

		IArchiveExtractCallback *m_extractCallback;

		std::list<Extract::FileListItem<CHARTYPE> > *m_pFileList;

	public:
		MyArchiveExtractCallback(IInArchive *pInArc, bool isDefinedPassword, const wchar_t *szPassword, const std::basic_string<CHARTYPE>& strExtractRootDir, IArchiveExtractCallback *extractCallback, std::list<Extract::FileListItem<CHARTYPE> > *pFileList)
		{
			m_inArc = pInArc;
			m_isDefinedPassword = isDefinedPassword;
			if (szPassword != NULL)
				m_strPassword = szPassword;
			m_strRootDir = strExtractRootDir;
			m_extractCallback = extractCallback;
			m_pFileList = pFileList;
		}

		~MyArchiveExtractCallback()
		{

		}

		HRESULT CheckBreak() { return S_OK; }

		MY_QUERYINTERFACE_BEGIN2(IArchiveExtractCallback)
			MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword)
			MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword2)
			MY_QUERYINTERFACE_END
			MY_ADDREF_RELEASE

			//INTERFACE_IArchiveExtractCallback(;)
			/*
			INTERFACE_IProgress(x) \
			STDMETHOD(SetTotal)(UInt64 total) x; \
			STDMETHOD(SetCompleted)(const UInt64 *completeValue) x; \
			STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode) x; \
			STDMETHOD(PrepareOperation)(Int32 askExtractMode) x; \
			STDMETHOD(SetOperationResult)(Int32 opRes) x; \
			*/

			// ICryptoGetTextPassword
			//STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

			// ICryptoGetTextPassword2
			//STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password);

			STDMETHOD(SetTotal)(UInt64 total)
		{
			if (m_extractCallback != NULL)
				return m_extractCallback->SetTotal(total);
			return S_OK;
		}

		STDMETHOD(SetCompleted)(const UInt64 *completeValue)
		{
			if (m_extractCallback != NULL)
				return m_extractCallback->SetCompleted(completeValue);
			return S_OK;
		}

		STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode)
		{
			HRESULT hres;

			int i, n;

			static const PROPID propIDs[] = { kpidAttrib, kpidIsDir, kpidIsAnti, kpidMTime, kpidATime, kpidCTime, kpidSize };

			Extract::FileListItem<CHARTYPE> fileItem;

			*outStream = NULL;

			if (m_extractCallback != NULL)
				return m_extractCallback->GetStream(index, outStream, askExtractMode);

			{
				// Get Filepath
				CPropVariant prop;

				hres = m_inArc->GetProperty(index, kpidPath, &prop);
				if (hres != S_OK)
					return hres;

				if (prop.vt == VT_EMPTY)
					fileItem.strArchivePath.clear();
				else
				{
					fileItem.strArchivePath = JsCPPUtils::StringEncoding::AutoMWStringToString<wchar_t, CHARTYPE>::encode(prop.bstrVal, wcslen(prop.bstrVal));
				}
			}

			if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
				return S_OK;

			for (i = 0, n = _countof(propIDs); i<n; i++)
			{
				CPropVariant prop;
				PROPID propid = propIDs[i];
				hres = m_inArc->GetProperty(index, propid, &prop);
				if (hres != S_OK)
					return hres;
				switch (propid)
				{
				case kpidAttrib:
					if (prop.vt == VT_EMPTY) {
						fileItem.fileAttribDefined = false;
						fileItem.fileAttrib = 0;
					}
					else if (prop.vt == VT_UI4) {
						fileItem.fileAttribDefined = false;
						fileItem.fileAttrib = prop.ulVal;
					}
					else {
						return E_FAIL;
					}
					break;
				case kpidIsDir:
					if (prop.vt == VT_EMPTY) {
						fileItem.isDir = false;
					}
					else if (prop.vt == VT_BOOL) {
						fileItem.isDir = prop.boolVal == 0 ? false : true;
					}
					else {
						return E_FAIL;
					}
					break;
				case kpidIsAnti:
					if (prop.vt == VT_EMPTY) {
						fileItem.isAnti = false;
					}
					else if (prop.vt == VT_BOOL) {
						fileItem.isAnti = prop.boolVal == 0 ? false : true;
					}
					else {
						return E_FAIL;
					}
					break;
				case kpidMTime:
				case kpidATime:
				case kpidCTime:
					if (prop.vt == VT_EMPTY) {
						// nothing
					}
					else if (prop.vt == VT_FILETIME) {
						switch (propid)
						{
						case kpidMTime:
							memcpy(&fileItem.fileMtime, &prop.filetime, sizeof(FILETIME));
							break;
						case kpidATime:
							memcpy(&fileItem.fileAtime, &prop.filetime, sizeof(FILETIME));
							break;
						case kpidCTime:
							memcpy(&fileItem.fileCtime, &prop.filetime, sizeof(FILETIME));
							break;
						}
					}
					else {
						return E_FAIL;
					}
					break;
				case kpidSize:
					if (prop.vt == VT_EMPTY) {
						fileItem.newFileSize = -1;
					}
					else {
						fileItem.newFileSize = ConvertPropVariantToUInt64(prop);
					}
					break;
				}
			}

			std::basic_string<CHARTYPE> strFullFilePath;
			if (m_strRootDir.length() > 0)
			{
				if (m_strRootDir.at(m_strRootDir.length() - 1) == '\\')
				{
					strFullFilePath = m_strRootDir + fileItem.strArchivePath;
				}
				else {
					std::basic_string<char> tempspliter = "\\";
					strFullFilePath = m_strRootDir + std::basic_string<CHARTYPE>(tempspliter.begin(), tempspliter.end()) + fileItem.strArchivePath;
				}
			}
			else {
				strFullFilePath = fileItem.strArchivePath;
			}

			std::basic_string<CHARTYPE> parentDirPath;
			parentDirPath = Extract::getParentDir(strFullFilePath);

			if (!Extract::makeDirs(parentDirPath))
				return S_OK; // ignore

			if (fileItem.isDir)
			{
				JsCPPUtils::FileMaker file;
				if (!file.create(strFullFilePath.c_str(), false, true))
				{
					return S_OK; // ignore
					//return E_FAIL;
				}
				if (fileItem.fileCtimeDefined)
					file.setCreationWinFileTime(&fileItem.fileCtime);
				if (fileItem.fileAtimeDefined)
					file.setLastAccessWinFileTime(&fileItem.fileAtime);
				if (fileItem.fileMtimeDefined)
					file.setLastWriteWinFileTime(&fileItem.fileMtime);
				file.close();
				if (m_pFileList != NULL)
					m_pFileList->push_back(fileItem);
			}
			else {
				FileOutStream *outStreamSpec = new FileOutStream();
				CMyComPtr<IOutStream> outStreamPtr(outStreamSpec);
				if (!outStreamSpec->Create(strFullFilePath.c_str(), true))
				{
					*outStream = NULL;
					return S_OK; // ignore
					//return E_FAIL;
				}
				*outStream = outStreamPtr.Detach();
				if (m_pFileList != NULL)
					m_pFileList->push_back(fileItem);
			}

			return S_OK;
		}

		STDMETHOD(PrepareOperation)(Int32 askExtractMode)
		{
			if (m_extractCallback != NULL)
				return m_extractCallback->PrepareOperation(askExtractMode);
			return S_OK;
		}

		STDMETHOD(SetOperationResult)(Int32 opRes)
		{
			if (m_extractCallback != NULL)
				return m_extractCallback->PrepareOperation(opRes);
			return S_OK;
		}


		STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword)
		{
			CComBSTR tmpbstr(m_strPassword.c_str());
			if (m_isDefinedPassword)
				return tmpbstr.CopyTo(aPassword);
			else
				*aPassword = NULL;
			return S_OK;
		}

		STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password)
		{
			CComBSTR tmpbstr(m_strPassword.c_str());
			*passwordIsDefined = m_isDefinedPassword ? 1 : 0;
			if (m_isDefinedPassword)
				return tmpbstr.CopyTo(password);
			else
				*password = NULL;
			return S_OK;
		}

		uint64_t ConvertPropVariantToUInt64(const PROPVARIANT &propVariant)
		{
			switch (propVariant.vt)
			{
			case VT_UI1:
				return propVariant.bVal;
			case VT_UI2:
				return propVariant.uiVal;
			case VT_UI4:
				return propVariant.ulVal;
			case VT_UI8:
				return (uint64_t)propVariant.uhVal.QuadPart;
			default:
#ifndef _WIN32_WCE
				throw 151199;
#else
				return 0;
#endif
			}
		}
	};

	Extract::Extract() : 
		Archive(),
		m_pInArcObj()
	{
		
	}

	Extract::Extract(JsCPPUtils::WinLibrary *pSevenZipLib) : 
		Archive(pSevenZipLib),
		m_pInArcObj()
	{
		
	}

	Extract::Extract(const Archive& archive) : 
		Archive(archive),
		m_pInArcObj()
	{
		
	}

	Extract::~Extract()
	{
		closeArchive();
	}

	HRESULT Extract::openArchive(const GUID *pFormatGUID, IInStream *pInStream, bool isDefinedPassword, const char *szPassword, UInt64 *pMaxCheckStartPosition)
	{
		HRESULT hres;

		CMyComPtr<IInArchive> pArc;

		MyArchiveOpenCallback *openCallbackSpec;
		CMyComPtr<IArchiveOpenCallback> openCallback;

		std::basic_string<wchar_t> strEncodedPassword;

		if(m_fpCreateObject == NULL)
			return E_FAIL;
		
		if(szPassword != NULL)
			strEncodedPassword = JsCPPUtils::StringEncoding::StringToUnicode(szPassword);

		do {
			hres = m_fpCreateObject(pFormatGUID, &IID_IInArchive, (void **)&pArc);
			if(FAILED(hres))
				break;

			if(szPassword == NULL)
				openCallbackSpec = new MyArchiveOpenCallback(isDefinedPassword, NULL);
			else
				openCallbackSpec = new MyArchiveOpenCallback(isDefinedPassword, strEncodedPassword);
			openCallback = CMyComPtr<IArchiveOpenCallback>(openCallbackSpec);

			hres = pArc->Open(pInStream, pMaxCheckStartPosition, openCallback);
			if(FAILED(hres))
				break;
			
			m_isDefinedPassword = isDefinedPassword;
			if(szPassword != NULL)
				m_strPassword = strEncodedPassword;
		}while(0);

		if(SUCCEEDED(hres))
			m_pInArcObj = pArc;
		else if(pArc != NULL)
		{
			pArc->Release();
		}
		
		return hres;
	}

	HRESULT Extract::openArchive(const GUID *pFormatGUID, IInStream *pInStream, bool isDefinedPassword, const wchar_t *szPassword, UInt64 *pMaxCheckStartPosition)
	{
		HRESULT hres;

		IInArchive* pArc = NULL;

		MyArchiveOpenCallback *openCallbackSpec;
		CMyComPtr<IArchiveOpenCallback> openCallback;

		if(m_fpCreateObject == NULL)
			return E_FAIL;

		do {
			hres = m_fpCreateObject(pFormatGUID, &IID_IInArchive, (void **)&pArc);
			if(FAILED(hres))
				break;

			openCallbackSpec = new MyArchiveOpenCallback(isDefinedPassword, szPassword);
			openCallback = CMyComPtr<IArchiveOpenCallback>(openCallbackSpec);

			hres = pArc->Open(pInStream, pMaxCheckStartPosition, openCallback);
			if(FAILED(hres))
				break;
			
			m_isDefinedPassword = isDefinedPassword;
			if(szPassword != NULL)
				m_strPassword = szPassword;
		}while(0);

		if(SUCCEEDED(hres))
			m_pInArcObj = pArc;
		else if(FAILED(hres))
		{
			pArc->Release();
			pArc = NULL;
		}
		
		return hres;
	}

	void Extract::closeArchive()
	{
		if(m_pInArcObj != NULL)
		{
			m_pInArcObj->Release();
			m_pInArcObj = NULL;
		}
	}
	
	CComPtr<IInArchive> Extract::getInArcPtr()
	{
		return m_pInArcObj;
	}

	HRESULT Extract::extract(const std::basic_string<char>& strExtractRootPath, IArchiveExtractCallback *extractCallback, std::list< FileListItem<char> > *pFileList)
	{
		HRESULT hres;

		if (m_pInArcObj == NULL)
			return E_FAIL;

		MyArchiveExtractCallback<char> *extractCallbackSpec = new MyArchiveExtractCallback<char>(m_pInArcObj, m_isDefinedPassword, m_strPassword.c_str(), strExtractRootPath, extractCallback, pFileList);
		CMyComPtr<IArchiveExtractCallback> extractCallbackPtr(extractCallbackSpec);

		hres = m_pInArcObj->Extract(NULL, (UInt32)(Int32)(-1), false, extractCallbackPtr);

		return hres;
	}

	HRESULT Extract::extract(const std::basic_string<wchar_t>& strExtractRootPath, IArchiveExtractCallback *extractCallback, std::list< FileListItem<wchar_t> > *pFileList)
	{
		HRESULT hres;

		if (m_pInArcObj == NULL)
			return E_FAIL;

		MyArchiveExtractCallback<wchar_t> *extractCallbackSpec = new MyArchiveExtractCallback<wchar_t>(m_pInArcObj, m_isDefinedPassword, m_strPassword.c_str(), strExtractRootPath, extractCallback, pFileList);
		CMyComPtr<IArchiveExtractCallback> extractCallbackPtr(extractCallbackSpec);

		hres = m_pInArcObj->Extract(NULL, (UInt32)(Int32)(-1), false, extractCallbackPtr);

		return hres;
	}

	bool Extract::makeDirs(const std::basic_string<char>& strDirPath)
	{
		char szDirPathBuf[MAX_PATH];

		strcpy_s(szDirPathBuf, strDirPath.c_str());

		return makeSubDirs(szDirPathBuf);
	}

	bool Extract::makeDirs(const std::basic_string<wchar_t>& strDirPath)
	{
		wchar_t szDirPathBuf[MAX_PATH];

		wcscpy_s(szDirPathBuf, strDirPath.c_str());

		return makeSubDirs(szDirPathBuf);
	}

	bool Extract::makeSubDirs(char *szDirPathBuf)
	{
		bool retval;
		int len = strlen(szDirPathBuf);
		char *tp = szDirPathBuf + len;
		DWORD dwAttrib;

		while (--tp > szDirPathBuf)
		{
			if (*tp == '\\')
				break;
		}

		if (tp > szDirPathBuf)
		{
			*tp = '\0';
			makeSubDirs(szDirPathBuf);
			*tp = '\\';
		}

		// For \\?\Volume{...}
		if (strcmp(szDirPathBuf, "\\") == 0)
			return true;
		else if (strcmp(szDirPathBuf, "\\\\?") == 0)
			return true;
		else {
			len = strlen(szDirPathBuf);
			if (strstr(szDirPathBuf, "\\\\?\\") == szDirPathBuf)
			{
				const char *tmpptr;
				if (strstr(szDirPathBuf + 4, "\\") == NULL)
				{
					tmpptr = szDirPathBuf + len - 1;
					if (*tmpptr == '}')
						return true;
				}
			}
		}

		dwAttrib = GetFileAttributesA(szDirPathBuf);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES)
		{
			retval = ::CreateDirectoryA(szDirPathBuf, NULL) ? true : false;
		}else{
			retval = true;
		}

		return retval;
	}

	bool Extract::makeSubDirs(wchar_t *szDirPathBuf)
	{
		bool retval;
		int len = wcslen(szDirPathBuf);
		wchar_t *tp = szDirPathBuf + len;
		DWORD dwAttrib;

		while (--tp > szDirPathBuf)
		{
			if (*tp == L'\\')
				break;
		}

		if (tp > szDirPathBuf)
		{
			*tp = L'\0';
			makeSubDirs(szDirPathBuf);
			*tp = L'\\';
		}

		// For \\?\Volume{...}
		if (wcscmp(szDirPathBuf, L"\\") == 0)
			return true;
		else if (wcscmp(szDirPathBuf, L"\\\\?") == 0)
			return true;
		else {
			len = wcslen(szDirPathBuf);
			if (wcsstr(szDirPathBuf, L"\\\\?\\") == szDirPathBuf)
			{
				const wchar_t *tmpptr;
				if (wcsstr(szDirPathBuf + 4, L"\\") == NULL)
				{
					tmpptr = szDirPathBuf + len - 1;
					if (*tmpptr == '}')
						return true;
				}
			}
		}

		dwAttrib = GetFileAttributesW(szDirPathBuf);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES)
		{
			retval = ::CreateDirectoryW(szDirPathBuf, NULL) ? true : false;
		}else{
			retval = true;
		}

		return retval;
	}

	std::basic_string<char> Extract::getParentDir(const std::basic_string<char>& strFilePath)
	{
		int len = strFilePath.length();
		const char *cstr = strFilePath.c_str();

		while ((len > 0) && (cstr[--len] != '\\'));
		if (len > 0)
			return std::basic_string<char>(cstr, len);
		else
			return "";
	}

	std::basic_string<wchar_t> Extract::getParentDir(const std::basic_string<wchar_t>& strFilePath)
	{
		int len = strFilePath.length();
		const wchar_t *cstr = strFilePath.c_str();

		while ((len > 0) && (cstr[--len] != _T('\\')));
		if (len > 0)
			return std::basic_string<wchar_t>(cstr, len);
		else
			return _T("");
	}

	// ========== MyArchiveOpenCallback =========
	Extract::MyArchiveOpenCallback::MyArchiveOpenCallback(bool isDefinedPassword, const wchar_t *szPassword)
	{
		m_isDefinedPassword = isDefinedPassword;
		if(szPassword != NULL)
			m_strPassword = szPassword;
	}
	
	Extract::MyArchiveOpenCallback::MyArchiveOpenCallback(bool isDefinedPassword, const std::basic_string<wchar_t> &strPassword)
	{
		m_isDefinedPassword = isDefinedPassword;
		m_strPassword = strPassword;
	}

	Extract::MyArchiveOpenCallback::~MyArchiveOpenCallback()
	{
	
	}

	HRESULT Extract::MyArchiveOpenCallback::CheckBreak()
	{
		return S_OK;
	}

	STDMETHODIMP Extract::MyArchiveOpenCallback::SetTotal(const UInt64 *files, const UInt64 *bytes)
	{
		return S_OK;
	}

	STDMETHODIMP Extract::MyArchiveOpenCallback::SetCompleted(const UInt64 *files, const UInt64 *bytes)
	{
		return S_OK;
	}

	STDMETHODIMP Extract::MyArchiveOpenCallback::CryptoGetTextPassword(BSTR *aPassword)
	{
		CComBSTR tmpbstr(m_strPassword.c_str());
		if(m_isDefinedPassword)
			return tmpbstr.CopyTo(aPassword);
		else
			*aPassword = NULL;
		return S_OK;
	}

	STDMETHODIMP Extract::MyArchiveOpenCallback::CryptoGetTextPassword2(Int32 *passwordIsDefined, BSTR *password)
	{
		CComBSTR tmpbstr(m_strPassword.c_str());
		*passwordIsDefined = m_isDefinedPassword ? 1 : 0;
		if(m_isDefinedPassword)
			return tmpbstr.CopyTo(password);
		else
			*password = NULL;
		return S_OK;
	}

}