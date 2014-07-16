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
	namespace
	{
		float sign(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3)
		{
			return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
		}
		
		// From: http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
		// For a more efficient implementation, check Andreas Brinck's answer
		bool isPointInTriangle(const Vector2f& point, const Vector2f& trV1, const Vector2f& trV2, const Vector2f& trV3)
		{
			bool b1, b2, b3;
			
			b1 = sign(point, trV1, trV2) < 0.0f;
			b2 = sign(point, trV2, trV3) < 0.0f;
			b3 = sign(point, trV3, trV1) < 0.0f;
			
			return ((b1 == b2) && (b2 == b3));
		}
		
		float getSmallestValue(const float values[], const int size)
		{
			float smallest = std::numeric_limits<float>::max();
			for (int i = 0; i < size; ++i)
			{
				if (values[i] < smallest)
				{
					smallest = values[i];
				}
			}
			
			return smallest;
		}
		
		float getBiggestValue(const float values[], const int size)
		{
			float biggest = std::numeric_limits<float>::min();
			for (int i = 0; i < size; ++i)
			{
				if (values[i] > biggest)
				{
					biggest = values[i];
				}
			}
			
			return biggest;
		}
	}
	
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
			
			// In order to avoid checking all the points in the surface, we just check those inside the imaginary bounding box surrounding the triangle
			const float xValues[] = {aFB.x, bFB.x, cFB.x};
			const float yValues[] = {aFB.y, bFB.y, cFB.y};
			
			const float leftMostX = getSmallestValue(xValues, sizeofarray(xValues));
			const float rightMostX = getBiggestValue(xValues, sizeofarray(xValues));
			const float upperMostY = getBiggestValue(yValues, sizeofarray(yValues));
			const float lowerMostY = getSmallestValue(yValues, sizeofarray(yValues));
			
			for (int i = leftMostX; i <= rightMostX; ++i)
			{
				for (int j = upperMostY; j >= lowerMostY; --j)
				{
					Vector2f candidate(i, j);
					if (isPointInTriangle(candidate, aFB, bFB, cFB))
					{
						// Barycentric color interpolation. For more info: http://classes.soe.ucsc.edu/cmps160/Fall10/resources/barycentricInterpolation.pdf
						const float totalArea = fabsf((aFB.x*(bFB.y - cFB.y) + bFB.x*(cFB.y - aFB.y) + cFB.x*(aFB.y - bFB.y))/2);
						const float pabArea = fabsf((candidate.x*(aFB.y - bFB.y) + aFB.x*(bFB.y - candidate.y) + bFB.x*(candidate.y - aFB.y))/2);
						const float pacArea = fabsf((candidate.x*(aFB.y - cFB.y) + aFB.x*(cFB.y - candidate.y) + cFB.x*(candidate.y - aFB.y))/2);
						const float pbcArea = fabsf((candidate.x*(bFB.y - cFB.y) + bFB.x*(cFB.y - candidate.y) + cFB.x*(candidate.y - bFB.y))/2);
						
						const Color color(pbcArea/totalArea * a.m_color.r + pacArea/totalArea * b.m_color.r + pabArea/totalArea * c.m_color.r,
										  pbcArea/totalArea * a.m_color.g + pacArea/totalArea * b.m_color.g + pabArea/totalArea * c.m_color.g,
										  pbcArea/totalArea * a.m_color.b + pacArea/totalArea * b.m_color.b + pabArea/totalArea * c.m_color.b,
										  255);
						setPixelColor(candidate, color);
					}
				}
			}
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