//
//  SoftwareRenderer.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 06/07/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "SoftwareRenderer.h"

#include "Utils.h"

#include <cmath>

namespace omb
{

	SoftwareRenderer::SoftwareRenderer()
	{
	}
	
	SoftwareRenderer::~SoftwareRenderer()
	{
	}
	
	void SoftwareRenderer::init(const int width, const int height)
	{
		OMBAssert(width <= kMaxWidth, "Width can't be bigger than %d", kMaxWidth);
		OMBAssert(height <= kMaxHeight, "Height can't be bigger than %d", kMaxHeight);
		
		m_width = width;
		m_height = height;
		m_initialised = true;
	}
	
	void SoftwareRenderer::clear(const Color& clearColor)
	{
		OMBAssert(m_initialised, "Uninitialized!");
	
		for (int i = 0; i < m_width * m_height * kBytesPerPixel; ++i)
		{
			if (i % kBytesPerPixel == 0)
			{
				m_frameBuffer[i] = clearColor.m_r;
			}
			else if (i % kBytesPerPixel == 1)
			{
				m_frameBuffer[i] = clearColor.m_g;
			}
			else if (i % kBytesPerPixel == 2)
			{
				m_frameBuffer[i] = clearColor.m_b;
			}
			else if (i % kBytesPerPixel == 3)
			{
				m_frameBuffer[i] = clearColor.m_a;
			}
		}
	}
	
	void SoftwareRenderer::drawPoints(const std::vector<Vertex>& vertices)
	{
		OMBAssert(m_initialised, "Uninitialized!");
		
		const Vector2 surfaceHalfSize = Vector2(m_width/2, m_height/2);
		
		for (int i = 0; i < vertices.size(); ++i)
		{
			const Vertex& vertex = vertices[i];
			
			const Vector2 surfaceVertexPos = surfaceHalfSize + (Vector2)vertex.m_pos * surfaceHalfSize;
			const int x = surfaceVertexPos.m_x;
			const int y = surfaceVertexPos.m_y;
			
			OMBAssert(x >= 0 && x < m_width, "x (%d) is out of bounds!", x);
			OMBAssert(y >= 0 && x < m_height, "y (%d) is out of bounds!", y);
			
			m_frameBuffer[y * (m_width * kBytesPerPixel) + (x * kBytesPerPixel) + 0] = vertex.m_color.m_r;
			m_frameBuffer[y * (m_width * kBytesPerPixel) + (x * kBytesPerPixel) + 1] = vertex.m_color.m_g;
			m_frameBuffer[y * (m_width * kBytesPerPixel) + (x * kBytesPerPixel) + 2] = vertex.m_color.m_b;
			m_frameBuffer[y * (m_width * kBytesPerPixel) + (x * kBytesPerPixel) + 3] = vertex.m_color.m_a;
		}
	}
	
	void SoftwareRenderer::drawTriangleStrip(const std::vector<Vertex>& vertices)
	{
		OMBAssert(m_initialised, "Uninitialized!");
		
		OMBAssert(false, "Unimplemented!");
	}

}