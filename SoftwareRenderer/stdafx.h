//
//  sys.h
//  SoftwareRenderer
//
//  The contents of this file have been mostly copied from the skeleton source of the following course:
//  http://jonbho.net/2013/08/25/spacecrash-designing-a-one-week-game/
//
//  Created by Enrique Alcor on 25/06/2014.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

// stdafx.h
#ifndef _STDAFX_H_
#define _STDAFX_H_

//=============================================================================
#ifdef _WINDOWS

#pragma pack(1)
#pragma warning(disable:4996) // Using open/close/read... for file access
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <GL/glu.h>
#include <io.h>

//=============================================================================
#elif defined(__APPLE__)

#include "TargetConditionals.h"
#if defined(__MACH__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
#include <unistd.h>
#include <GL/glfw.h>
#define GL_BGRA_EXT GL_BGRA

#endif
#endif

//=============================================================================
// Common includes

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#endif