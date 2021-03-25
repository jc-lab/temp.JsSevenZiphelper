/**
 * @file	Compress.h
 * @class	Compress
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/06
 * @brief	JsSevenZipHelper Compress class
 */

#pragma once

#include <list>

#include "Archive.h"

#include <IPassword.h>

#include "MyCom.h"
#include "Stream.h"

#include <JsCPPUtils/FileInfo.h>

namespace JsSevenZipHelper
{
	class Compress : public Archive
	{
	public:
		typedef bool(*FindFileCheckFuncA_t)(const char *cszFilePath, const char *cszArchivePath, LPWIN32_FIND_DATAA lpfinddata, void *cbparam);
		typedef bool(*FindFileCheckFuncW_t)(const wchar_t *cszFilePath, const wchar_t *cszArchivePath, LPWIN32_FIND_DATAW lpfinddata, void *cbparam);

		template <typename CHARTYPE>
		class FileListItem {
		public:
			bool isDir;
			bool isAnti;
			std::basic_string<CHARTYPE> strFilePath;
			std::basic_string<CHARTYPE> strArchivePath;

			FileListItem(const bool _isDir, const bool _isAnti, const std::basic_string<CHARTYPE>& _strFilePath, const std::basic_string<CHARTYPE>& _strArchivePath) :
				isDir(_isDir),
				isAnti(_isAnti),
				strFilePath(_strFilePath),
				strArchivePath(_strArchivePath)
			{
			}
		};

		template <typename CHARTYPE>
		class ArchiveFileInfo {
		public:
			bool isDir;
			bool isAnti;
			JsCPPUtils::basic_FileInfo<CHARTYPE> fileInfo;
			std::basic_string<CHARTYPE> strArchivePath;

			bool isNewFile;

			ArchiveFileInfo(const bool _isDir, const bool _isAnti, const std::basic_string<CHARTYPE>& _strFilePath, const std::basic_string<CHARTYPE>& _strArchivePath) :
				isDir(_isDir),
				isAnti(_isAnti),
				fileInfo(_strFilePath.c_str()),
				strArchivePath(_strArchivePath),
				isNewFile(true)
			{
			}
		};

	private:
		CMyComPtr<IOutArchive> *m_pOutArcObj;

		static int findFiles_subproc(std::list<JsSevenZipHelper::Compress::FileListItem<char> > &lstFiles, char *szRootPathBuf, char *szArchivePathBuf, FindFileCheckFuncA_t fpFileCheck, void *cbparam);
		static int findFiles_subproc(std::list<JsSevenZipHelper::Compress::FileListItem<wchar_t> > &lstFiles, wchar_t *szRootPathBuf, wchar_t *szArchivePathBuf, FindFileCheckFuncW_t fpFileCheck, void *cbparam);

	public:
		Compress();
		Compress(JsCPPUtils::WinLibrary *pSevenZipLib);
		Compress(JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> spSevenZipLib);
		Compress(const Archive& archive);
		virtual ~Compress();
		HRESULT createArchive(const GUID *pFormatGUID);
		void closeArchive();
		
		HRESULT compress(ISequentialOutStream *pOutStreamSpec, const std::list<FileListItem<char> >& fileList, bool isDefinedPassword, bool isHeaderEncrypt, const char *cszPassword);
		HRESULT compress(ISequentialOutStream *pOutStreamSpec, const std::list<FileListItem<wchar_t> >& fileList, bool isDefinedPassword, bool isHeaderEncrypt, const wchar_t *cszPassword);

		static int findFiles(std::list<JsSevenZipHelper::Compress::FileListItem<char> > &lstFiles, const char *cszRootPath, const char *cszArchivePath, FindFileCheckFuncA_t fpFileCheck = NULL, void *cbparam = NULL);
		static int findFiles(std::list<JsSevenZipHelper::Compress::FileListItem<wchar_t> > &lstFiles, const wchar_t *cszRootPath, const wchar_t *cszArchivePath, FindFileCheckFuncW_t fpFileCheck = NULL, void *cbparam = NULL);
	};

}
