/**
 * @file	Lockable.h
 * @class	Lockable
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/category/JsCPPUtils )
 * @date	2016/10/14
 * @copyright Copyright (C) 2016 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the MIT license.  See the LICENSE file for details.
 */

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#ifndef __JSCPPUTILS_LOCKABLE_H__
#define __JSCPPUTILS_LOCKABLE_H__

#include "Common.h"

#if defined(JSCUTILS_OS_WINDOWS)
#include <windows.h>
#elif defined(JSCUTILS_OS_LINUX)
#include <pthread.h>
#endif

namespace JsCPPUtils
{

	class Lockable
	{
	private:
#if defined(JSCUTILS_OS_WINDOWS)
		CRITICAL_SECTION m_cs;
#elif defined(JSCUTILS_OS_LINUX)
		pthread_mutex_t m_mutex;
#endif

	public:
		Lockable();
		~Lockable();
		int lock() const;
		int unlock() const;
	};

	class LockableEx
	{
	private:
#if defined(JSCUTILS_OS_WINDOWS)
		CRITICAL_SECTION m_cs;
#elif defined(JSCUTILS_OS_LINUX)
		pthread_mutex_t m_mutex;
#endif

	public:
		LockableEx();
		~LockableEx();
		int lock() const;
		int unlock() const;
	};

	class LockableRW
	{
	private:
		enum LockFlag
		{
			LF_WRITE_MASK = 0x7FF00000,
			LF_WRITE_FLAG = 0x00100000,
			LF_READ_MASK  = 0x000FFFFF ///< ���� 20��Ʈ�� readlock�� ���� �÷��׷� ����Ѵ�.
		};
#if defined(JSCUTILS_OS_WINDOWS)
		volatile LONG m_syslock;
#elif defined(JSCUTILS_OS_LINUX)
		pthread_rwlock_t m_syslock;
#endif

	public:
		LockableRW();
		~LockableRW();
		int writelock() const;
		int writeunlock() const;
		int readlock() const;
		int readunlock() const;
	};

};

#endif
