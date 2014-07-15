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
		
		m_size.x = width;
		m_size.y = height;
		m_halfSize = Vector2i(m_size.x/2, m_size.y/2);
		m_initialised = true;
	}
	
	void SoftwareRenderer::clear(const Color& clearColor)
	{
		OMBAssert(m_initialised, "Uninitialized!");
	
		for (int i = 0; i < m_size.x * m_size.y * kBytesPerPixel; ++i)
		{
			if (i % kBytesPerPixel == 0)
			{
				m_frameBuffer[i] = clearColor.r;
			}
			else if (i % kBytesPerPixel == 1)
			{
				m_frameBuffer[i] = clearColor.g;
			}
			else if (i % kBytesPerPixel == 2)
			{
				m_frameBuffer[i] = clearColor.b;
			}
			else if (i % kBytesPerPixel == 3)
			{
				m_frameBuffer[i] = clearColor.a;
			}
		}
	}
	
	void SoftwareRenderer::drawPoints(const std::vector<Vertex>& vertices)
	{
		OMBAssert(m_initialised, "Uninitialized!");
		
		for (int i = 0; i < vertices.size(); ++i)
		{
			const Vertex& vertex = vertices[i];
			const Vector2i fbPos = ndcCoordToFBCoord((Vector2f)vertex.m_pos);
			
			setPixelColor(fbPos, vertex.m_color);
		}
	}
	
	void SoftwareRenderer::drawTriangleStrip(const std::vector<Vertex>& vertices)
	{
		OMBAssert(m_initialised, "Uninitialized!");
		
		for (int i = 0; i < vertices.size() - 2; ++i)
		{
			const Vertex& a = vertices[i + 0];
			const Vertex& b = vertices[i + 1];
			const Vertex& c = vertices[i + 2];
			
			const Vector2f aFB = ndcCoordToFBCoord((Vector2f)a.m_pos);
			const Vector2f bFB = ndcCoordToFBCoord((Vector2f)b.m_pos);
			const Vector2f cFB = ndcCoordToFBCoord((Vector2f)c.m_pos);
			
			drawLine(aFB, bFB, a.m_color);
			drawLine(bFB, cFB, b.m_color);
			drawLine(cFB, aFB, c.m_color);
		}
	}
	
	Vector2f SoftwareRenderer::ndcCoordToFBCoord(const Vector2f& ndcCoord)
	{
		Vector2f fbCoord;
		fbCoord.x = m_halfSize.x + ndcCoord.x * m_halfSize.x;
		fbCoord.y = m_halfSize.y + ndcCoord.y * m_halfSize.y;
		
		// Refactor (round)
		if (fbCoord.x - (int)fbCoord.x >= 0.5f)
		{
			fbCoord.x = (int)fbCoord.x + 1;
		}
		
		if (fbCoord.y - (int)fbCoord.y >= 0.5f)
		{
			fbCoord.y = (int)fbCoord.y + 1;
		}
		
		return fbCoord;
	}
	
	void SoftwareRenderer::setPixelColor(const Vector2i& pos, const Color& color)
	{
		OMBAssert(pos.x >= 0 && pos.x < m_size.x, "x (%d) is out of bounds!", pos.x);
		OMBAssert(pos.y >= 0 && pos.x < m_size.y, "y (%d) is out of bounds!", pos.y);
		
		m_frameBuffer[pos.y * (m_size.x * kBytesPerPixel) + (pos.x * kBytesPerPixel) + 0] = color.r;
		m_frameBuffer[pos.y * (m_size.x * kBytesPerPixel) + (pos.x * kBytesPerPixel) + 1] = color.g;
		m_frameBuffer[pos.y * (m_size.x * kBytesPerPixel) + (pos.x * kBytesPerPixel) + 2] = color.b;
		m_frameBuffer[pos.y * (m_size.x * kBytesPerPixel) + (pos.x * kBytesPerPixel) + 3] = color.a;
	}
	
	void SoftwareRenderer::drawLine(const Vector2f& aFB, const Vector2f& bFB, const Color& color)
	{
		Vector2f aToB = bFB - aFB;
		const double aToBLength = aToB.getLength();
		aToB.normalize();
		
		for (int i = 0; i <= aToBLength; ++i)
		{
			Vector2i fragment;
			fragment.x = aFB.x + round(aToB.x * i);
			fragment.y = aFB.y + round(aToB.y * i);
			
			setPixelColor(fragment, color);
		}
	}

}