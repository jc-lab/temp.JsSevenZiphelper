/**
 * @file	Random.h
 * @class	Random
 * @author	Jichan (jic5760@naver.com)
 * @date	2016/09/30
 * @brief	Random
 */

#pragma once

#include "Common.h"

namespace JsCPPUtils
{

	class Random
	{
	public:
		virtual bool nextBoolean() = 0;
		virtual void nextBytes(char *bytes, int len) = 0;
		virtual float nextFloat() = 0;
		virtual double nextDouble() = 0;
		virtual int32_t nextInt() = 0;
		virtual int32_t nextInt(int n) = 0;
	};

}

