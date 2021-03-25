/**
 * @file	Archive.h
 * @class	Archive
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/28
 * @brief	JsSevenZipHelper Archive class
 */

#pragma once

#pragma push_macro("INITGUID")
#define INITGUID
#include <guiddef.h>

#include <Archive/IArchive.h>

#include <JsCPPUtils/WinLibrary.h>
#include <JsCPPUtils/SmartPointer.h>

/*87 CLSID_CArchiveHandler : TGUID = '{23170F69-40C1-278A-1000-000110000000}'; 
88   CLSID_CFormatZip      : TGUID = '{23170F69-40C1-278A-1000-000110010000}'; 
89   CLSID_CFormatBZ2      : TGUID = '{23170F69-40C1-278A-1000-000110020000}'; 
90   CLSID_CFormatRar      : TGUID = '{23170F69-40C1-278A-1000-000110030000}'; 
91   CLSID_CFormatArj      : TGUID = '{23170F69-40C1-278A-1000-000110040000}'; 
92   CLSID_CFormatZ        : TGUID = '{23170F69-40C1-278A-1000-000110050000}'; 
93   CLSID_CFormatLzh      : TGUID = '{23170F69-40C1-278A-1000-000110060000}'; 
94   CLSID_CFormat7z       : TGUID = '{23170F69-40C1-278A-1000-000110070000}'; 
95   CLSID_CFormatCab      : TGUID = '{23170F69-40C1-278A-1000-000110080000}'; 
96   CLSID_CFormatNsis     : TGUID = '{23170F69-40C1-278A-1000-000110090000}'; 
97   //CLSID_CFormatLzma   : TGUID = '{23170F69-40C1-278A-1000-0001100A0000}';  not in 4.57 
98   CLSID_CFormatCompound : TGUID = '{23170F69-40C1-278A-1000-000110E50000}'; 
99   CLSID_CFormatWim      : TGUID = '{23170F69-40C1-278A-1000-000110E60000}'; 
100   CLSID_CFormatIso     : TGUID = '{23170F69-40C1-278A-1000-000110E70000}'; 
101   //CLSID_CFormatBkf   : TGUID = '{23170F69-40C1-278A-1000-000110E80000}';  not in 4.57 
102   CLSID_CFormatChm     : TGUID = '{23170F69-40C1-278A-1000-000110E90000}'; 
103   CLSID_CFormatSplit   : TGUID = '{23170F69-40C1-278A-1000-000110EA0000}'; 
104   CLSID_CFormatRpm     : TGUID = '{23170F69-40C1-278A-1000-000110EB0000}'; 
105   CLSID_CFormatDeb     : TGUID = '{23170F69-40C1-278A-1000-000110EC0000}'; 
106   CLSID_CFormatCpio    : TGUID = '{23170F69-40C1-278A-1000-000110ED0000}'; 
107   CLSID_CFormatTar     : TGUID = '{23170F69-40C1-278A-1000-000110EE0000}'; 
108   CLSID_CFormatGZip    : TGUID = '{23170F69-40C1-278A-1000-000110EF0000}'; */

// use another CLSIDs, if you want to support other formats (zip, rar, ...).
// {23170F69-40C1-278A-1000-000110070000}
DEFINE_GUID(CLSID_CFormatZip,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatBZ2,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatRar,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatArj,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatZ,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x05, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatLzh,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x06, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormat7z,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatCab,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x09, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatNsis,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x09, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatLzma,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0A, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatCompound,	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xE5, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatWim,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xE6, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatIso,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xE7, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatBkf,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xE8, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatChm,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xE9, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatSplit,	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEA, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatRpm,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEB, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatDeb,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEC, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatCpio,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xED, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatTar,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEE, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatGZip,		0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0xEF, 0x00, 0x00);

namespace JsSevenZipHelper
{
	class Archive
	{
	private:
		typedef UINT32 (WINAPI *CreateObjectFunc)(
			const GUID *clsID,
			const GUID *interfaceID,
			void **outObject);

		JsCPPUtils::WinLibrary *m_sevenZipLibPtr;
		JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> m_sevenZipLibSP;

		void postLibLoaded();

	protected:
		CreateObjectFunc m_fpCreateObject;

	public:
		Archive();
		Archive(const Archive& refobj);
		Archive(JsCPPUtils::WinLibrary *pSevenZipLib);
		Archive(JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> spSevenZipLib);
		~Archive();
		void setSevenLib(JsCPPUtils::WinLibrary *pSevenZipLib);
		void setSevenLib(JsCPPUtils::SmartPointer<JsCPPUtils::WinLibrary> spSevenZipLib);
		bool isLibLoaded() const;
		DWORD getLibErrno() const;

		static const GUID *getFormatClsidByExt(const char *szExt);
		static const GUID *getFormatClsidByExt(const wchar_t *szExt);
	};

}

#pragma pop_macro("INITGUID")
