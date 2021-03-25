/**
 * @file	SmartPointerCharArray.h
 * @class	SmartPointerCharArray
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/10/01
 * @brief	SmartPointerCharArray
 */

#pragma once

#include <stdlib.h>
#include <string.h>

#include "SmartPointer.h"

namespace JsCPPUtils
{
	template <typename T>
	class SmartArrayPointer : public ISmartPointer
	{
	private:
		T *m_ptr;
		int m_size;
		int m_length;

	public:
		SmartArrayPointer() : 
		ISmartPointer(this),
		  m_ptr(NULL), m_size(0), m_length(0)
		{
			
		}

		bool alloc(int size)
		{
			free();
			m_size = size;
			m_length = 0;
			m_ptr = (T*)::malloc(sizeof(T) * size);
			if(m_ptr == NULL)
			{
				std::bad_alloc e;
				throw e;
				return false;
			}else{
				return true;
			}
		}

		bool free()
		{
			if(m_ptr != NULL)
			{
				::free(m_ptr);
				m_ptr = NULL;
				m_size = 0;
				m_length = 0;
				return true;
			}else{
				return false;
			}
		}

		operator T*() const
		{
			return m_ptr;
		}

		T* getPtr()
		{
			return m_ptr;
		}

		int getSize()
		{
			return m_length;
		}

		int getLength()
		{
			return m_length;
		}

		void setLength(int len)
		{
			m_length = len;
		}
	};

}