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

#ifndef _SYS_H_
#define _SYS_H_

namespace omb
{

struct Vector2i;

//========================================================================================
// Resolution (real & virtual)

#define SYS_WIDTH  512
#define SYS_HEIGHT 512
#define SYS_FULLSCREEN 0

//========================================================================================
// Platform layer

void SYS_Pump();
void SYS_Show();
bool SYS_GottaQuit();
bool SYS_KeyPressed(int key);
Vector2i SYS_MousePos();
bool SYS_MouseButonPressed(int button);
double SYS_GetTime();

//-----------------------------------------------------------------------------
#ifdef _WINDOWS

#define SYS_KEY_UP       VK_UP
#define SYS_KEY_DOWN     VK_DOWN
#define SYS_KEY_LEFT     VK_LEFT
#define SYS_KEY_RIGHT    VK_RIGHT
#define SYS_KEY_PAGEUP   VK_PRIOR
#define SYS_KEY_PAGEDOWN VK_NEXT

#define SYS_MB_LEFT      VK_LBUTTON
#define SYS_MB_RIGHT     VK_RBUTTON
#define SYS_MB_MIDDLE    VK_MBUTTON

//-----------------------------------------------------------------------------
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if defined(__MACH__) && TARGET_OS_MAC && !TARGET_OS_IPHONE

#define SYS_KEY_UP       GLFW_KEY_UP
#define SYS_KEY_DOWN     GLFW_KEY_DOWN
#define SYS_KEY_LEFT     GLFW_KEY_LEFT
#define SYS_KEY_RIGHT    GLFW_KEY_RIGHT
#define SYS_KEY_PAGEUP   GLFW_KEY_PAGEUP
#define SYS_KEY_PAGEDOWN GLFW_KEY_PAGEDOWN

#define SYS_MB_LEFT      GLFW_MOUSE_BUTTON_LEFT
#define SYS_MB_RIGHT     GLFW_MOUSE_BUTTON_RIGHT
#define SYS_MB_MIDDLE    GLFW_MOUSE_BUTTON_MIDDLE

#endif //defined(__MACH__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
#endif //defined(__APPLE__)
//-----------------------------------------------------------------------------
}
#endif
