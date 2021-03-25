/**
 * @file	Compress.h
 * @class	Compress
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/06
 * @brief	JsSevenZipHelper Compress class
 */

#pragma once

#include <tchar.h>

#include <list>
#include <map>

#include <Archive/IArchive.h>
#include <IPassword.h>

#include "Archive.h"
#include "MyCom.h"
#include "Stream.h"

namespace JsSevenZipHelper
{
	class Extract : public Archive
	{
	public:
		template<typename CHARTYPE>
		class FileListItem {
		public:
			bool isDir;
			bool isAnti;
			std::basic_string<CHARTYPE> strArchivePath;
			DWORD fileAttrib;
			bool fileAttribDefined;
			FILETIME fileCtime;
			bool     fileCtimeDefined;
			FILETIME fileAtime;
			bool     fileAtimeDefined;
			FILETIME fileMtime;
			bool     fileMtimeDefined;
			int64_t newFileSize;

			FileListItem() : 
				isDir(false),
				isAnti(false),
				fileAttrib(0),
				fileAttribDefined(false),
				fileCtimeDefined(false),
				fileAtimeDefined(false),
				fileMtimeDefined(false),
				newFileSize(-1)
			{
				fileCtime.dwHighDateTime = 0;
				fileCtime.dwLowDateTime = 0;
				fileAtime.dwHighDateTime = 0;
				fileAtime.dwLowDateTime = 0;
				fileMtime.dwHighDateTime = 0;
				fileMtime.dwLowDateTime = 0;
			}
		};

	private:
		class MyArchiveOpenCallback :
			public IArchiveOpenCallback,
			public ICryptoGetTextPassword,
			public ICryptoGetTextPassword2,
			public CMyUnknownImp
		{
		private:
			bool m_isDefinedPassword;
			std::basic_string<wchar_t> m_strPassword;

		public:
			MyArchiveOpenCallback(bool isDefinedPassword, const wchar_t *szPassword);
			MyArchiveOpenCallback(bool isDefinedPassword, const std::basic_string<wchar_t> &strPassword);
			virtual ~MyArchiveOpenCallback();

			HRESULT CheckBreak();

			MY_QUERYINTERFACE_BEGIN2(IArchiveOpenCallback)
				MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword)
				MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword2)
			MY_QUERYINTERFACE_END
			MY_ADDREF_RELEASE

			// IArchiveUpdateCallback
			INTERFACE_IArchiveOpenCallback(;)
			/*
				STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes) x; \
				STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes) x; \
			*/
			
			// ICryptoGetTextPassword
			STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);
			
			// ICryptoGetTextPassword2
			STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password);
		};

		CMyComPtr<IInArchive> m_pInArcObj;

		bool m_isDefinedPassword;
		std::basic_string<wchar_t> m_strPassword;

		static bool makeSubDirs(char *szDirPathBuf);
		static bool makeSubDirs(wchar_t *szDirPathBuf);

	public:
		Extract();
		Extract(JsCPPUtils::WinLibrary *pSevenZipLib);
		Extract(const Archive& archive);
		virtual ~Extract();
		HRESULT openArchive(const GUID *pFormatGUID, IInStream *pInStream, bool isDefinedPassword, const char *szPassword, UInt64 *pMaxCheckStartPosition = NULL);
		HRESULT openArchive(const GUID *pFormatGUID, IInStream *pInStream, bool isDefinedPassword, const wchar_t *szPassword, UInt64 *pMaxCheckStartPosition = NULL);
		void closeArchive();
		CComPtr<IInArchive> getInArcPtr();

		HRESULT extract(const std::basic_string<char>& strExtractRootPath, IArchiveExtractCallback *extractCallback, std::list<FileListItem<char> > *pFileList);
		HRESULT extract(const std::basic_string<wchar_t>& strExtractRootPath, IArchiveExtractCallback *extractCallback, std::list<FileListItem<wchar_t> > *pFileList);

		static bool makeDirs(const std::basic_string<char>& strDirPath);
		static bool makeDirs(const std::basic_string<wchar_t>& strDirPath);
		static std::basic_string<char> getParentDir(const std::basic_string<char>& strFilePath);
		static std::basic_string<wchar_t> getParentDir(const std::basic_string<wchar_t>& strFilePath);
	};
}
