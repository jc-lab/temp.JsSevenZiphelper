/**
 * @file	Compress.cpp
 * @class	Compress
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/06
 * @brief	JsSevenZipHelper Compress class
 */

/*
 * ISSUE
 * FileListItem 에 isDir체크해도 압축파일에 Directory가 아닌 0bytes 파일로 생성됨...ㅠㅠ
 * 
 * NOT SUPPORTED
 * isAnti 지원안함. 꼭 isAnti = false으로..ㅠㅠ
 * kpidIsAltStream 뭔지 아직 모르겠다..
 * hardLink, symLink도 지원안함..
*/

#include <iostream>

#include <atlbase.h>

#include "Compress.h"

#include <JsCPPUtils/StringEncoding.h>

namespace JsSevenZipHelper
{

	// ========== MyArchiveUpdateCallback =========
	
	template <typename CHARTYPE>
	class MyArchiveUpdateCallback :
		public IArchiveUpdateCallback,
		public IArchiveUpdateCallbackFile,
		public ICryptoGetTextPassword,
		public ICryptoGetTextPassword2,
		public CMyUnknownImp
	{
	private:
		bool m_isDefinedPassword;
		std::basic_string<wchar_t> m_strPassword;
		std::list<Compress::ArchiveFileInfo<CHARTYPE> > m_fileList;

	public:
		MY_QUERYINTERFACE_BEGIN2(IArchiveUpdateCallback)
			/*
			MY_QUERYINTERFACE_ENTRY(IArchiveExtractCallbackMessage)
			MY_QUERYINTERFACE_ENTRY(IArchiveGetRawProps)
			MY_QUERYINTERFACE_ENTRY(IArchiveGetRootProps)
			MY_QUERYINTERFACE_ENTRY(ICompressProgressInfo)
			*/
			MY_QUERYINTERFACE_ENTRY(IArchiveUpdateCallbackFile)
			MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword2)
			MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword)
			MY_QUERYINTERFACE_END
			MY_ADDREF_RELEASE

			// IArchiveUpdateCallback
			//INTERFACE_IArchiveUpdateCallback(;)
			/*
			INTERFACE_IProgress(x); \
			-> STDMETHOD(SetTotal)(UInt64 total);
			-> STDMETHOD(SetCompleted)(const UInt64 *completeValue);
			STDMETHOD(GetUpdateItemInfo)(UInt32 index, Int32 *newData, Int32 *newProps, UInt32 *indexInArchive) x; \
			STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value) x; \
			STDMETHOD(GetStream)(UInt32 index, ISequentialInStream **inStream) x; \
			STDMETHOD(SetOperationResult)(Int32 operationResult) x; \
			*/
			//HRESULT GetTimeProperty(int64_t time, PROPVARIANT *value);

		//INTERFACE_IArchiveUpdateCallbackFile(;)
			/*
			STDMETHOD(GetStream2)(UInt32 index, ISequentialInStream **inStream, UInt32 notifyOp) x; \
			STDMETHOD(ReportOperation)(UInt32 indexType, UInt32 index, UInt32 notifyOp) x; \
			*/

			// ICryptoGetTextPassword
			//STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

		// ICryptoGetTextPassword2
		//STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password);
		
		MyArchiveUpdateCallback(const std::list<Compress::FileListItem<CHARTYPE> >& fileList, bool isDefinedPassword, const wchar_t *cszPassword)
		{
			m_isDefinedPassword = isDefinedPassword;
			if (cszPassword)
				m_strPassword = cszPassword;
			for (std::list<Compress::FileListItem<CHARTYPE> >::const_iterator iter = fileList.begin(); iter != fileList.end(); iter++)
			{
				m_fileList.push_back(Compress::ArchiveFileInfo<CHARTYPE>(iter->isDir, iter->isAnti, iter->strFilePath, iter->strArchivePath));
			}
		}
		
		MyArchiveUpdateCallback(const std::list<Compress::FileListItem<CHARTYPE> >& fileList, bool isDefinedPassword, const std::basic_string<wchar_t> &strPassword)
		{
			m_isDefinedPassword = isDefinedPassword;
			m_strPassword = strPassword;
			for (std::list<Compress::FileListItem<CHARTYPE> >::const_iterator iter = fileList.begin(); iter != fileList.end(); iter++)
			{
				m_fileList.push_back(Compress::ArchiveFileInfo<CHARTYPE>(iter->isDir, iter->isAnti, iter->strFilePath, iter->strArchivePath));
			}
		}

		~MyArchiveUpdateCallback()
		{

		}

		HRESULT CheckBreak()
		{
			return S_OK;
		}

		STDMETHODIMP SetTotal(UInt64 total)
		{
			return S_OK;
		}

		STDMETHODIMP SetCompleted(const UInt64 *completeValue)
		{
			return S_OK;
		}

		STDMETHODIMP GetUpdateItemInfo(UInt32 index, Int32 *newData, Int32 *newProps, UInt32 *indexInArchive)
		{
			*newData = 1;
			*newProps = 1;
			*indexInArchive = 0xFFFFFFFF;
			return S_OK;
		}

		HRESULT GetTimeProperty(int64_t time, PROPVARIANT *value)
		{
			value->vt = VT_FILETIME;
			value->hVal.QuadPart = time;
			return S_OK;
		}

		STDMETHODIMP GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
		{
			std::list<Compress::ArchiveFileInfo<CHARTYPE> >::iterator fileInfoIter = m_fileList.begin();
			std::advance(fileInfoIter, index);
			switch (propID)
			{
			case kpidPath: // 3
			{
				CComBSTR tmpbstr(fileInfoIter->strArchivePath.c_str());
				value->vt = VT_BSTR;
				value->bstrVal = tmpbstr.Copy();
			}
			break;
			case kpidIsDir: // 6
				value->vt = VT_BOOL;
				value->boolVal = fileInfoIter->isDir ? VARIANT_TRUE : VARIANT_FALSE;
				break;
			case kpidSize: // 7
				value->vt = VT_UI8;
				if (fileInfoIter->isDir || fileInfoIter->isAnti)
					value->uhVal.QuadPart = 0;
				else
					value->uhVal.QuadPart = fileInfoIter->fileInfo.getFileSize();
				break;
			case kpidAttrib: // 9
				value->vt = VT_EMPTY;
				break;
			case kpidIsAnti:
				value->vt = VT_BOOL;
				value->boolVal = fileInfoIter->isAnti ? VARIANT_TRUE : VARIANT_FALSE;
				break;
			case kpidCTime: // 10
				GetTimeProperty(fileInfoIter->fileInfo.getCreationWinFileTime(), value);
				break;
			case kpidATime: // 11
				GetTimeProperty(fileInfoIter->fileInfo.getLastAccessWinFileTime(), value);
				break;
			case kpidMTime: // 12
				GetTimeProperty(fileInfoIter->fileInfo.getLastWriteWinFileTime(), value);
				break;
			default:
				value->vt = VT_EMPTY;
				break;
			}
			return S_OK;
		}

		STDMETHODIMP GetStream(UInt32 index, ISequentialInStream **inStream)
		{
			//NUpdateNotifyOp::kAdd
			//NUpdateNotifyOp::kUpdate
			//etc...
			return GetStream2(index, inStream, NUpdateNotifyOp::kAdd);
		}

		STDMETHODIMP SetOperationResult(Int32 operationResult)
		{
			return S_OK;
		}

		STDMETHODIMP GetStream2(UInt32 index, ISequentialInStream **inStream, UInt32 notifyOp)
		{
			std::list<Compress::ArchiveFileInfo<CHARTYPE> >::iterator fileInfoIter = m_fileList.begin();
			std::advance(fileInfoIter, index);

			*inStream = NULL;

			if (fileInfoIter->isDir)
				return S_OK;

			if (notifyOp != NUpdateNotifyOp::kAdd &&
				notifyOp != NUpdateNotifyOp::kUpdate)
				return S_OK;

			FileInStream *inStreamSpec = new FileInStream();
			CMyComPtr<ISequentialInStream> inStreamLoc(inStreamSpec);
			if (!inStreamSpec->Open(fileInfoIter->fileInfo.getFullFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, OPEN_EXISTING))
			{
				*inStream = NULL;
				return S_OK;
				//return E_FAIL;
			}
			*inStream = inStreamLoc.Detach();
			fileInfoIter->isNewFile = false;
			return S_OK;
		}

		STDMETHODIMP ReportOperation(UInt32 indexType, UInt32 index, UInt32 notifyOp)
		{
			return S_OK;
		}


		STDMETHODIMP CryptoGetTextPassword(BSTR *aPassword)
		{
			CComBSTR tmpbstr(m_strPassword.c_str());
			if (m_isDefinedPassword)
				return tmpbstr.CopyTo(aPassword);
			else
				*aPassword = NULL;
			return S_OK;
		}

		STDMETHODIMP CryptoGetTextPassword2(Int32 *passwordIsDefined, BSTR *password)
		{
			CComBSTR tmpbstr(m_strPassword.c_str());
			*passwordIsDefined = m_isDefinedPassword ? 1 : 0;
			if (m_isDefinedPassword)
				return tmpbstr.CopyTo(password);
			else
				*password = NULL;
			return S_OK;
		}

	};

	// ========== Compress ==========

	Compress::Compress() :
		Archive()
	{

	}

	Compress::Compress(JsCPPUtils::WinLibrary *pSevenZipLib) :
		Archive(pSevenZipLib)
	{

	}

	Compress::Compress(JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> spSevenZipLib) :
		Archive(spSevenZipLib)
	{

	}

	Compress::Compress(const Archive& archive) :
		Archive(archive)
	{

	}

	Compress::~Compress()
	{
		closeArchive();
	}

	HRESULT Compress::createArchive(const GUID *pFormatGUID)
	{
		HRESULT hres;

		CMyComPtr<IOutArchive> *pArc = NULL;

		if (m_fpCreateObject == NULL)
			return E_FAIL;

		pArc = new CMyComPtr<IOutArchive>();
		hres = m_fpCreateObject(pFormatGUID, &IID_IOutArchive, (void **)pArc);

		if (hres == S_OK)
			m_pOutArcObj = pArc;

		if (hres != S_OK && pArc != NULL)
		{
			delete pArc;
			pArc = NULL;
		}

		return hres;
	}

	void Compress::closeArchive()
	{
		if (m_pOutArcObj != NULL)
		{
			delete m_pOutArcObj;
			m_pOutArcObj = NULL;
		}
	}
	
	HRESULT Compress::compress(ISequentialOutStream *pOutStreamSpec, const std::list<FileListItem<char> >& fileList, bool isDefinedPassword, bool isHeaderEncrypt, const char *cszPassword)
	{
		HRESULT hres;

		MyArchiveUpdateCallback<char> *updateCallbackSpec;
		CMyComPtr<IArchiveUpdateCallback> updateCallback;
		CMyComPtr<ISetProperties> setProperties;

		wchar_t *names[] = { L"he" };
		PROPVARIANT props[1];

		if (m_pOutArcObj == NULL)
			return false;

		(*m_pOutArcObj)->QueryInterface(IID_ISetProperties, (void **)&setProperties);

		if(cszPassword == NULL)
			updateCallbackSpec = new MyArchiveUpdateCallback<char>(fileList, isDefinedPassword, NULL);
		else
			updateCallbackSpec = new MyArchiveUpdateCallback<char>(fileList, isDefinedPassword, JsCPPUtils::StringEncoding::StringToUnicode(cszPassword));
		updateCallback = CMyComPtr<IArchiveUpdateCallback>(updateCallbackSpec);

		if (isHeaderEncrypt)
		{
			CComBSTR tmpbstr("on");
			props[0].vt = VT_BSTR;
			props[0].bstrVal = tmpbstr.Copy();
			(*setProperties).SetProperties(names, props, 1);
		}

		hres = (*m_pOutArcObj)->UpdateItems(pOutStreamSpec, fileList.size(), updateCallback);

		return hres;
	}
	
	HRESULT Compress::compress(ISequentialOutStream *pOutStreamSpec, const std::list<FileListItem<wchar_t> >& fileList, bool isDefinedPassword, bool isHeaderEncrypt, const wchar_t *cszPassword)
	{
		HRESULT hres;

		MyArchiveUpdateCallback<wchar_t> *updateCallbackSpec;
		CMyComPtr<IArchiveUpdateCallback> updateCallback;
		CMyComPtr<ISetProperties> setProperties;

		wchar_t *names[] = { L"he" };
		PROPVARIANT props[1];

		if (m_pOutArcObj == NULL)
			return false;

		(*m_pOutArcObj)->QueryInterface(IID_ISetProperties, (void **)&setProperties);

		updateCallbackSpec = new MyArchiveUpdateCallback<wchar_t>(fileList, isDefinedPassword, cszPassword);
		updateCallback = CMyComPtr<IArchiveUpdateCallback>(updateCallbackSpec);

		if (isHeaderEncrypt)
		{
			CComBSTR tmpbstr("on");
			props[0].vt = VT_BSTR;
			props[0].bstrVal = tmpbstr.Copy();
			(*setProperties).SetProperties(names, props, 1);
		}

		hres = (*m_pOutArcObj)->UpdateItems(pOutStreamSpec, fileList.size(), updateCallback);

		return hres;
	}

	// ========== UTILS ==========

	int Compress::findFiles(std::list<JsSevenZipHelper::Compress::FileListItem<char> > &lstFiles, const char *cszRootPath, const char *cszArchivePath, FindFileCheckFuncA_t fpFileCheck, void *cbparam)
	{
		int totalFiles = 0;
		char szRootPathBuf[MAX_PATH];
		char szArchivePathBuf[MAX_PATH];
		size_t len;

		len = strlen(cszRootPath);
		if (len <= 0)
			return 0;

		strcpy_s(szRootPathBuf, cszRootPath);
		strcpy_s(szArchivePathBuf, cszArchivePath);
		if (szRootPathBuf[len - 1] != '\\')
		{
			strcat_s(szRootPathBuf, "\\");
		}

		totalFiles += findFiles_subproc(lstFiles, szRootPathBuf, szArchivePathBuf, fpFileCheck, cbparam);

		return totalFiles;
	}

	int Compress::findFiles(std::list<JsSevenZipHelper::Compress::FileListItem<wchar_t> > &lstFiles, const wchar_t *cszRootPath, const wchar_t *cszArchivePath, FindFileCheckFuncW_t fpFileCheck, void *cbparam)
	{
		int totalFiles = 0;
		wchar_t szRootPathBuf[MAX_PATH];
		wchar_t szArchivePathBuf[MAX_PATH];
		size_t len;

		len = wcslen(cszRootPath);
		if (len <= 0)
			return 0;

		wcscpy_s(szRootPathBuf, cszRootPath);
		wcscpy_s(szArchivePathBuf, cszArchivePath);
		if (szRootPathBuf[len - 1] != _T('\\'))
		{
			wcscat_s(szRootPathBuf, _T("\\"));
		}

		totalFiles += findFiles_subproc(lstFiles, szRootPathBuf, szArchivePathBuf, fpFileCheck, cbparam);

		return totalFiles;
	}

	int Compress::findFiles_subproc(std::list<JsSevenZipHelper::Compress::FileListItem<char> > &lstFiles, char *szRootPathBuf, char *szArchivePathBuf, FindFileCheckFuncA_t fpFileCheck, void *cbparam)
	{
		size_t len1, len2;
		HANDLE hFind;
		WIN32_FIND_DATAA finddata;
		int totalFiles = 0;

		len1 = strlen(szRootPathBuf);
		len2 = strlen(szArchivePathBuf);

		strcat_s(szRootPathBuf, MAX_PATH, "*");

		memset(&finddata, 0, sizeof(finddata));
		hFind = FindFirstFileA(szRootPathBuf, &finddata);
		if (hFind == INVALID_HANDLE_VALUE)
			return 0;
		do {
			if (!((strcmp(finddata.cFileName, ".") == 0) || (strcmp(finddata.cFileName, "..") == 0)))
			{
				bool badd = false;
				szRootPathBuf[len1] = 0;
				szArchivePathBuf[len2] = 0;
				strcat_s(szRootPathBuf, MAX_PATH, finddata.cFileName);
				strcat_s(szArchivePathBuf, MAX_PATH, finddata.cFileName);

				if (fpFileCheck == NULL)
					badd = true;
				else
					badd = fpFileCheck(szRootPathBuf, szArchivePathBuf, &finddata, cbparam);

				if (badd)
				{
					if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						strcat_s(szRootPathBuf, MAX_PATH, "\\");
						strcat_s(szArchivePathBuf, MAX_PATH, "\\");
						totalFiles += findFiles_subproc(lstFiles, szRootPathBuf, szArchivePathBuf, fpFileCheck, cbparam);
					}
					else {
						lstFiles.push_back(JsSevenZipHelper::Compress::FileListItem<char>(false, false, szRootPathBuf, szArchivePathBuf));
						totalFiles++;
					}
				}
			}
			memset(&finddata, 0, sizeof(finddata));
		} while (FindNextFileA(hFind, &finddata));
		FindClose(hFind);

		return totalFiles;
	}

	int Compress::findFiles_subproc(std::list<JsSevenZipHelper::Compress::FileListItem<wchar_t> > &lstFiles, wchar_t *szRootPathBuf, wchar_t *szArchivePathBuf, FindFileCheckFuncW_t fpFileCheck, void *cbparam)
	{
		size_t len1, len2;
		HANDLE hFind;
		WIN32_FIND_DATAW finddata;
		int totalFiles = 0;

		len1 = wcslen(szRootPathBuf);
		len2 = wcslen(szArchivePathBuf);

		wcscat_s(szRootPathBuf, MAX_PATH, L"*");

		memset(&finddata, 0, sizeof(finddata));
		hFind = FindFirstFileW(szRootPathBuf, &finddata);
		if (hFind == INVALID_HANDLE_VALUE)
			return 0;
		do {
			if (!((wcscmp(finddata.cFileName, L".") == 0) || (wcscmp(finddata.cFileName, L"..") == 0)))
			{
				bool badd = false;
				szRootPathBuf[len1] = 0;
				szArchivePathBuf[len2] = 0;
				wcscat_s(szRootPathBuf, MAX_PATH, finddata.cFileName);
				wcscat_s(szArchivePathBuf, MAX_PATH, finddata.cFileName);

				if (fpFileCheck == NULL)
					badd = true;
				else
					badd = fpFileCheck(szRootPathBuf, szArchivePathBuf, &finddata, cbparam);

				if (badd)
				{
					if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						wcscat_s(szRootPathBuf, MAX_PATH, L"\\");
						wcscat_s(szArchivePathBuf, MAX_PATH, L"\\");
						totalFiles += findFiles_subproc(lstFiles, szRootPathBuf, szArchivePathBuf, fpFileCheck, cbparam);
					}
					else {
						lstFiles.push_back(JsSevenZipHelper::Compress::FileListItem<wchar_t>(false, false, szRootPathBuf, szArchivePathBuf));
						totalFiles++;
					}
				}
			}
			memset(&finddata, 0, sizeof(finddata));
		} while (FindNextFileW(hFind, &finddata));
		FindClose(hFind);

		return totalFiles;
	}

}