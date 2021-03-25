/**
 * @file	SmartPointer.h
 * @class	SmartPointer
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/30
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#pragma once

namespace JsCPPUtils
{
	class ISmartPointer
	{
	private:
		int m_refCount;
		void *m_pthis;

	public:
		void sp_addRef()
		{
			m_refCount++;
		}

		void sp_delRef()
		{
			m_refCount--;
			if(m_refCount == 0)
			{
				sp_release();
			}
		}

		void sp_release()
		{
			delete m_pthis;
		}

		ISmartPointer(void *pthis) : 
			m_refCount(0),
			m_pthis(pthis)
		{

		}
	};

	template <class T>
	class SmartPointer {
	public:
	
	private:
		T *m_ptr;

		void release()
		{
			if(m_ptr)
				m_ptr->sp_delRef();
			m_ptr = NULL;
		}

	public:
		SmartPointer() :
		  m_ptr(NULL)
		{
		
		}
		
		SmartPointer(T* p) // 1
		{
			m_ptr = p;
			if(m_ptr != NULL)
				p->sp_addRef();
		}

		SmartPointer(const SmartPointer<T>& refObj)
		{
			m_ptr = refObj.m_ptr;
			if(m_ptr != NULL)
				m_ptr->sp_addRef();
		}

		~SmartPointer()
		{
			release();
		}

		T* operator->() const
		{
			return m_ptr;
		}

		T& operator *() const
		{
			return *m_ptr;
		}

		SmartPointer<T>& operator=(T* p) // 2
		{
			if(m_ptr != NULL)
				m_ptr->sp_delRef();
			if (p != NULL)
				p->sp_addRef();
			m_ptr = p;
			return *this;
		}

		SmartPointer<T>& operator=(const SmartPointer<T>& refObj)
		{
			if(m_ptr != NULL)
				m_ptr->sp_delRef();
			if (refObj.m_ptr != NULL)
				refObj.m_ptr->sp_addRef();
			m_ptr = refObj.m_ptr;
			return *this;
		}
		
		/*
		T* operator=(T* p)
		{
			if (p != NULL)
				p->sp_addRef();
			if (_p != NULL)
				_p->sp_delRef();
			m_ptr = p;
			return m_ptr;
		}

		T* operator=(const SmartPointer<T>& lp)
		{
			return (*this = lp.m_ptr);
		}
		*/
		
		bool operator!() const
		{
			return (m_ptr == NULL);
		}

		bool operator==(T* p) const
		{
			return m_ptr == p;
		}

		void Attach(T* p2)
		{
			Release();
			_p = p2;
		}

		T* Detach()
		{
			T* pt = _p;
			_p = NULL;
			return pt;
		}
	};

}
