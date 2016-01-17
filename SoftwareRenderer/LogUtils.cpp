//
//  LogUtils.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 30/12/15.
//  Copyright (c) 2015 Enrique Alcor Martín. All rights reserved.
//

#include "LogUtils.h"

#include <cstdarg>
#include <cstdio>
#include <string>

#ifdef _WINDOWS
#include <Windows.h>
#endif

namespace omb
{

void Log::Debug(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int bufferSize = 1024;
	char buff[bufferSize];
	vsnprintf(buff, bufferSize, fmt, args);
#ifdef _WINDOWS
	OutputDebugString(buff);
#elif defined(__APPLE__)
	printf("%s\n", buff);
#endif
	va_end(args);
}

}