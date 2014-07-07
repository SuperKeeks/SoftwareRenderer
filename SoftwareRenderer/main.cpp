// Inspired by http://jonbho.net/2013/08/26/spacecrash-day-1-of-7-game-skeleton/

#include "SoftwareRenderer.h"
#include "Utils.h"

#include "stdafx.h"
#include "base.h"
#include "sys.h"

GLuint openGLTexId = 1;
omb::SoftwareRenderer renderer;
const dword width_pow2 = 512;
const dword height_pow2 = 512;
const dword width = 512;
const sdword height = 512;

//-----------------------------------------------------------------------------
void StartGame()
{
	renderer.init(width, height);
	renderer.clear(omb::Color(255, 0, 0, 255));
	
	glGenTextures(1, &openGLTexId);
	glBindTexture(GL_TEXTURE_2D, openGLTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_pow2, height_pow2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

//-----------------------------------------------------------------------------
void EndGame()
{
	glDeleteTextures(1, &openGLTexId);
}

//-----------------------------------------------------------------------------
void Render()
{
	//glClear(GL_COLOR_BUFFER_BIT);
	static uint8_t r = 0;
	++r;
	renderer.clear(omb::Color(r, 0, 0, 255));
	
	const vec2 p0 = vmake(0, 0);
	const vec2 p1 = vmake(G_WIDTH,G_HEIGHT);
	
	glBindTexture(GL_TEXTURE_2D, openGLTexId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, renderer.getFrameBuffer());
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2f(p0.x, p0.y);
	glTexCoord2d(1.0, 0.0); glVertex2f(p1.x, p0.y);
	glTexCoord2d(1.0, 1.0); glVertex2f(p1.x, p1.y);
	glTexCoord2d(0.0, 1.0); glVertex2f(p0.x, p1.y);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void RunGame()
{
}

//-----------------------------------------------------------------------------
void ProcessInput()
{
}

//-----------------------------------------------------------------------------
// Game state (apart from entities & other stand-alone modules)
float g_time = 0.f;

//-----------------------------------------------------------------------------
// Main
int Main(void)
{
	// Start things up & load resources ---------------------------------------------------
	StartGame();
	
	// Set up rendering ---------------------------------------------------------------------
	glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
	glClearColor( 0.0f, 0.1f, 0.3f, 0.0f );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, G_WIDTH, 0.0, G_HEIGHT, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Main game loop! ======================================================================
	while (!SYS_GottaQuit())
	{
		Render();
		ProcessInput();
		RunGame();
		SYS_Show();
		SYS_Pump();
		SYS_Sleep(16);
		g_time += 1.f/60.f;
	}
	EndGame();
	
	return 0;
}