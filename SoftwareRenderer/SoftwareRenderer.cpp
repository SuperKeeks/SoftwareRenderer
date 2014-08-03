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
		float sign(const Vector2i& p1, const Vector2i& p2, const Vector2i& p3)
		{
			return (double)(p1.x - p3.x) * (p2.y - p3.y) - (double)(p2.x - p3.x) * (p1.y - p3.y);
		}
		
		// From: http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
		// For a more efficient implementation, check Andreas Brinck's answer
		bool isPointInTriangle(const Vector2i& point, const Vector2i& trV1, const Vector2i& trV2, const Vector2i& trV3)
		{
			bool b1, b2, b3;
			
			b1 = sign(point, trV1, trV2) < 0.0f;
			b2 = sign(point, trV2, trV3) < 0.0f;
			b3 = sign(point, trV3, trV1) < 0.0f;
			
			return ((b1 == b2) && (b2 == b3));
		}
		
		int getSmallestValue(const int values[], const int size)
		{
			int smallest = std::numeric_limits<int>::max();
			for (int i = 0; i < size; ++i)
			{
				if (values[i] < smallest)
				{
					smallest = values[i];
				}
			}
			
			return smallest;
		}
		
		int getBiggestValue(const int values[], const int size)
		{
			int biggest = std::numeric_limits<int>::min();
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
		m_halfSize = Vector2i((m_size.x-1)/2, (m_size.y-1)/2);
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
			const Vertex& b = vertices[i + ((i % 2 == 0) ? 2 : 1)];
			const Vertex& c = vertices[i + ((i % 2 == 0) ? 1 : 2)];
			
			//drawTriangleSlow(a, b, c);
			drawTriangleFaster(a, b, c);
		}
	}
	
	Vector2i SoftwareRenderer::ndcCoordToFBCoord(const Vector2f& ndcCoord)
	{
		Vector2i fbCoord;
		fbCoord.x = round(m_halfSize.x + ndcCoord.x * m_halfSize.x);
		fbCoord.y = round(m_halfSize.y + ndcCoord.y * m_halfSize.y);
		
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
	
	// This is not the function I'm using to draw lines, but can be interesting to implement at some point:
	// http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
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
	
	void SoftwareRenderer::drawSubTriangle(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		OMBAssert(a.m_pos.y == b.m_pos.y ||
				  a.m_pos.y == c.m_pos.y ||
				  b.m_pos.y == c.m_pos.y, "Condition of this function: 2 vertices need to share same y");
		
		const Vector2i aFB = ndcCoordToFBCoord((Vector2f)a.m_pos);
		const Vector2i bFB = ndcCoordToFBCoord((Vector2f)b.m_pos);
		const Vector2i cFB = ndcCoordToFBCoord((Vector2f)c.m_pos);
		
		const int yValues[] = {aFB.y, bFB.y, cFB.y};
		const int upperMostY = getBiggestValue(yValues, sizeofarray(yValues));
		const int lowerMostY = getSmallestValue(yValues, sizeofarray(yValues));
		
		const Vector2i* peak = nullptr;
		const Vector2i* baseA = nullptr;
		const Vector2i* baseB = nullptr;
		
		if (aFB.y == bFB.y)
		{
			peak = &cFB;
			baseA = &aFB;
			baseB = &bFB;
		}
		else if (aFB.y == cFB.y)
		{
			peak = &bFB;
			baseA = &aFB;
			baseB = &cFB;
		}
		else
		{
			peak = &aFB;
			baseA = &bFB;
			baseB = &cFB;
		}
		
		if (baseB->x < baseA->x)
		{
			const Vector2i* temp = baseA;
			baseA = baseB;
			baseB = temp;
		}
		
		const float slopePBA = (float)(baseA->x - peak->x) / (baseA->y - peak->y);
		const float slopePBB = (float)(baseB->x - peak->x) / (baseB->y - peak->y);
		const float factorPBA = - (slopePBA * peak->y) + peak->x;
		const float factorPBB = - (slopePBB * peak->y) + peak->x;
		
		for (int y = upperMostY; y >= lowerMostY; --y)
		{
			const int minX = (slopePBA * y) + factorPBA;
			const int maxX = (slopePBB * y) + factorPBB;
			
			for (int x = minX; x <= maxX; ++x)
			{
				setPixelColor(Vector2i(x, y), a.m_color);
			}
		}
	}
	
	void SoftwareRenderer::drawTriangleFaster(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		drawSubTriangle(a, b, c);
	}
	
	void SoftwareRenderer::drawTriangleSlow(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		const Vector2i aFB = ndcCoordToFBCoord((Vector2f)a.m_pos);
		const Vector2i bFB = ndcCoordToFBCoord((Vector2f)b.m_pos);
		const Vector2i cFB = ndcCoordToFBCoord((Vector2f)c.m_pos);
		
		// In order to avoid checking all the points in the surface, we just check those inside the imaginary bounding box surrounding the triangle
		const int xValues[] = {aFB.x, bFB.x, cFB.x};
		const int yValues[] = {aFB.y, bFB.y, cFB.y};
		
		const int leftMostX = getSmallestValue(xValues, sizeofarray(xValues));
		const int rightMostX = getBiggestValue(xValues, sizeofarray(xValues));
		const int upperMostY = getBiggestValue(yValues, sizeofarray(yValues));
		const int lowerMostY = getSmallestValue(yValues, sizeofarray(yValues));
		
		const float totalArea = fabsf((aFB.x*(bFB.y - cFB.y) + bFB.x*(cFB.y - aFB.y) + cFB.x*(aFB.y - bFB.y))/2);
		
		for (int j = leftMostX; j <= rightMostX; ++j)
		{
			for (int k = upperMostY; k >= lowerMostY; --k)
			{
				Vector2f candidate(j, k);
				if (isPointInTriangle(candidate, aFB, bFB, cFB))
				{
					// Barycentric color interpolation. For more info: http://classes.soe.ucsc.edu/cmps160/Fall10/resources/barycentricInterpolation.pdf
					const float pabArea = fabsf((candidate.x*(aFB.y - bFB.y) + aFB.x*(bFB.y - candidate.y) + bFB.x*(candidate.y - aFB.y))/2);
					const float pacArea = fabsf((candidate.x*(aFB.y - cFB.y) + aFB.x*(cFB.y - candidate.y) + cFB.x*(candidate.y - aFB.y))/2);
					const float pbcArea = fabsf((candidate.x*(bFB.y - cFB.y) + bFB.x*(cFB.y - candidate.y) + cFB.x*(candidate.y - bFB.y))/2);
					
					const Color color(pbcArea/totalArea * a.m_color.r + pacArea/totalArea * b.m_color.r + pabArea/totalArea * c.m_color.r,
									  pbcArea/totalArea * a.m_color.g + pacArea/totalArea * b.m_color.g + pabArea/totalArea * c.m_color.g,
									  pbcArea/totalArea * a.m_color.b + pacArea/totalArea * b.m_color.b + pabArea/totalArea * c.m_color.b,
									  pbcArea/totalArea * a.m_color.a + pacArea/totalArea * b.m_color.a + pabArea/totalArea * c.m_color.a);
					//const Color color(255, 0, 0, 255);
					setPixelColor(candidate, color);
				}
			}
		}
	}

}