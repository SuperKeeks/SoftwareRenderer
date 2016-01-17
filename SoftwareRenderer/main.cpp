#include "DemoController.h"

#include "LogUtils.h"

#include "stdafx.h"
#include "base.h"
#include "sys.h"

#include <ctime>

clock_t prevFrameTime;

DemoController demoController;

int Main(void)
{
	prevFrameTime = clock();
	demoController.init();
	while (!SYS_GottaQuit())
	{
		const clock_t currentTime = clock();
		const float dt = (((float)(currentTime - prevFrameTime)) / CLOCKS_PER_SEC);
		//Log::Debug("\ndt: %.5f", dt);
		demoController.update(dt);
		demoController.draw();
		SYS_Show();
		SYS_Pump();
		//Log::Debug("\nFPS: %.1f", 1/dt);
		prevFrameTime = currentTime;
	}
	demoController.end();
	
	return 0;
}
