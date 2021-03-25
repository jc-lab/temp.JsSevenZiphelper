#include "StringBuffer.h"

namespace JsCPPUtils
{

	StringBuffer::StringBuffer() : 
		m_blocksize(128),
		m_bufsize(0),
		m_bufpos(0),
		m_pbuf(NULL)

	{

	}

	StringBuffer::StringBuffer(int blocksize) : 
		m_blocksize(128),
		m_bufsize(0),
		m_bufpos(0),
		m_pbuf(NULL)
	{
		
	}


	StringBuffer::~StringBuffer()
	{
		if(m_pbuf != NULL)
		{
			free(m_pbuf);
			m_pbuf = NULL;
		}
		m_bufsize = 0;
		m_bufpos = 0;
	}

	void StringBuffer::putData(const char *data, size_t size)
	{
		size_t needsize = m_bufpos + size + 1;
		if(needsize > m_bufsize)
		{
			char *pnewptr;
			size_t newsize = needsize;
			if(newsize % m_blocksize)
				newsize += m_blocksize - (newsize % m_blocksize);
			pnewptr = (char*)realloc(m_pbuf, newsize);
			if(pnewptr == NULL)
			{
				std::bad_alloc e;
				throw e;
			}
			m_bufsize = newsize;
			m_pbuf = pnewptr;
		}

		memcpy(&m_pbuf[m_bufpos], data, size);
		m_bufpos += size;
		m_pbuf[m_bufpos] = '\0';
	}

	void StringBuffer::clear()
	{
		m_bufpos = 0;
	}
	
	void StringBuffer::append(const char *szAppend, int len)
	{
		putData(szAppend, len);
	}

	void StringBuffer::append(const StringBuffer& sbAppend)
	{
		putData(sbAppend.m_pbuf, sbAppend.m_bufpos);
	}

	void StringBuffer::append(const char *szAppend)
	{
		putData(szAppend, strlen(szAppend));
	}

	void StringBuffer::append(const std::string& strAppend)
	{
		putData(strAppend.c_str(), strAppend.length());
	}
	
	void StringBuffer::append(bool bvalue)
	{
		static const char cszTrue[] = "true";
		static const char cszFalse[] = "false";
		if(bvalue)
		{
			putData(cszTrue, 4);
		}else{
			putData(cszFalse, 5);
		}
	}
	
	void StringBuffer::appendHex(unsigned char x, bool uppercase)
	{
		static const char cszHex_lower[] = "0123456789abcdef";
		static const char cszHex_upper[] = "0123456789ABCDEF";
		const char *cszHex;
		char tmpbuf[2];
		if(uppercase)
			cszHex = cszHex_upper;
		else
			cszHex = cszHex_lower;
		tmpbuf[0] = cszHex[(x >> 4) & 0x0F];
		tmpbuf[1] = cszHex[(x     ) & 0x0F];
		putData(tmpbuf, 2);
	}
	
	void StringBuffer::appendHex(unsigned char x)
	{
		appendHex(x, false);
	}
	
	bool StringBuffer::append(uint32_t unvalue, int radix)
	{
		static const char cszHex_lower[] = "0123456789abcdef";

		char tmpbuf[32];
		char *ppos = &tmpbuf[sizeof(tmpbuf)];
		int n = 0;

		if((radix <= 1) || (radix > 16))
			return false;

		if(unvalue == 0)
		{
			*--ppos = '0';
			n = 1;
		}else{
			while(unvalue != 0)
			{
				int x = unvalue % radix;
				*--ppos = cszHex_lower[x];
				n++;
				unvalue /= radix;
			}
		}

		putData(ppos, n);

		return true;
	}

	bool StringBuffer::append(int32_t nvalue, int radix)
	{
		static const char cszHex_lower[] = "0123456789abcdef";

		char tmpbuf[32];
		char *ppos = &tmpbuf[sizeof(tmpbuf)];
		int n = 0;

		if((radix <= 1) || (radix > 16))
			return false;

		if(nvalue == 0)
		{
			*--ppos = '0';
			n = 1;
		}else{
			bool isNegNum = (nvalue < 0);
			while(nvalue != 0)
			{
				int x = nvalue % radix;
				if(x<0) x *= -1;
				*--ppos = cszHex_lower[x];
				n++;
				nvalue /= radix;
			}
			if(isNegNum)
			{
				*--ppos = '-';
				n++;
			}
		}

		putData(ppos, n);

		return true;
	}
	
	bool StringBuffer::append(uint64_t unvalue, int radix)
	{
		static const char cszHex_lower[] = "0123456789abcdef";

		char tmpbuf[32];
		char *ppos = &tmpbuf[sizeof(tmpbuf)];
		int n = 0;

		if((radix <= 1) || (radix > 16))
			return false;

		if(unvalue == 0)
		{
			*--ppos = '0';
			n = 1;
		}else{
			while(unvalue != 0)
			{
				int x = unvalue % radix;
				*--ppos = cszHex_lower[x];
				n++;
				unvalue /= radix;
			}
		}

		putData(ppos, n);

		return true;
	}

	bool StringBuffer::append(int64_t nvalue, int radix)
	{
		static const char cszHex_lower[] = "0123456789abcdef";

		char tmpbuf[32];
		char *ppos = &tmpbuf[sizeof(tmpbuf)];
		int n = 0;

		if((radix <= 1) || (radix > 16))
			return false;

		if(nvalue == 0)
		{
			*--ppos = '0';
			n = 1;
		}else{
			bool isNegNum = (nvalue < 0);
			while(nvalue != 0)
			{
				int x = nvalue % radix;
				if(x<0) x *= -1;
				*--ppos = cszHex_lower[x];
				n++;
				nvalue /= radix;
			}
			if(isNegNum)
			{
				*--ppos = '-';
				n++;
			}
		}

		putData(ppos, n);

		return true;
	}

	bool StringBuffer::append(uint32_t unvalue)
	{
		return append(unvalue, 10);
	}
	bool StringBuffer::append(int32_t nvalue)
	{
		return append(nvalue, 10);
	}
	bool StringBuffer::append(uint64_t unvalue)
	{
		return append(unvalue, 10);
	}
	bool StringBuffer::append(int64_t nvalue)
	{
		return append(nvalue, 10);
	}
	
	void StringBuffer::appendHexBytes(unsigned char *pdata, int length, bool uppercase)
	{
		static const char cszHex_lower[] = "0123456789abcdef";
		static const char cszHex_upper[] = "0123456789ABCDEF";
		const char *cszHex;
		int i;
		char tmpbuf[2];
		if(uppercase)
			cszHex = cszHex_upper;
		else
			cszHex = cszHex_lower;
		for(i=0; i<length; i++)
		{
			unsigned char x = pdata[i];
			tmpbuf[0] = cszHex[(x >> 4) & 0x0F];
			tmpbuf[1] = cszHex[(x     ) & 0x0F];
			putData(tmpbuf, 2);
		}
	}
	
	void StringBuffer::appendHexBytes(unsigned char *pdata, int length)
	{
		appendHexBytes(pdata, length, false);
	}
	
	void StringBuffer::operator=(const StringBuffer& sbAppend)
	{
		clear();
		putData(sbAppend.m_pbuf, sbAppend.m_bufpos);
	}
	void StringBuffer::operator=(const std::string& strAppend)
	{
		clear();
		putData(strAppend.c_str(), strAppend.length());
	}
	void StringBuffer::operator=(const char *szAppend)
	{
		clear();
		putData(szAppend, strlen(szAppend));
	}
	void StringBuffer::operator=(bool value)
	{
		clear();
		append(value);
	}
	void StringBuffer::operator=(uint32_t value)
	{
		clear();
		append(value, 10);
	}
	void StringBuffer::operator=(int32_t value)
	{
		clear();
		append(value, 10);
	}
	void StringBuffer::operator=(uint64_t value)
	{
		clear();
		append(value, 10);
	}
	void StringBuffer::operator=(int64_t value)
	{
		clear();
		append(value, 10);
	}

	void StringBuffer::operator+=(const StringBuffer& sbAppend)
	{
		append(sbAppend);
	}
	void StringBuffer::operator+=(const std::string& strAppend)
	{
		append(strAppend);
	}
	void StringBuffer::operator+=(const char *szAppend)
	{
		append(szAppend);
	}
	void StringBuffer::operator+=(bool value)
	{
		clear();
		append(value);
	}
	void StringBuffer::operator+=(uint32_t value)
	{
		clear();
		append(value, 10);
	}
	void StringBuffer::operator+=(int32_t value)
	{
		clear();
		append(value, 10);
	}
	void StringBuffer::operator+=(uint64_t value)
	{
		clear();
		append(value, 10);
	}
	void StringBuffer::operator+=(int64_t value)
	{
		clear();
		append(value, 10);
	}

	const StringBuffer& StringBuffer::operator+(const StringBuffer& sbAppend)
	{
		append(sbAppend);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(const std::string& strAppend)
	{
		append(strAppend);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(const char *szAppend)
	{
		append(szAppend);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(bool value)
	{
		append(value);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(uint32_t value)
	{
		append(value, 10);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(int32_t value)
	{
		append(value, 10);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(uint64_t value)
	{
		append(value, 10);
		return *this;
	}
	const StringBuffer& StringBuffer::operator+(int64_t value)
	{
		append(value, 10);
		return *this;
	}
	
	bool StringBuffer::operator==(const StringBuffer &ref) const
	{
		if(m_bufpos != ref.m_bufpos)
			return false;
		return (::memcmp(m_pbuf, ref.m_pbuf, m_bufpos) == 0);
	}

	std::string StringBuffer::toString() const
	{
		return std::string(m_pbuf, m_bufpos);
	}
	
	int StringBuffer::length() const
	{
		return m_bufpos;
	}

	const char *StringBuffer::c_str() const
	{
		return m_pbuf;
	}
}
