/**
 * @file	Stream.h
 * @class	Stream
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/28
 * @brief	JsSevenZipHelper Stream class
 */

#pragma once

#include <string>

#include <Windows.h>

#include "MyCom.h"
#include <IStream.h>

namespace JsSevenZipHelper {
	class FileInStream : public IInStream, public CMyUnknownImp
	{
	protected:
		HANDLE m_hFile;
		DWORD m_dwLastErr;

		UInt64 m_offset;

	public:
		MY_UNKNOWN_IMP1(IInStream)

		bool Create(LPCSTR fileName);
		bool Create(LPCWSTR fileName);
		bool Open(LPCSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
		bool Open(LPCWSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
		bool Create(LPCSTR fileName, bool createAlways);
		bool Create(LPCWSTR fileName, bool createAlways);
		bool Open(LPCSTR fileName);
		bool Open(LPCWSTR fileName);

		bool setOffset(UInt64 offset);

		DWORD getErrno();

		FileInStream();
		virtual ~FileInStream();

		STDMETHOD(SetTime)(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime);
		STDMETHOD(SetMTime)(const FILETIME *mTime);
		STDMETHOD(SetSize)(UInt64 newSize);
		STDMETHOD(GetSize)(UInt64 *size);
		STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
		STDMETHOD(SetEndOfFile)();
		STDMETHOD(Flush)();
		STDMETHOD(Close)();
	};

	class FileOutStream : public IOutStream, public CMyUnknownImp
	{
	protected:
		HANDLE m_hFile;
		DWORD m_dwLastErr;

	public:
		MY_UNKNOWN_IMP1(IOutStream)

		bool Create(LPCSTR fileName);
		bool Create(LPCWSTR fileName);
		bool Open(LPCSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
		bool Open(LPCWSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition);
		bool Create(LPCSTR fileName, bool createAlways);
		bool Create(LPCWSTR fileName, bool createAlways);
		bool Open(LPCSTR fileName);
		bool Open(LPCWSTR fileName);

		DWORD getErrno();

		FileOutStream();
		virtual ~FileOutStream();

		STDMETHOD(SetTime)(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime);
		STDMETHOD(SetMTime)(const FILETIME *mTime);
		STDMETHOD(SetSize)(UInt64 newSize);
		STDMETHOD(GetSize)(UInt64 *size);
		STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
		STDMETHOD(SetEndOfFile)();
		STDMETHOD(Flush)();
		STDMETHOD(Close)();
	};

	class MemoryInStream : public IInStream, public CMyUnknownImp
	{
	protected:
		static const int BLOCKSIZE = 4096;

	private:
		bool m_isExternalBuf;
		UInt64 m_dataBufSize;
		UInt64 m_dataBufPos;
		UInt64 m_dataSize;
		char *m_dataBuf;
		bool resizeBuf(UInt64 needSize);

	public:
		MY_UNKNOWN_IMP1(IInStream)

		MemoryInStream();
		virtual ~MemoryInStream();
		STDMETHOD(SetTime)(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime);
		STDMETHOD(SetMTime)(const FILETIME *mTime);
		STDMETHOD(SetSize)(UInt64 newSize);
		STDMETHOD(GetSize)(UInt64 *size);
		STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
		STDMETHOD(SetEndOfFile)();
		STDMETHOD(Close)();

		void setNativePtr(char *ptr, UInt64 size);
	};

	class MemoryOutStream : public IOutStream, public CMyUnknownImp
	{
	protected:
		static const int BLOCKSIZE = 4096;

	private:
		UInt64 m_dataBufSize;
		UInt64 m_dataBufPos;
		UInt64 m_dataSize;
		char *m_dataBuf;
		bool resizeBuf(UInt64 needSize);

	public:
		MY_UNKNOWN_IMP1(IOutStream)

		MemoryOutStream();
		virtual ~MemoryOutStream();
		STDMETHOD(SetTime)(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime);
		STDMETHOD(SetMTime)(const FILETIME *mTime);
		STDMETHOD(SetSize)(UInt64 newSize);
		STDMETHOD(GetSize)(UInt64 *size);
		STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize);
		STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);
		STDMETHOD(SetEndOfFile)();
		STDMETHOD(Close)();

		char *getNativeBuffer();
		UInt64 getNativeLength();
	};
}
