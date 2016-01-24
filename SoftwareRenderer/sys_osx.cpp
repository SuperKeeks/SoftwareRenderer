#include "stdafx.h"
#include "base.h"
#include "sys.h"

#include "Vector2.h"

extern int Main(void);

int main(int argc, char *argv[])
{
	int retval = -1;
	
	if (glfwInit() == GL_TRUE)
	{
		if (glfwOpenWindow(SYS_WIDTH, SYS_HEIGHT, 0, 0, 0, 0, 8, 0, SYS_FULLSCREEN ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_TRUE) /* rgba, depth, stencil */
		{
			retval = Main();
			glfwCloseWindow();
		}
		glfwTerminate();
	}
	return retval;
}

namespace omb
{

void SYS_Pump()
{
	// GLFW takes care...
}

void SYS_Show()
{
	glfwSwapBuffers();
}

bool SYS_GottaQuit()
{
	return glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED);
}

bool SYS_KeyPressed(int key)
{
	return glfwGetKey(key);
}

Vector2i SYS_MousePos()
{
	Vector2i pos;
	glfwGetMousePos(&pos.x, &pos.y);
	pos.y = SYS_HEIGHT - pos.y;
	return pos;
}

bool SYS_MouseButonPressed(int button)
{
	return glfwGetMouseButton(button);
}

double SYS_GetTime()
{
	return glfwGetTime();
}

}