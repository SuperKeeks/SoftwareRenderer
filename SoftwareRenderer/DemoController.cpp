//
//  DemoController.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 17/1/16.
//  Copyright (c) 2016 Enrique Alcor Martín. All rights reserved.
//

#include "DemoController.h"

#include "LogUtils.h"
#include "MathUtils.h"
#include "MD2.h"
#include "SizeOfArray.h"
#include "Vector4.h"
#include "Vertex.h"

#include "stdafx.h"
#include "sys.h"

using namespace omb;

DemoController::DemoController()
{
}

void DemoController::init()
{
	initOpenGL();
	initRenderer();
	initModels();
	printUsage();
}

void DemoController::end()
{
	m_renderer.unloadTexture(m_avatarTexId);
	m_renderer.unloadTexture(m_chessPatternTexId);
	m_renderer.unloadTexture(m_cyberDemonTexId);
	
	MD2Utils::UnloadMode(m_cyberDemonModel);
	
	glDeleteTextures(1, &m_openGLTexId);
}

void DemoController::update(float dt)
{
	updateInput(dt);
	
	if (m_selectedModel == SelectedModel::Cyberdemon)
	{
		updateCyberDemon(dt);
	}
}

void DemoController::updateInput(float dt)
{
	updateInputSwitches(dt);
	updateInputMove(dt);
	updateInputRotation(dt);
	updateInputScale(dt);
	
	m_lastMousePos = SYS_MousePos();
}

void DemoController::updateInputSwitches(float dt)
{
	const int kCyberdemonKey = '1';
	const int kAvatarKey = '2';
	const int kTriangleKey = '3';
	const int kChesspatternKey = '4';
	const int kCubeKey = '5';
	const int kSwitchPerspectiveKey = 'P';
	const int kSwitchWireframeModeKey = 'W';
	const int kSwitchBackfacecullingKey = 'B';
	const int kSwitchAnimInterpKey = 'I';
	const int kSwitchRenderModeKey = 'R';
	const int kPlayAnimationsKey = SYS_KEY_SPACE;
	const int kSwitchKeys[] = {
		kCyberdemonKey,
		kAvatarKey,
		kTriangleKey,
		kChesspatternKey,
		kCubeKey,
		kSwitchPerspectiveKey,
		kSwitchWireframeModeKey,
		kSwitchBackfacecullingKey,
		kSwitchAnimInterpKey,
		kSwitchRenderModeKey,
		kPlayAnimationsKey};
	bool switchKeyPressedThisFrame = false;
	for (int i = 0; i < sizeofarray(kSwitchKeys); ++i)
	{
		if (SYS_KeyPressed(kSwitchKeys[i]))
		{
			switchKeyPressedThisFrame = true;
		}
	}
	
	// Some keys can't be keeped pressed
	if (switchKeyPressedThisFrame && !m_switchKeyPressedLastFrame)
	{
		// Model selection
		if (SYS_KeyPressed(kCyberdemonKey))
		{
			m_selectedModel = SelectedModel::Cyberdemon;
			m_selectedTransform = &m_cyberDemonTransform;
		}
		else if (SYS_KeyPressed(kAvatarKey))
		{
			m_selectedModel = SelectedModel::Avatar;
			m_selectedTransform = &m_avatarTransform;
		}
		else if (SYS_KeyPressed(kChesspatternKey))
		{
			m_selectedModel = SelectedModel::ChessPattern;
			m_selectedTransform = &m_avatarTransform;
		}
		else if (SYS_KeyPressed(kTriangleKey))
		{
			m_selectedModel = SelectedModel::Triangle;
			m_selectedTransform = &m_TriangleTransform;
		}
		else if (SYS_KeyPressed(kCubeKey))
		{
			m_selectedModel = SelectedModel::Cube;
			m_selectedTransform = &m_cubeTransform;
		}
		
		// Projection switch
		if (SYS_KeyPressed(kSwitchPerspectiveKey))
		{
			switchProjectionMode();
		}
		
		// Wireframe mode
		if (SYS_KeyPressed(kSwitchWireframeModeKey))
		{
			m_renderer.setWireFrameModeEnabled(!m_renderer.getWireFrameModeEnabled());
		}
		
		// Backface culling
		if (SYS_KeyPressed(kSwitchBackfacecullingKey))
		{
			m_renderer.setBackFaceCullingEnabled(!m_renderer.getBackFaceCullingEnabled());
			if (m_renderer.getBackFaceCullingEnabled())
			{
				Log::Debug("Enabled Back Face Culling");
			}
			else
			{
				Log::Debug("Disabled Back Face Culling");
			}
		}
		
		// Animation interpolation
		if (SYS_KeyPressed(kSwitchAnimInterpKey))
		{
			m_interpolateAnimations = !m_interpolateAnimations;
		}
		
		// Render mode switching
		if (SYS_KeyPressed(kSwitchRenderModeKey))
		{
			if (m_renderer.getRenderMode() == SoftwareRenderer::RenderMode::Scanlines)
			{
				m_renderer.setRenderMode(SoftwareRenderer::RenderMode::CheckPixelInTriangle);
				Log::Debug("Switched to CheckPixelInTriangle render mode");
			}
			else
			{
				m_renderer.setRenderMode(SoftwareRenderer::RenderMode::Scanlines);
				Log::Debug("Switched to Scanlines render mode");
			}
		}
		
		// Play/Pause animation
		if (SYS_KeyPressed(kPlayAnimationsKey))
		{
			m_playingAnimations = !m_playingAnimations;
		}
	}
	m_switchKeyPressedLastFrame = switchKeyPressedThisFrame;
}

void DemoController::updateInputMove(float dt)
{
	const int kMoveUpKey = SYS_KEY_UP;
	const int kMoveDownKey = SYS_KEY_DOWN;
	const int kMoveLeftKey = SYS_KEY_RIGHT;
	const int kMoveRightKey = SYS_KEY_LEFT;
	const int kMoveInKey = SYS_KEY_PAGEUP;
	const int kMoveOutKey = SYS_KEY_PAGEDOWN;
	const float kMoveMult = 2.0f;
	
	if (SYS_KeyPressed(kMoveUpKey))
	{
		m_selectedTransform->m_position.y += kMoveMult * dt;
	}
	
	if (SYS_KeyPressed(kMoveDownKey))
	{
		m_selectedTransform->m_position.y -= kMoveMult * dt;
	}
	
	if (SYS_KeyPressed(kMoveRightKey))
	{
		m_selectedTransform->m_position.x -= kMoveMult * dt;
	}
	
	if (SYS_KeyPressed(kMoveLeftKey))
	{
		m_selectedTransform->m_position.x += kMoveMult * dt;
	}
	
	if (SYS_KeyPressed(kMoveInKey))
	{
		m_selectedTransform->m_position.z -= kMoveMult * dt;
	}
	
	if (SYS_KeyPressed(kMoveOutKey))
	{
		m_selectedTransform->m_position.z += kMoveMult * dt;
	}
}

void DemoController::updateInputRotation(float dt)
{
	const float kMouseRotationFactor = 0.5f;
	const Vector2i newMousePos = SYS_MousePos();
	
	if (SYS_MouseButonPressed(0))
	{
		const Vector2i mousePosDiff = newMousePos - m_lastMousePos;
		m_selectedTransform->m_rotation.z += mousePosDiff.x * kMouseRotationFactor;
		m_selectedTransform->m_rotation.x += mousePosDiff.y * kMouseRotationFactor;
	}
}

void DemoController::updateInputScale(float dt)
{
	const float kScaleMult = 1.0f;
	const Vector2i newMousePos = SYS_MousePos();
	
	if (SYS_MouseButonPressed(1))
	{
		const Vector2i mousePosDiff = newMousePos - m_lastMousePos;
		const float scaleDiff = m_selectedTransform->m_scale.x * kScaleMult * mousePosDiff.y * dt;
		
		m_selectedTransform->m_scale.x -= scaleDiff;
		m_selectedTransform->m_scale.y -= scaleDiff;
		m_selectedTransform->m_scale.z -= scaleDiff;
	}
}

void DemoController::draw()
{
	m_renderer.clear(omb::Color(0, 0, 0, 255));
	
	switch (m_selectedModel)
	{
		case SelectedModel::Avatar:
			drawAvatar();
			break;
		case SelectedModel::ChessPattern:
			drawChessPattern();
			break;
		case SelectedModel::Cube:
			drawCube();
			break;
		case SelectedModel::Cyberdemon:
			drawCyberDemon();
			break;
		case SelectedModel::Triangle:
			drawTriangle();
			break;
	}
	
	// Tell OpenGL to display the final image (bit of irony on this :) )
	glBindTexture(GL_TEXTURE_2D, m_openGLTexId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kFrameBufferWidth, kFrameBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_renderer.getFrameBuffer());
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2f(0, 0);
	glTexCoord2d(1.0, 0.0); glVertex2f(SYS_WIDTH, 0);
	glTexCoord2d(1.0, 1.0); glVertex2f(SYS_WIDTH, SYS_HEIGHT);
	glTexCoord2d(0.0, 1.0); glVertex2f(0, SYS_HEIGHT);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DemoController::initOpenGL()
{
	glGenTextures(1, &m_openGLTexId);
	glBindTexture(GL_TEXTURE_2D, m_openGLTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kFrameBufferWidth, kFrameBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
	glClearColor( 0.0f, 0.1f, 0.3f, 0.0f );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SYS_WIDTH, 0.0, SYS_HEIGHT, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DemoController::initRenderer()
{
	m_renderer.init(kFrameBufferWidth, kFrameBufferHeight);
	m_renderer.setViewProjectionMatrix(MathUtils::CreatePerspectiveMatrix(kFrustumScale, kZNear, kZFar));
	m_perspectiveProjection = true;
}

void DemoController::initModels()
{
	// Coloured triangle
	m_TriangleVertices.reserve(3);
	m_TriangleVertices.push_back(Vertex(Vector4f(-0.5f, 0.5f, 0, 1.0f), Color(255, 0, 0, 255)));
	m_TriangleVertices.push_back(Vertex(Vector4f(0.5f, 0.5f, 0, 1.0f), Color(0, 255, 0, 255)));
	m_TriangleVertices.push_back(Vertex(Vector4f(0, -0.5f, 0, 1.0f), Color(0, 0, 255, 255)));
	m_TriangleTransform.m_position.z = -1.25f;
	
	// Avatar
	m_avatarTexId = m_renderer.loadTexture("Resources/avatar.png");
	const Vector2f topLeft(0, 0);
	const Vector2f topRight(1.0f, 0);
	const Vector2f bottomLeft(0, 1.0f);
	const Vector2f bottomRight(1.0f, 1.0f);
	m_avatarVertices.reserve(6);
	m_avatarVertices.push_back(Vertex(Vector4f(1.0f,  1.0f, 0, 1.0f), topRight));
	m_avatarVertices.push_back(Vertex(Vector4f(1.0f, -1.0f, 0, 1.0f), bottomRight));
	m_avatarVertices.push_back(Vertex(Vector4f(-1.0f,  1.0f, 0, 1.0f), topLeft));
	m_avatarVertices.push_back(Vertex(Vector4f(1.0f, -1.0f, 0, 1.0f), bottomRight));
	m_avatarVertices.push_back(Vertex(Vector4f(-1.0f, -1.0f, 0, 1.0f), bottomLeft));
	m_avatarVertices.push_back(Vertex(Vector4f(-1.0f,  1.0f, 0, 1.0f), topLeft));
	m_avatarTransform.m_position.y = 0.5f;
	m_avatarTransform.m_position.z = -1.25f;
	m_avatarTransform.m_rotation.x = 310.0f;
	
	// Chess pattern
	m_chessPatternTexId = m_renderer.loadTexture("Resources/chess_pattern.png");
	m_chessPatternTransform.m_position.y = 0.5f;
	m_chessPatternTransform.m_position.z = -1.25f;
	m_chessPatternTransform.m_rotation.x = 310.0f;
	
	// Cyberdemon
	m_cyberDemonModel = MD2Utils::LoadModel("Resources/Cyber.md2");
	m_cyberDemonTexId = m_renderer.loadTexture("Resources/cyber.jpg");
	m_cyberDemonTransform.m_rotation.x = 285.0f;
	m_cyberDemonTransform.m_rotation.y = 0;
	m_cyberDemonTransform.m_rotation.z = 265.0f;
	m_cyberDemonTransform.m_scale.x = 0.006f;
	m_cyberDemonTransform.m_scale.y = 0.006f;
	m_cyberDemonTransform.m_scale.z = 0.006f;
	m_cyberDemonTransform.m_position.x = 0;
	m_cyberDemonTransform.m_position.y = -0.6f;
	m_cyberDemonTransform.m_position.z = -1.25f;
	
	// Cube
	m_cubeTransform.m_position.x = 0.5f;
	m_cubeTransform.m_position.y = 1.0f;
	m_cubeTransform.m_rotation.x = 310.0f;
}

void DemoController::printUsage() const
{
	Log::Debug("Welcome to Kike's Software Renderer!");
	Log::Debug("Usage:");
	Log::Debug("\t-Change the model with keys 1-5");
	Log::Debug("\t-Move the model with the Up/Down/Left/Right keys");
	Log::Debug("\t-Move closer with PageDown, Move away with PageUp");
	Log::Debug("\t-Rotate model by left clicking and moving the mouse");
	Log::Debug("\t-Scale model by right clicking and moving the mouse Up / Down");
	Log::Debug("\t-'P' => Switch projection (Orthographic / Perspective)");
	Log::Debug("\t-'W' => Enable/Disable Wireframe Mode");
	Log::Debug("\t-'B' => Enable/Disable Backface Culling");
	Log::Debug("\t-'I' => Enable/Disable MD2 animation interpolation");
	Log::Debug("\t-'R' => Switch Render Mode (CheckPixelInTriangle / ScanLines)");
}

void DemoController::updateCyberDemon(float dt)
{
	if (!m_playingAnimations)
	{
		return;
	}
	
	const float timeFullAnim = 2.0f;
	const int numberOfFrames = 8;
	const int firstFrameIndex = 2;
	const int lastFrameIndex = firstFrameIndex + numberOfFrames - 1;
	m_timeInCurrentAnim += dt;
	while (m_timeInCurrentAnim > timeFullAnim)
	{
		m_timeInCurrentAnim -= timeFullAnim;
	}
	const float currentCyberdemonFrameFloat = firstFrameIndex + (m_timeInCurrentAnim / timeFullAnim) * numberOfFrames;
	const int currentFrameIndex = (int)currentCyberdemonFrameFloat;
	const float percentCurrentFrame = currentCyberdemonFrameFloat - currentFrameIndex;
	const int nextFrameIndex = (currentFrameIndex < lastFrameIndex) ? currentFrameIndex + 1 : firstFrameIndex;
	
	m_cyberDemonVertices.clear();
	m_cyberDemonVertices.reserve(m_cyberDemonModel->m_header.m_numTriangles * 3);
	
	const MD2Frame& currentFrame = m_cyberDemonModel->m_frames[currentFrameIndex];
	const MD2Frame& nextFrame = m_cyberDemonModel->m_frames[nextFrameIndex];
	for (int i = 0; i < m_cyberDemonModel->m_header.m_numTriangles; ++i)
	{
		const MD2Triangle& triangle = m_cyberDemonModel->m_triangles[i];
		for (int j = 0; j < 3; ++j)
		{
			const MD2Vertex& curMd2Vertex = currentFrame.m_vertices[triangle.m_vertexIdxs[j]];
			const MD2Vertex& nextMd2Vertex = nextFrame.m_vertices[triangle.m_vertexIdxs[j]];
			
			Vector4f curFramePos;
			curFramePos.x = (curMd2Vertex.m_position[0] * currentFrame.m_scale.x) + currentFrame.m_translate.x;
			curFramePos.y = (curMd2Vertex.m_position[1] * currentFrame.m_scale.y) + currentFrame.m_translate.y;
			curFramePos.z = (curMd2Vertex.m_position[2] * currentFrame.m_scale.z) + currentFrame.m_translate.z;
			curFramePos.w = 1.0f;
			
			Vector4f pos;
			if (m_interpolateAnimations)
			{
				Vector4f nextFramePos;
				nextFramePos.x = (nextMd2Vertex.m_position[0] * nextFrame.m_scale.x) + nextFrame.m_translate.x;
				nextFramePos.y = (nextMd2Vertex.m_position[1] * nextFrame.m_scale.y) + nextFrame.m_translate.y;
				nextFramePos.z = (nextMd2Vertex.m_position[2] * nextFrame.m_scale.z) + nextFrame.m_translate.z;
				nextFramePos.w = 1.0f;
				
				pos = curFramePos.lerp(nextFramePos, percentCurrentFrame);
			}
			else
			{
				pos = curFramePos;
			}
			
			Vector2f texCoord;
			texCoord.x = (float)m_cyberDemonModel->m_texCoords[triangle.m_texCoordIdxs[j]].m_s / m_cyberDemonModel->m_header.m_textureWidth;
			texCoord.y = (float)m_cyberDemonModel->m_texCoords[triangle.m_texCoordIdxs[j]].m_t / m_cyberDemonModel->m_header.m_textureHeight;
			
			m_cyberDemonVertices.push_back(Vertex(pos, texCoord));
		}
	}
}

void DemoController::drawTriangle()
{
	m_renderer.drawTriangleStrip(m_TriangleVertices, m_TriangleTransform.calculateFinalTransform());
}

void DemoController::drawAvatar()
{
	m_renderer.bindTexture(m_avatarTexId);
	m_renderer.drawTriangles(m_avatarVertices, m_avatarTransform.calculateFinalTransform());
	m_renderer.unbindTexture();
}

void DemoController::drawChessPattern()
{
	m_renderer.bindTexture(m_chessPatternTexId);
	m_renderer.drawTriangles(m_avatarVertices, m_chessPatternTransform.calculateFinalTransform());
	m_renderer.unbindTexture();
}

void DemoController::drawCube()
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
	
	m_renderer.bindTexture(m_avatarTexId);
	m_renderer.drawTriangles(vertices, m_cubeTransform.calculateFinalTransform());
	m_renderer.unbindTexture();
	
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
	
	m_renderer.drawTriangles(vertices, m_cubeTransform.calculateFinalTransform());
}

void DemoController::drawCyberDemon()
{
	m_renderer.bindTexture(m_cyberDemonTexId);
	m_renderer.drawTriangles(m_cyberDemonVertices, m_cyberDemonTransform.calculateFinalTransform());
	m_renderer.unbindTexture();
}

void DemoController::switchProjectionMode()
{
	m_perspectiveProjection = !m_perspectiveProjection;
	if (m_perspectiveProjection)
	{
		m_renderer.setViewProjectionMatrix(MathUtils::CreatePerspectiveMatrix(kFrustumScale, kZNear, kZFar));
		Log::Debug("Using perspective projection");
	}
	else
	{
		m_renderer.setViewProjectionMatrix(MathUtils::CreateOrthographicMatrix(-1.0f, 1.0f, -1.0f, 1.0f, kZNear, kZFar));
		Log::Debug("Using orthogonal projection");
	}
}