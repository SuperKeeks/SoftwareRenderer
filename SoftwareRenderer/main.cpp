#include "DemoController.h"

#include "LogUtils.h"

#include "stdafx.h"
#include "base.h"
#include "sys.h"

#include <ctime>

double prevFrameTime;

DemoController demoController;

int Main(void)
{
	prevFrameTime = glfwGetTime();
	demoController.init();
	while (!SYS_GottaQuit())
	{
		const double currentTime = glfwGetTime();
		const float dt = currentTime - prevFrameTime;
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
