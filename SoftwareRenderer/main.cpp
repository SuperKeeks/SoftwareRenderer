#include "DemoController.h"

#include "LogUtils.h"

#include "stdafx.h"
#include "sys.h"

#include <ctime>

using namespace omb;

double prevFrameTime;
DemoController demoController;

int Main(void)
{
	prevFrameTime = SYS_GetTime();
	demoController.init();
	while (!SYS_GottaQuit())
	{
		const double currentTime = SYS_GetTime();
		const double dt = currentTime - prevFrameTime;
		//Log::Debug("\ndt: %.5f", dt);
		demoController.update((float)dt);
		demoController.draw();
		SYS_Show();
		SYS_Pump();
		//Log::Debug("\nFPS: %.1f", 1/dt);
		prevFrameTime = currentTime;
	}
	demoController.end();
	
	return 0;
}
