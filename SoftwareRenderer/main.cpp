// Inspired by http://jonbho.net/2013/08/26/spacecrash-day-1-of-7-game-skeleton/

#include "Assert.h"
#include "Color.h"
#include "LogUtils.h"
#include "Matrix44.h"
#include "MathUtils.h"
#include "MD2.h"
#include "Quaternion.h"
#include "SoftwareRenderer.h"
#include "Vector3.h"
#include "Vertex.h"

#include "stdafx.h"
#include "base.h"
#include "sys.h"

#include <ctime>

using namespace omb;

GLuint openGLTexId = 1;
SoftwareRenderer renderer;
uint16_t avatarTexId;
uint16_t chessPatternTexId;
uint16_t cyberDemonTexId;
MD2Model* cyberDemonModel;
const dword width_pow2 = 512;
const dword height_pow2 = 512;
const dword width = 512;
const sdword height = 512;
float rotationDegX = 310.0f;
float rotationDegY = 0;
float rotationDegZ = 0;
//float scale = -0.009f; // Use this one for cyberdemon
float scale = 1.0f;
Vector3f posOffset(0, 0.5f, -1.25);
//Vector3f posOffset(0, 0,0);
bool usePerspective = true;
bool keyPressedLastFrame = false;

void DrawAvatar(const Matrix44&);
void DrawCube(const Matrix44&);
void DrawCyberDemon(const Matrix44&);
void DrawColouredTriangle(const Matrix44&);

//-----------------------------------------------------------------------------
void StartGame()
{
	renderer.init(width, height);
	avatarTexId = renderer.loadTexture("Resources/avatar.png");
	chessPatternTexId = renderer.loadTexture("Resources/chess_pattern.png");
	cyberDemonModel = MD2Utils::LoadModel("Resources/Cyber.md2");
	OMBAssert(cyberDemonModel, "Can't load model!");
	
	cyberDemonTexId = renderer.loadTexture("Resources/cyber.jpg");
	
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
	
	renderer.unloadTexture(avatarTexId);
}

//-----------------------------------------------------------------------------
void Render()
{
	//glClear(GL_COLOR_BUFFER_BIT); // Not necessary, as the image generated by SoftwareRenderer will be fullscreen
	renderer.clear(omb::Color(0, 0, 0, 255));
	
	// Perspective matrix
	const float frustumScale = 1.0f;
	const float zNear = 0.1f;
	const float zFar = 5.0f;
	Matrix44 persMatrix;
	persMatrix(0, 0) = frustumScale;
	persMatrix(1, 1) = frustumScale;
	persMatrix(2, 2) = (zFar + zNear) / (zNear - zFar);
	persMatrix(2, 3) = (2 * zFar * zNear) / (zNear - zFar);
	persMatrix(3, 2) = -1.0f;
	
	const Matrix44 scaleMatrix = MathUtils::CreateScaleMatrix(scale);
	
	const Matrix44 translationMatrix = MathUtils::CreateTranslationMatrix(posOffset.x, posOffset.y, posOffset.z);
	
	Quaternion rotationTransX(Vector3f(1.0f, 0, 0), MathUtils::DegToRad(rotationDegX));
	Quaternion rotationTransY(Vector3f(0, 1.0f, 0), MathUtils::DegToRad(rotationDegY));
	Quaternion rotationTransZ(Vector3f(0, 0, 1.0f), MathUtils::DegToRad(rotationDegZ));
	
	Quaternion allRotations = rotationTransX * rotationTransY * rotationTransZ;
	const Matrix44 allRotationsMatrix = allRotations;
	
	const Matrix44 modelTransform =  translationMatrix * allRotationsMatrix * scaleMatrix;
	
	renderer.setViewProjectionMatrix(usePerspective ? persMatrix : MathUtils::CreateIdentityMatrix());
	DrawAvatar(modelTransform);
	//DrawCube(modelTransform);
	//DrawColouredTriangle(modelTransform);
	
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
	const bool left = SYS_KeyPressed(SYS_KEY_LEFT);
	const bool right = SYS_KeyPressed(SYS_KEY_RIGHT);
	const bool up = SYS_KeyPressed(SYS_KEY_UP);
	const bool down = SYS_KeyPressed(SYS_KEY_DOWN);
	const bool scaleUp = SYS_KeyPressed('W');
	const bool scaleDown = SYS_KeyPressed('S');
	const bool rotateCCWX = SYS_KeyPressed('A');
	const bool rotateCWX = SYS_KeyPressed('D');
	const bool rotateCCWY = SYS_KeyPressed('Q');
	const bool rotateCWY = SYS_KeyPressed('E');
	const bool rotateCCWZ = SYS_KeyPressed('Z');
	const bool rotateCWZ = SYS_KeyPressed('C');
	const bool swPerspective = SYS_KeyPressed('P');
	const bool swWireframe = SYS_KeyPressed('1');
	const bool swBackFaceCulling = SYS_KeyPressed('2');
	
	float keyPressedThisFrame = left || right || up || down ||
		scaleUp || scaleDown ||
		rotateCCWX || rotateCWX ||
		rotateCCWY || rotateCWY ||
		rotateCCWZ || rotateCWZ ||
		swPerspective || swWireframe || swBackFaceCulling;
	
	if (keyPressedLastFrame)
	{
		keyPressedLastFrame = keyPressedThisFrame;
		return;
	}
	
	const float rotationInc = 5.0f;
	const float scaleInc = 0.5f;
	
	if (rotateCWX)
	{
		rotationDegX -= rotationInc;
		if (rotationDegX < 0)
		{
			rotationDegX = 360.0f - rotationInc;
		}
		printf("\nRotationX: %.1f", rotationDegX);
	}
	else if (rotateCCWX)
	{
		rotationDegX += rotationInc;
		if (rotationDegX > 360.0f)
		{
			rotationDegX = rotationInc;
		}
		printf("\nRotationX: %.1f", rotationDegX);
	}
	
	if (rotateCWY)
	{
		rotationDegY -= rotationInc;
		if (rotationDegY < 0)
		{
			rotationDegY = 360.0f - rotationInc;
		}
		printf("\nRotationY: %.1f", rotationDegY);
	}
	else if (rotateCCWY)
	{
		rotationDegY += rotationInc;
		if (rotationDegY > 360.0f)
		{
			rotationDegY = rotationInc;
		}
		printf("\nRotationY: %.1f", rotationDegY);
	}
	
	if (rotateCWZ)
	{
		rotationDegZ -= rotationInc;
		if (rotationDegZ < 0)
		{
			rotationDegZ = 360.0f - rotationInc;
		}
		printf("\nRotationZ: %.1f", rotationDegZ);
	}
	else if (rotateCCWZ)
	{
		rotationDegZ += rotationInc;
		if (rotationDegZ > 360.0f)
		{
			rotationDegZ = rotationInc;
		}
		printf("\nRotationZ: %.1f", rotationDegZ);
	}
	
	if (scaleUp)
	{
		scale += scaleInc;
		printf("\nScale: %.2f", scale);
	}
	else if (scaleDown)
	{
		scale -= scaleInc;
		printf("\nScale: %.2f", scale);
	}
	
	const float kOffsetInc = 0.01f;
	
	if (left)
	{
		posOffset.x -= kOffsetInc;
	}
	
	if (right)
	{
		posOffset.x += kOffsetInc;
	}
	
	if (up)
	{
		posOffset.y += kOffsetInc;
	}
	
	if (down)
	{
		posOffset.y -= kOffsetInc;
	}
	
	if (swPerspective && !keyPressedLastFrame)
	{
		usePerspective = !usePerspective;
		
		if (usePerspective)
		{
			printf("\nProjection: Perspective");
		}
		else
		{
			printf("\nProjection: Orthogonal");
		}
	}
	
	if (swWireframe && !keyPressedLastFrame)
	{
		renderer.setWireFrameModeEnabled(!renderer.getWireFrameModeEnabled());
		printf("\nWireframe mode: %d", renderer.getWireFrameModeEnabled());
	}
	
	if (swBackFaceCulling && !keyPressedLastFrame)
	{
		renderer.setBackFaceCullingEnabled(!renderer.getBackFaceCullingEnabled());
		printf("\nBack face culling enabled: %d", renderer.getBackFaceCullingEnabled());
	}
	
	//keyPressedLastFrame = swPerspective || swWireframe || swBackFaceCulling;
	keyPressedLastFrame = keyPressedThisFrame;
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
		const clock_t startTime = clock();
		Render();
		ProcessInput();
		RunGame();
		SYS_Show();
		SYS_Pump();
		g_time += 1.f/60.f;
		const clock_t endTime = clock();
		Log::Debug("\nFPS: %.1f", 1/(((float)(endTime - startTime)) / CLOCKS_PER_SEC));
	}
	EndGame();
	
	return 0;
}

void DrawAvatar(const Matrix44& modelMatrix)
{
	const Vector2f topLeft(0, 0);
	const Vector2f topRight(1.0f, 0);
	const Vector2f bottomLeft(0, 1.0f);
	const Vector2f bottomRight(1.0f, 1.0f);
	
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vector4f(1.0f,  1.0f, 0, 1.0f), topRight));
	vertices.push_back(Vertex(Vector4f(1.0f, -1.0f, 0, 1.0f), bottomRight));
	vertices.push_back(Vertex(Vector4f(-1.0f,  1.0f, 0, 1.0f), topLeft));
	vertices.push_back(Vertex(Vector4f(1.0f, -1.0f, 0, 1.0f), bottomRight));
	vertices.push_back(Vertex(Vector4f(-1.0f, -1.0f, 0, 1.0f), bottomLeft));
	vertices.push_back(Vertex(Vector4f(-1.0f,  1.0f, 0, 1.0f), topLeft));
	
	renderer.bindTexture(avatarTexId);
	renderer.drawTriangles(vertices, modelMatrix);
	renderer.unbindTexture();
}

void DrawCube(const Matrix44& modelMatrix)
{
	const Color red(255, 0, 0, 255);
	const Color green(0, 255, 0, 255);
	const Color blue(0, 0, 255, 255);
	const Color yellow(255, 255, 0, 255);
	const Color orange(255, 69, 0, 255);
	const Color pink(255, 20, 147, 255);
	
	const Vector2f topLeft(0, 0);
	const Vector2f topRight(1.0f, 0);
	const Vector2f bottomLeft(0, 1.0f);
	const Vector2f bottomRight(1.0f, 1.0f);
	
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -1.25f, 1.0f), topRight));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -1.25f, 1.0f), bottomRight));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -1.25f, 1.0f), topLeft));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -1.25f, 1.0f), bottomRight));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -1.25f, 1.0f), bottomLeft));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -1.25f, 1.0f), topLeft));
	
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -2.75f, 1.0f), bottomRight));
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -1.25f, 1.0f), topRight));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -1.25f, 1.0f), topLeft));
	
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -2.75f, 1.0f), bottomRight));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -1.25f, 1.0f), topLeft));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -2.75f, 1.0f), bottomLeft));
	
	renderer.bindTexture(avatarTexId);
	renderer.drawTriangles(vertices, modelMatrix);
	renderer.unbindTexture();
	
	vertices.clear();
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -2.75f, 1.0f), red));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -2.75f, 1.0f), red));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -2.75f, 1.0f), red));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -2.75f, 1.0f), red));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -2.75f, 1.0f), red));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -2.75f, 1.0f), red));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -1.25f, 1.0f), green));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -1.25f, 1.0f), green));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -2.75f, 1.0f), green));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -1.25f, 1.0f), green));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -2.75f, 1.0f), green));
	vertices.push_back(Vertex(Vector4f(-0.25f,  0.25f, -2.75f, 1.0f), green));
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -1.25f, 1.0f), yellow));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -2.75f, 1.0f), yellow));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -1.25f, 1.0f), yellow));
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -1.25f, 1.0f), yellow));
	vertices.push_back(Vertex(Vector4f(0.25f,  0.25f, -2.75f, 1.0f), yellow));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -2.75f, 1.0f), yellow));
	
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -2.75f, 1.0f), pink));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -1.25f, 1.0f), pink));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -1.25f, 1.0f), pink));
	vertices.push_back(Vertex(Vector4f(0.25f, -0.25f, -2.75f, 1.0f), pink));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -2.75f, 1.0f), pink));
	vertices.push_back(Vertex(Vector4f(-0.25f, -0.25f, -1.25f, 1.0f), pink));
	
	renderer.drawTriangles(vertices, modelMatrix);
}

void DrawCyberDemon(const Matrix44& modelMatrix)
{
	std::vector<Vertex> vertices;
	
	const MD2Frame& frame = cyberDemonModel->m_frames[0];
	for (int i = 0; i < cyberDemonModel->m_header.m_numTriangles; ++i)
	{
		const MD2Triangle& triangle = cyberDemonModel->m_triangles[i];
		for (int j = 0; j < 3; ++j)
		{
			const MD2Vertex& md2Vertex = frame.m_vertices[triangle.m_vertexIdxs[j]];
			
			Vector4f pos;
			pos.x = (md2Vertex.m_position[0] * frame.m_scale.x) + frame.m_translate.x;
			pos.y = (md2Vertex.m_position[1] * frame.m_scale.y) + frame.m_translate.y;
			pos.z = (md2Vertex.m_position[2] * frame.m_scale.z) + frame.m_translate.z;
			pos.w = 1.0f;
			
			Vector2f texCoord;
			texCoord.x = (float)cyberDemonModel->m_texCoords[triangle.m_texCoordIdxs[j]].m_s / cyberDemonModel->m_header.m_textureWidth;
			texCoord.y = (float)cyberDemonModel->m_texCoords[triangle.m_texCoordIdxs[j]].m_t / cyberDemonModel->m_header.m_textureHeight;
			
			vertices.push_back(Vertex(pos, texCoord));
		}
	}
	
	renderer.bindTexture(cyberDemonTexId);
	renderer.drawTriangles(vertices, modelMatrix);
	renderer.unbindTexture();
}

void DrawColouredTriangle(const Matrix44& modelMatrix)
{
	std::vector<Vertex> vertices;
	
	vertices.push_back(Vertex(Vector4f(-0.5f, 0.5f, 0), Color(255, 0, 0, 255)));
	vertices.push_back(Vertex(Vector4f(0.5f, 0.5f, 0), Color(0, 255, 0, 255)));
	vertices.push_back(Vertex(Vector4f(0, -0.5f, 0), Color(0, 0, 255, 255)));
	/*vertices.push_back(Vertex(Vector4f(-0.5f, 0.5f, 0), Color(255, 0, 0, 255)));
	vertices.push_back(Vertex(Vector4f(0, -0.5f, 0), Color(0, 0, 255, 255)));
	vertices.push_back(Vertex(Vector4f(0.5f, 0.5f, 0), Color(0, 255, 0, 255)));*/

	renderer.drawTriangleStrip(vertices, modelMatrix);
}