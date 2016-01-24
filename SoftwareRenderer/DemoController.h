//
//  DemoController.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 17/01/16.
//  Copyright (c) 2016 Enrique Alcor Martín. All rights reserved.
//

#pragma once

#include "SoftwareRenderer.h"

#include "Transform.h"
#include "Vertex.h"

#include "stdafx.h"

namespace omb
{
	struct MD2Model;
}

class DemoController
{
public:
	DemoController();
	
	void init();
	void end();
	void update(float dt);
	void draw();
	
private:
	enum class SelectedModel
	{
		Cyberdemon,
		Avatar,
		ChessPattern,
		Triangle,
		Cube
	};
	
	// Constants
	const int kFrameBufferHeight = 512;
	const int kFrameBufferWidth = 512;
	const float kFrustumScale = 1.0f;
	const float kZNear = 0.1f;
	const float kZFar = 5.0f;
	
	GLuint m_openGLTexId;
	omb::SoftwareRenderer m_renderer;
	SelectedModel m_selectedModel = SelectedModel::Cyberdemon;
	omb::Transform* m_selectedTransform = &m_cyberDemonTransform;
	bool m_perspectiveProjection;
	float m_timeInCurrentAnim = 0;
	bool m_interpolateAnimations = true;
	
	// Input data
	bool m_switchKeyPressedLastFrame = false;
	omb::Vector2i m_lastMousePos;
	
	// Models data
	omb::Transform m_TriangleTransform;
	omb::Transform m_avatarTransform;
	omb::Transform m_chessPatternTransform;
	omb::Transform m_cyberDemonTransform;
	omb::Transform m_cubeTransform;
	
	uint16_t m_avatarTexId;
	uint16_t m_chessPatternTexId;
	uint16_t m_cyberDemonTexId;
	
	std::vector<omb::Vertex> m_TriangleVertices;
	std::vector<omb::Vertex> m_avatarVertices;
	std::vector<omb::Vertex> m_cyberDemonVertices;
	
	omb::MD2Model* m_cyberDemonModel;
	
	// Internal functions
	void initOpenGL();
	void initRenderer();
	void initModels();
	
	void updateInput(float dt);
	void updateInputSwitches(float dt);
	void updateInputMove(float dt);
	void updateInputRotation(float dt);
	void updateInputScale(float dt);
	void updateCyberDemon(float dt);
	
	void switchProjectionMode();
	
	void drawAvatar();
	void drawChessPattern();
	void drawTriangle();
	void drawCube();
	void drawCyberDemon();
};
