//
//  SoftwareRenderer.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 06/07/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "SoftwareRenderer.h"

#include "Assert.h"
#include "Color.h"
#include "SizeOfArray.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vertex.h"

#define STB_IMAGE_IMPLEMENTATION
#include "externals/stb_image.h"

#include <cmath>

#define USE_SLOW_TRIANGLE_METHOD 1

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
		
		b1 = sign(point, trV1, trV2) <= 0.0f;
		b2 = sign(point, trV2, trV3) <= 0.0f;
		b3 = sign(point, trV3, trV1) <= 0.0f;
		
		return ((b1 == b2) && (b2 == b3));
	}
	
	template <typename T>
	T getSmallestValue(const T values[], const size_t size)
	{
		T smallest = values[0];
		for (int i = 1; i < size; ++i)
		{
			if (values[i] < smallest)
			{
				smallest = values[i];
			}
		}
		
		return smallest;
	}
	
	template <typename T>
	T getBiggestValue(const T values[], const size_t size)
	{
		T biggest = values[0];
		for (int i = 1; i < size; ++i)
		{
			if (values[i] > biggest)
			{
				biggest = values[i];
			}
		}
		
		return biggest;
	}
	
	int clamp(float value, int min, int max)
	{
		int result = value;
		result = std::max(result, min);
		result = std::min(result, max);
		
		return result;
	}
	
	struct InterpolateResult
	{
		Color m_color;
		Vector2f m_texCoord;
		float m_z;
		
		InterpolateResult()
		: m_color(0, 0, 0, 0)
		, m_z(0)
		{
		}
	};
	
	InterpolateResult interpolate(const Vector2f& point, const Vertex& a, const Vertex& b, const Vertex& c, const TextureInfo& texInfo)
	{
		InterpolateResult res;
		
		// Barycentric color interpolation. For more info: http://classes.soe.ucsc.edu/cmps160/Fall10/resources/barycentricInterpolation.pdf
		const float totalArea = fabsf((a.m_pos.x*(b.m_pos.y - c.m_pos.y) + b.m_pos.x*(c.m_pos.y - a.m_pos.y) + c.m_pos.x*(a.m_pos.y - b.m_pos.y))/2);
		const float pabArea = fabsf((point.x*(a.m_pos.y - b.m_pos.y) + a.m_pos.x*(b.m_pos.y - point.y) + b.m_pos.x*(point.y - a.m_pos.y))/2);
		const float pacArea = fabsf((point.x*(a.m_pos.y - c.m_pos.y) + a.m_pos.x*(c.m_pos.y - point.y) + c.m_pos.x*(point.y - a.m_pos.y))/2);
		const float pbcArea = fabsf((point.x*(b.m_pos.y - c.m_pos.y) + b.m_pos.x*(c.m_pos.y - point.y) + c.m_pos.x*(point.y - b.m_pos.y))/2);
		
		if (texInfo.m_data)
		{
			res.m_texCoord.x = pbcArea/totalArea * a.m_texCoord.x + pacArea/totalArea * b.m_texCoord.x + pabArea/totalArea * c.m_texCoord.x;
			res.m_texCoord.y = pbcArea/totalArea * a.m_texCoord.y + pacArea/totalArea * b.m_texCoord.y + pabArea/totalArea * c.m_texCoord.y;
			
			const int xCoordAbs = clamp((texInfo.m_size.x - 1) * res.m_texCoord.x, 0, texInfo.m_size.x - 1);
			const int yCoordAbs = clamp((texInfo.m_size.y - 1) * res.m_texCoord.y, 0, texInfo.m_size.y - 1);
			
			const int baseIndex = yCoordAbs * texInfo.m_size.x * texInfo.m_bytesPerPixel + xCoordAbs * texInfo.m_bytesPerPixel;
			res.m_color.r = texInfo.m_data[baseIndex + 0];
			res.m_color.g = texInfo.m_data[baseIndex + 1];
			res.m_color.b = texInfo.m_data[baseIndex + 2];
			res.m_color.a = texInfo.m_data[baseIndex + 3];
		}
		else
		{
			res.m_color.r = pbcArea/totalArea * a.m_color.r + pacArea/totalArea * b.m_color.r + pabArea/totalArea * c.m_color.r;
			res.m_color.g = pbcArea/totalArea * a.m_color.g + pacArea/totalArea * b.m_color.g + pabArea/totalArea * c.m_color.g;
			res.m_color.b = pbcArea/totalArea * a.m_color.b + pacArea/totalArea * b.m_color.b + pabArea/totalArea * c.m_color.b;
			res.m_color.a = pbcArea/totalArea * a.m_color.a + pacArea/totalArea * b.m_color.a + pabArea/totalArea * c.m_color.a;
		}
		
		res.m_z = pbcArea/totalArea * a.m_pos.z + pacArea/totalArea * b.m_pos.z + pabArea/totalArea * c.m_pos.z;
		
		return res;
	}
	
	bool isBackFace(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		const Vector4f& posA = a.m_pos;
		const Vector4f& posB = b.m_pos;
		const Vector4f& posC = c.m_pos;
		
		// Omit back faces based on triangle winding based on the sign of the determinant of vectors
		// For more info check: http://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
		const float position = (posB.x - posA.x) * (posC.y - posA.y) - (posB.y - posA.y) * (posC.x - posA.x);
		// CW (Clock Wise) winding
#if 1
		if (position > 0)
		{
			return true;
		}
		// CCW (Counter Clock Wise) winding
#else
		if (position < 0)
		{
			return true;
		}
#endif
		
		return false;
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

	for (int i = 0; i < m_size.x * m_size.y; ++i)
	{
		const int baseIndex = i * kBytesPerPixel;
		m_frameBuffer[baseIndex + 0] = clearColor.r;
		m_frameBuffer[baseIndex + 1] = clearColor.g;
		m_frameBuffer[baseIndex + 2] = clearColor.b;
		m_frameBuffer[baseIndex + 3] = clearColor.a;
		
		m_zBuffer[i] = std::numeric_limits<float>::max();
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

void SoftwareRenderer::drawTriangles(const std::vector<Vertex>& vertices)
{
	OMBAssert(m_initialised, "Uninitialized!");
	OMBAssert(vertices.size() % 3 == 0, "Total number of vertices is not a multiple of 3!");
	
	for (int i = 0; i < vertices.size() - 2; i += 3)
	{
		const Vertex& a = vertices[i + 0];
		const Vertex& b = vertices[i + 1];
		const Vertex& c = vertices[i + 2];
		
		if (!isBackFace(a, b, c))
		{
			Vertex divA = a;
			Vertex divB = b;
			Vertex divC = c;
			
			divA.m_pos = divA.m_pos / divA.m_pos.w;
			divB.m_pos = divB.m_pos / divB.m_pos.w;
			divC.m_pos = divC.m_pos / divC.m_pos.w;
			
#if USE_SLOW_TRIANGLE_METHOD
			drawTriangleSlow(divA, divB, divC);
#else
			drawTriangleFaster(divA, divB, divC);
#endif
		}
	}
}
	
int16_t SoftwareRenderer::loadTexture(const char* fileName)
{
	int16_t textureId = SoftwareRendererConsts::kInvalidTextureId;
	
	for (int i = 0; i < kMaxTextures; ++i)
	{
		if (!m_textures[i].m_data)
		{
			int defaultComp;
			TextureInfo& textureInfo = m_textures[i];
			textureId = i;
			textureInfo.m_data = stbi_load(fileName, &textureInfo.m_size.x, &textureInfo.m_size.y, &defaultComp, kBytesPerPixel);
			textureInfo.m_bytesPerPixel = kBytesPerPixel;
			
			OMBAssert(textureInfo.m_data, "Can't load texture (%s)", stbi_failure_reason());
			
			break;
		}
	}
	
	OMBAssert(textureId != SoftwareRendererConsts::kInvalidTextureId, "Can't load texture (too many textures loaded)");
	
	return textureId;
}

void SoftwareRenderer::unloadTexture(int16_t textureId)
{
	OMBAssert(textureId >= 0 && textureId < kMaxTextures, "Invalid textureId!");
	
	TextureInfo& textureInfo = m_textures[textureId];
	stbi_image_free(textureInfo.m_data);
	textureInfo.m_data = nullptr;
	textureInfo.m_bytesPerPixel = 0;
	textureInfo.m_size.x = 0;
	textureInfo.m_size.y = 0;
}

void SoftwareRenderer::bindTexture(int16_t textureId)
{
	if (textureId >= 0 && textureId < kMaxTextures)
	{
		if (m_textures[textureId].m_data)
		{
			m_bindedTextureId = textureId;
		}
		else
		{
			OMBAssert(false, "Texture has no data!");
		}
	}
	else
	{
		OMBAssert(false, "Invalid textureId!");
	}
}

void SoftwareRenderer::unbindTexture()
{
	m_bindedTextureId = SoftwareRendererConsts::kInvalidTextureId;
}

void SoftwareRenderer::drawTriangleStrip(const std::vector<Vertex>& vertices)
{
	OMBAssert(m_initialised, "Uninitialized!");
	
	for (int i = 0; i < vertices.size() - 2; ++i)
	{
		const Vertex& a = vertices[i + 0];
		const Vertex& b = vertices[i + ((i % 2 == 0) ? 1 : 2)];
		const Vertex& c = vertices[i + ((i % 2 == 0) ? 2 : 1)];
		
		if (!isBackFace(a, b, c))
		{
			Vertex divA = a;
			Vertex divB = b;
			Vertex divC = c;
			
			divA.m_pos = divA.m_pos / divA.m_pos.w;
			divB.m_pos = divB.m_pos / divB.m_pos.w;
			divC.m_pos = divC.m_pos / divC.m_pos.w;
			
#if USE_SLOW_TRIANGLE_METHOD
			drawTriangleSlow(divA, divB, divC);
#else
			drawTriangleFaster(divA, divB, divC);
#endif
		}
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

void SoftwareRenderer::setPixelZ(const Vector2i& pos, const float z)
{
	m_zBuffer[pos.y * m_size.x + pos.x] = z;
}

float SoftwareRenderer::getPixelZ(const Vector2i& pos)
{
	return m_zBuffer[pos.y * m_size.x + pos.x];
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
	/*
		This function does the following to paint a triangle:
			-Finds which vertex (or vertices) is on the top
			-Finds which vertex (or vertices) is on the bottom
			-It draws the triangle line by line from top to bottom (Y coord)
				 -For each line, the left and right limits are calculated
				 -Then it fills the pixels that are between those limits
	*/

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
	
	double slopePBA;
	double slopePBB;
	double factorPBA;
	double factorPBB;
	
	// Vertical lines have no slope, so we do a trick to avoid a division by 0
	if (baseA->y - peak->y != 0)
	{
		slopePBA = (float)(baseA->x - peak->x) / (baseA->y - peak->y);
		factorPBA = - (slopePBA * peak->y) + peak->x;
	}
	else
	{
		slopePBA = 0;
		factorPBA = std::min(baseA->x, peak->x);
	}
	
	if (baseB->y - peak->y != 0)
	{
		slopePBB = (float)(baseB->x - peak->x) / (baseB->y - peak->y);
		factorPBB = - (slopePBB * peak->y) + peak->x;
	}
	else
	{
		slopePBB = 0;
		factorPBB = std::min(baseB->x, peak->x);
	}
	
	const Vertex aFBAsVertex(Vector4f(aFB.x, aFB.y, a.m_pos.z), a.m_color, a.m_texCoord);
	const Vertex bFBAsVertex(Vector4f(bFB.x, bFB.y, b.m_pos.z), b.m_color, b.m_texCoord);
	const Vertex cFBAsVertex(Vector4f(cFB.x, cFB.y, c.m_pos.z), c.m_color, c.m_texCoord);
	
	for (int y = upperMostY; y >= lowerMostY; --y)
	{
		const int minX = ceil((slopePBA * y) + factorPBA);
		const int maxX = floor((slopePBB * y) + factorPBB);
		
		for (int x = minX; x <= maxX; ++x)
		{
			const Vector2i position(x, y);
			TextureInfo texInfo;
			if (m_bindedTextureId != SoftwareRendererConsts::kInvalidTextureId)
			{
				texInfo = m_textures[m_bindedTextureId];
			}
			const InterpolateResult intRes = interpolate(Vector2f(position.x, position.y), aFBAsVertex, bFBAsVertex, cFBAsVertex, texInfo);
			
			if (intRes.m_z < getPixelZ(position))
			{
				setPixelColor(position, intRes.m_color);
				setPixelZ(position, intRes.m_z);
			}
		}
	}
}

void SoftwareRenderer::drawTriangleFaster(const Vertex& a, const Vertex& b, const Vertex& c)
{
	if (a.m_pos.y == b.m_pos.y ||
		a.m_pos.y == c.m_pos.y ||
		b.m_pos.y == c.m_pos.y)
	{
		// The triangle has a flat surface on top or bottom
		/*                        ______
					/\            \    /
				   /  \     or     \  /
				  /____\            \/
		*/
		
		drawSubTriangle(a, b, c);
	}
	else
	{
		/* If it doesn't we'll need to split it into 2 smaller triangles.
		   To do that, we take the vertex that is in the middle (in Y coordinate) and project it onto the opposite side
		   (the line formed by the union of the other 2 vertices). We call the projection "auxPoint"
		   Once that projected point is calculated, we can use it as a 4th vertex to draw 2 triangles with flat bottom and flat top respectively
		   
					  A                   A
					 /|                  /|
					/ |                 / |
				   /  |		           /  |                   A          B ______ auxPoint
				  /   |               /   |                  /|		       \    |
			   B /    |      =>    B /____| auxPoint =>     / |      +      \   |
				 \    |		         \    |		           /  |              \  |
				  \   |               \   |               /   |               \ |
				   \  |                \  |            B /____| auxPoint       \|
					\ |                 \ |                                     C
					 \|                  \|
					  C                   C
		*/
		
		
		const Vertex* top;
		const Vertex* middle;
		const Vertex* bottom;
		
		const float yValues[] = {a.m_pos.y, b.m_pos.y, c.m_pos.y};
		const float upperMostY = getBiggestValue(yValues, sizeofarray(yValues));
		const float lowerMostY = getSmallestValue(yValues, sizeofarray(yValues));
		
		if (a.m_pos.y == upperMostY)
		{
			top = &a;
		}
		else if (b.m_pos.y == upperMostY)
		{
			top = &b;
		}
		else
		{
			top = &c;
		}
		
		if (a.m_pos.y == lowerMostY)
		{
			bottom = &a;
		}
		else if (b.m_pos.y == lowerMostY)
		{
			bottom = &b;
		}
		else
		{
			bottom = &c;
		}
		
		if (a.m_pos.y != upperMostY && a.m_pos.y != lowerMostY)
		{
			middle = &a;
		}
		else if (b.m_pos.y != upperMostY && b.m_pos.y != lowerMostY)
		{
			middle = &b;
		}
		else
		{
			middle = &c;
		}
		
		OMBAssert(top != middle && top != bottom && middle != bottom, "Assigned some vertex twice!");
		OMBAssert(top->m_pos.y > middle->m_pos.y &&
				  top->m_pos.y > bottom->m_pos.y &&
				  middle->m_pos.y > bottom->m_pos.y, "Vertices are not sorted!");
		
		const float slopeTopToBottom = (bottom->m_pos.x - top->m_pos.x) / (bottom->m_pos.y - top->m_pos.y);
		const float factorTopToBottom = - (slopeTopToBottom * top->m_pos.y) + top->m_pos.x;
		const float auxPointX = (slopeTopToBottom * middle->m_pos.y) + factorTopToBottom;
		
		TextureInfo texInfo;
		if (m_bindedTextureId != SoftwareRendererConsts::kInvalidTextureId)
		{
			texInfo = m_textures[m_bindedTextureId];
		}
		const InterpolateResult intRes = interpolate(Vector2f(auxPointX, middle->m_pos.y), a, b, c, texInfo);
		const Vertex auxVertex(Vector4f(auxPointX, middle->m_pos.y, intRes.m_z), intRes.m_color, intRes.m_texCoord);
		
		drawSubTriangle(*top, auxVertex, *middle);
		drawSubTriangle(auxVertex, *bottom, *middle);
	}
}

void SoftwareRenderer::drawTriangleSlow(const Vertex& a, const Vertex& b, const Vertex& c)
{
	/*
		This was the first aproach to draw a triangle I could think of and is based in brute force.
		First, it calculates the bounding box that contains the triangle.
		Then, for each point inside the box, it checks whether or not is inside the triangle, and fills the pixel if it is.
		This method is fine to draw a triangle, but it is extremely inefficient.
	*/

	const Vector2i aFB = ndcCoordToFBCoord((Vector2f)a.m_pos);
	const Vector2i bFB = ndcCoordToFBCoord((Vector2f)b.m_pos);
	const Vector2i cFB = ndcCoordToFBCoord((Vector2f)c.m_pos);
	
	// In order to avoid checking all the points in the surface, we just check those inside the imaginary bounding box surrounding the triangle
	const int xValues[] = {aFB.x, bFB.x, cFB.x};
	const int yValues[] = {aFB.y, bFB.y, cFB.y};
	
	const int leftMostXTr = getSmallestValue(xValues, sizeofarray(xValues));
	const int rightMostXTr = getBiggestValue(xValues, sizeofarray(xValues));
	const int upperMostYTr = getBiggestValue(yValues, sizeofarray(yValues));
	const int lowerMostYTr = getSmallestValue(yValues, sizeofarray(yValues));
	
	const int leftMostX = std::max(leftMostXTr, 0);
	const int rightMostX = std::min(rightMostXTr, m_size.x - 1);
	const int upperMostY = std::min(upperMostYTr, m_size.y - 1);
	const int lowerMostY = std::max(lowerMostYTr, 0);
	
	const Vertex aFBAsVertex(Vector4f(aFB.x, aFB.y, a.m_pos.z), a.m_color, a.m_texCoord);
	const Vertex bFBAsVertex(Vector4f(bFB.x, bFB.y, b.m_pos.z), b.m_color, b.m_texCoord);
	const Vertex cFBAsVertex(Vector4f(cFB.x, cFB.y, c.m_pos.z), c.m_color, c.m_texCoord);
	
	TextureInfo texInfo;
	if (m_bindedTextureId != SoftwareRendererConsts::kInvalidTextureId)
	{
		texInfo = m_textures[m_bindedTextureId];
	}
	
	for (int j = leftMostX; j <= rightMostX; ++j)
	{
		for (int k = upperMostY; k >= lowerMostY; --k)
		{
			Vector2i candidate(j, k);
			if (isPointInTriangle(candidate, aFB, bFB, cFB))
			{
				const InterpolateResult intRes = interpolate(Vector2f(candidate.x, candidate.y), aFBAsVertex, bFBAsVertex, cFBAsVertex, texInfo);
				if (intRes.m_z < getPixelZ(candidate))
				{
					setPixelColor(candidate, intRes.m_color);
					setPixelZ(candidate, intRes.m_z);
				}
			}
		}
	}
}

}