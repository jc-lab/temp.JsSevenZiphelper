/**
 * @file	Archive.h
 * @class	Archive
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/28
 * @brief	JsSevenZipHelper Stream class
 */

#include "Stream.h"

namespace JsSevenZipHelper {

	// ============================== FileInStream ==============================
	
	FileInStream::FileInStream() :
		m_hFile(INVALID_HANDLE_VALUE),
		m_offset(0)
	{
	
	}
	
	FileInStream::~FileInStream()
	{
		Close();
	}

	bool FileInStream::Create(LPCTSTR fileName)
	{
		return Create(fileName, true);
	}

	bool FileInStream::Open(LPCSTR fileName)
	{
		m_hFile = ::CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileInStream::Open(LPCWSTR fileName)
	{
		m_hFile = ::CreateFileW(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileInStream::Create(LPCSTR fileName, bool createAlways)
	{
		return Open(fileName, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
	}

	bool FileInStream::Create(LPCWSTR fileName, bool createAlways)
	{
		return Open(fileName, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS);
	}

	bool FileInStream::Open(LPCSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
	{
		m_hFile = ::CreateFileA(fileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileInStream::Open(LPCWSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
	{
		m_hFile = ::CreateFileW(fileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileInStream::setOffset(UInt64 offset)
	{
		m_offset = offset;
		return Seek(0, FILE_BEGIN, NULL);
	}

	DWORD FileInStream::getErrno()
	{
		return m_dwLastErr;
	}

	HRESULT FileInStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
	{
		DWORD dwWrittenSize = 0;
		if(!::WriteFile(m_hFile, data, size, &dwWrittenSize, NULL))
			return E_FAIL;
		if(processedSize != NULL)
			*processedSize = dwWrittenSize;
		Flush();
		return S_OK;
	}

	HRESULT FileInStream::Read(void *data, UInt32 size, UInt32 *processedSize)
	{
		DWORD dwWrittenSize = 0;
		if(!::ReadFile(m_hFile, data, size, &dwWrittenSize, NULL))
			return E_FAIL;
		if(processedSize != NULL)
			*processedSize = dwWrittenSize;
		return S_OK;
	}

	HRESULT FileInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
	{
		BOOL bRst;
		LARGE_INTEGER linewsize;
		LARGE_INTEGER licursize;
		
		linewsize.QuadPart = m_offset + offset;
		bRst = ::SetFilePointerEx(m_hFile, linewsize, &licursize, seekOrigin);
		if(bRst)
		{
			if(licursize.QuadPart > m_offset)
				licursize.QuadPart -= m_offset;
			if(newPosition)
				*newPosition = licursize.QuadPart;
			return S_OK;
		}
		else
			return E_FAIL;
	}

	HRESULT FileInStream::SetSize(UInt64 newSize)
	{
		UInt64 newPosition = 0;
		if (!Seek(newSize, FILE_BEGIN, &newPosition))
			return E_FAIL;
		if (newPosition != newSize)
			return E_FAIL;
		return SetEndOfFile();
	}

	HRESULT FileInStream::SetEndOfFile()
	{
		if(::SetEndOfFile(m_hFile))
			return S_OK;
		else
			return E_FAIL;
	}

	HRESULT FileInStream::GetSize(UInt64 *size)
	{
		DWORD sizeHigh;
		DWORD sizeLow = ::GetFileSize(m_hFile, &sizeHigh);
		if (sizeLow == 0xFFFFFFFF)
			if (::GetLastError() != NO_ERROR)
				return E_FAIL;
		*size = (((UInt64)sizeHigh) << 32) + sizeLow;
		return S_OK;
	}

	HRESULT FileInStream::Flush()
	{
		if(FlushFileBuffers(m_hFile))
			return S_OK;
		else
			return E_FAIL;
	}

	HRESULT FileInStream::Close()
	{
		if(m_hFile == INVALID_HANDLE_VALUE)
			return E_FAIL;

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;

		return S_OK;
	}

	HRESULT FileInStream::SetTime(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime)
	{
		if(SetFileTime(m_hFile, cTime, aTime, mTime))
			return S_OK;
		else
			return E_FAIL;
	}

	HRESULT FileInStream::SetMTime(const FILETIME *mTime)
	{
		return SetTime(NULL, NULL, mTime);
	}

	// ============================== FileInStream ==============================

	// ============================== MemoryInSteram ==============================

	MemoryInStream::MemoryInStream() :
			m_isExternalBuf(false),
			m_dataBufSize(0),
			m_dataBufPos(0),
			m_dataBuf(NULL),
			m_dataSize(0)
	{
	
	}

	MemoryInStream::~MemoryInStream()
	{
		Close();
	}

	void MemoryInStream::setNativePtr(char *ptr, UInt64 size)
	{
		if (m_dataBuf && !m_isExternalBuf)
		{
			free(m_dataBuf);
		}
		m_dataBuf = ptr;
		m_dataBufSize = size;
		m_dataBufPos = 0;
		m_dataSize = size;
		m_isExternalBuf = true;
	}

	bool MemoryInStream::resizeBuf(UInt64 needSize)
	{
		char *newPtr;
		UInt64 newSize = needSize;
		if(newSize % BLOCKSIZE)
			newSize += BLOCKSIZE - (newSize % BLOCKSIZE);

		if (m_isExternalBuf)
			return false;

		newPtr = (char*)realloc(m_dataBuf, newSize);
		if(newPtr == NULL)
			return false;

		m_dataBuf = newPtr;

		if(newSize > m_dataBufSize)
			memset(&m_dataBuf[m_dataBufSize], 0, newSize - m_dataBufSize);

		m_dataBufSize = newSize;

		return true;
	}

	HRESULT MemoryInStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
	{
		UInt64 needSize = m_dataBufPos + size;

		if (m_isExternalBuf)
			return E_FAIL;

		if(needSize > m_dataBufSize)
		{
			if(!resizeBuf(needSize))
				return E_FAIL;
		}

		if(needSize > m_dataSize)
			m_dataSize = needSize;

		memcpy(&m_dataBuf[m_dataBufPos], data, size);
		m_dataBufPos += size;

		if(processedSize != NULL)
			*processedSize = size;
		
		return S_OK;
	}

	HRESULT MemoryInStream::Read(void *data, UInt32 size, UInt32 *processedSize)
	{
		UInt64 remainLen = m_dataSize - m_dataBufPos;
		UInt32 availLen;

		if(remainLen > size)
			availLen = size;
		else
			availLen = (UInt32)remainLen;

		memcpy(data, &m_dataBuf[m_dataBufPos], availLen);
		m_dataBufPos += availLen;

		if (processedSize != NULL)
			*processedSize = availLen;

		return S_OK;
	}

	HRESULT MemoryInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
	{
		UInt64 newPos;

		switch(seekOrigin)
		{
		case FILE_BEGIN:
			newPos = offset;
			break;

		case FILE_CURRENT:
			newPos = m_dataBufPos + offset;
			break;

		case FILE_END:
			newPos = m_dataSize + offset;
			break;

		default:
			return E_FAIL;
		}

		if(newPos < 0)
			return E_FAIL;

		if(newPos > m_dataBufSize)
		{
			if(!resizeBuf(newPos))
				return E_FAIL;
		}

		if(newPos > m_dataSize)
		{
			m_dataSize = newPos;
		}

		m_dataBufPos = newPos;

		if(newPosition != NULL)
			*newPosition = m_dataBufPos;

		return S_OK;
	}

	HRESULT MemoryInStream::SetSize(UInt64 newSize)
	{
		if(newSize > m_dataBufSize)
		{
			if(!resizeBuf(newSize))
				return E_FAIL;
		}

		m_dataSize = newSize;

		return SetEndOfFile();
	}

	HRESULT MemoryInStream::SetEndOfFile()
	{
		// No supported
		return S_OK;
	}

	HRESULT MemoryInStream::GetSize(UInt64 *size)
	{
		*size = m_dataSize;
		return S_OK;
	}

	HRESULT MemoryInStream::Close()
	{
		if(!m_isExternalBuf && m_dataBuf != NULL)
		{
			free(m_dataBuf);
			m_dataBuf = NULL;
			m_dataBufSize = 0;
			m_dataBufPos = 0;
			m_dataSize = 0;
		}

		return S_OK;
	}

	HRESULT MemoryInStream::SetTime(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime)
	{
		// No supported
		return S_OK;
	}

	HRESULT MemoryInStream::SetMTime(const FILETIME *mTime)
	{
		// No supported
		return S_OK;
	}
	
	// ============================== MemoryInStream ==============================


	// ============================== FileOutStream ==============================

	FileOutStream::FileOutStream() :
		m_hFile(INVALID_HANDLE_VALUE)
	{
	
	}
	
	FileOutStream::~FileOutStream()
	{
		Close();
	}

	bool FileOutStream::Create(LPCSTR fileName)
	{
		return Create(fileName, true);
	}

	bool FileOutStream::Create(LPCWSTR fileName)
	{
		return Create(fileName, true);
	}

	bool FileOutStream::Open(LPCSTR fileName)
	{
		m_hFile = ::CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileOutStream::Open(LPCWSTR fileName)
	{
		m_hFile = ::CreateFileW(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileOutStream::Create(LPCSTR fileName, bool createAlways)
	{
		return Open(fileName, GENERIC_READ | GENERIC_WRITE, 0, createAlways ? CREATE_ALWAYS : OPEN_ALWAYS);
	}

	bool FileOutStream::Create(LPCWSTR fileName, bool createAlways)
	{
		return Open(fileName, GENERIC_READ | GENERIC_WRITE, 0, createAlways ? CREATE_ALWAYS : OPEN_ALWAYS);
	}

	bool FileOutStream::Open(LPCSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
	{
		m_hFile = ::CreateFileA(fileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	bool FileOutStream::Open(LPCWSTR fileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
	{
		m_hFile = ::CreateFileW(fileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_dwLastErr = GetLastError();
			return false;
		}
		m_dwLastErr = 0;
		return true;
	}

	DWORD FileOutStream::getErrno()
	{
		return m_dwLastErr;
	}

	HRESULT FileOutStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
	{
		DWORD dwWrittenSize = 0;
		if(!::WriteFile(m_hFile, data, size, &dwWrittenSize, NULL))
			return E_FAIL;
		if(processedSize != NULL)
			*processedSize = dwWrittenSize;
		Flush();
		return S_OK;
	}

	HRESULT FileOutStream::Read(void *data, UInt32 size, UInt32 *processedSize)
	{
		DWORD dwWrittenSize = 0;
		if(!::ReadFile(m_hFile, data, size, &dwWrittenSize, NULL))
			return E_FAIL;
		if(processedSize != NULL)
			*processedSize = dwWrittenSize;
		return S_OK;
	}

	HRESULT FileOutStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
	{
		LONG high = (LONG)(offset >> 32);
		DWORD low = ::SetFilePointer(m_hFile, (LONG)(offset & 0xFFFFFFFF), &high, seekOrigin);
		if (low == 0xFFFFFFFF)
			if (::GetLastError() != NO_ERROR)
				return E_FAIL;
		if(newPosition != NULL)
			*newPosition = (((UInt64)(UInt32)high) << 32) + low;
		return S_OK;
	}

	HRESULT FileOutStream::SetSize(UInt64 newSize)
	{
		UInt64 newPosition = 0;
		if (!Seek(newSize, FILE_BEGIN, &newPosition))
			return E_FAIL;
		if (newPosition != newSize)
			return E_FAIL;
		return SetEndOfFile();
	}

	HRESULT FileOutStream::SetEndOfFile()
	{
		if(::SetEndOfFile(m_hFile))
			return S_OK;
		else
			return E_FAIL;
	}

	HRESULT FileOutStream::GetSize(UInt64 *size)
	{
		DWORD sizeHigh;
		DWORD sizeLow = ::GetFileSize(m_hFile, &sizeHigh);
		if (sizeLow == 0xFFFFFFFF)
			if (::GetLastError() != NO_ERROR)
				return E_FAIL;
		*size = (((UInt64)sizeHigh) << 32) + sizeLow;
		return S_OK;
	}

	HRESULT FileOutStream::Flush()
	{
		if(FlushFileBuffers(m_hFile))
			return S_OK;
		else
			return E_FAIL;
	}

	HRESULT FileOutStream::Close()
	{
		if(m_hFile == INVALID_HANDLE_VALUE)
			return E_FAIL;

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;

		return S_OK;
	}

	HRESULT FileOutStream::SetTime(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime)
	{
		if(SetFileTime(m_hFile, cTime, aTime, mTime))
			return S_OK;
		else
			return E_FAIL;
	}

	HRESULT FileOutStream::SetMTime(const FILETIME *mTime)
	{
		return SetTime(NULL, NULL, mTime);
	}

	// ============================== FileOutStream ==============================

	// ============================== MemoryOutSteram ==============================

	MemoryOutStream::MemoryOutStream() :
			m_dataBufSize(0),
			m_dataBufPos(0),
			m_dataBuf(NULL),
			m_dataSize(0)
	{
	
	}

	MemoryOutStream::~MemoryOutStream()
	{
		Close();
	}

	bool MemoryOutStream::resizeBuf(UInt64 needSize)
	{
		char *newPtr;
		UInt64 newSize = needSize;
		if(newSize % BLOCKSIZE)
			newSize += BLOCKSIZE - (newSize % BLOCKSIZE);
		newPtr = (char*)realloc(m_dataBuf, newSize);
		if(newPtr == NULL)
			return false;

		m_dataBuf = newPtr;

		if(newSize > m_dataBufSize)
			memset(&m_dataBuf[m_dataBufSize], 0, newSize - m_dataBufSize);

		m_dataBufSize = newSize;

		return true;
	}

	HRESULT MemoryOutStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
	{
		UInt64 needSize = m_dataBufPos + size;

		if(needSize > m_dataBufSize)
		{
			if(!resizeBuf(needSize))
				return E_FAIL;
		}

		if(needSize > m_dataSize)
			m_dataSize = needSize;

		memcpy(&m_dataBuf[m_dataBufPos], data, size);
		m_dataBufPos += size;

		if(processedSize != NULL)
			*processedSize = size;
		
		return S_OK;
	}

	HRESULT MemoryOutStream::Read(void *data, UInt32 size, UInt32 *processedSize)
	{
		UInt64 remainLen = m_dataSize - m_dataBufPos;
		UInt32 availLen;

		if(remainLen > size)
			availLen = size;
		else
			availLen = remainLen;

		memcpy(data, &m_dataBuf[m_dataBufPos], availLen);
		m_dataBufPos += availLen;

		return S_OK;
	}

	HRESULT MemoryOutStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
	{
		UInt64 newPos;

		switch(seekOrigin)
		{
		case FILE_BEGIN:
			newPos = offset;
			break;

		case FILE_CURRENT:
			newPos = m_dataBufPos + offset;
			break;

		case FILE_END:
			newPos = m_dataSize + offset;
			break;

		default:
			return E_FAIL;
		}

		if(newPos < 0)
			return E_FAIL;

		if(newPos > m_dataBufSize)
		{
			if(!resizeBuf(newPos))
				return E_FAIL;
		}

		if(newPos > m_dataSize)
		{
			m_dataSize = newPos;
		}

		m_dataBufPos = newPos;

		if(newPosition != NULL)
			*newPosition = m_dataBufPos;

		return S_OK;
	}

	HRESULT MemoryOutStream::SetSize(UInt64 newSize)
	{
		if(newSize > m_dataBufSize)
		{
			if(!resizeBuf(newSize))
				return E_FAIL;
		}

		m_dataSize = newSize;

		return SetEndOfFile();
	}

	HRESULT MemoryOutStream::SetEndOfFile()
	{
		// No supported
		return S_OK;
	}

	HRESULT MemoryOutStream::GetSize(UInt64 *size)
	{
		*size = m_dataSize;
		return S_OK;
	}

	HRESULT MemoryOutStream::Close()
	{
		if(m_dataBuf != NULL)
		{
			free(m_dataBuf);
			m_dataBuf = NULL;
			m_dataBufSize = 0;
			m_dataBufPos = 0;
			m_dataSize = 0;
		}

		return S_OK;
	}

	HRESULT MemoryOutStream::SetTime(const FILETIME *cTime, const FILETIME *aTime, const FILETIME *mTime)
	{
		// No supported
		return S_OK;
	}

	HRESULT MemoryOutStream::SetMTime(const FILETIME *mTime)
	{
		// No supported
		return S_OK;
	}

	char *MemoryOutStream::getNativeBuffer()
	{
		return m_dataBuf;
	}

	UInt64 MemoryOutStream::getNativeLength()
	{
		return m_dataSize;
	}
	
	// ============================== MemoryOutStream ==============================

}