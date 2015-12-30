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
#include "MathUtils.h"
#include "SizeOfArray.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vertex.h"

#define STB_IMAGE_IMPLEMENTATION
#include "externals/stb_image.h"

#include <algorithm>
#include <cmath>

#define USE_SLOW_TRIANGLE_METHOD 0

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
	
	struct VertexInterpInfo
	{
		const Vector2f& m_fbPos; // Position in FB coordinates
		const float m_z;
		const Color& m_color;
		const float m_uOverW;
		const float m_vOverW;
		const float m_1OverW;
		
		VertexInterpInfo(const Vector2f& fbPos, float z, const Color& color, float uOverW, float vOverW, float oneOverW)
		: m_fbPos(fbPos)
		, m_z(z)
		, m_color(color)
		, m_uOverW(uOverW)
		, m_vOverW(vOverW)
		, m_1OverW(oneOverW)
		{
		}
	};
	
	struct InterpolateResult
	{
		Color m_color;
		Vector2f m_texCoord;
		float m_z;
		float m_w;
		
		InterpolateResult()
		: m_color(0, 0, 0, 0)
		, m_z(0)
		, m_w(0)
		{
		}
	};
	
	InterpolateResult interpolate(const Vector2f& point, const VertexInterpInfo& a, const VertexInterpInfo& b, const VertexInterpInfo& c, const TextureInfo& texInfo)
	{
		InterpolateResult res;
		
		// Barycentric color interpolation. For more info: http://classes.soe.ucsc.edu/cmps160/Fall10/resources/barycentricInterpolation.pdf
		const float totalArea = fabsf((a.m_fbPos.x*(b.m_fbPos.y - c.m_fbPos.y) + b.m_fbPos.x*(c.m_fbPos.y - a.m_fbPos.y) + c.m_fbPos.x*(a.m_fbPos.y - b.m_fbPos.y))/2);
		const float pabArea = fabsf((point.x*(a.m_fbPos.y - b.m_fbPos.y) + a.m_fbPos.x*(b.m_fbPos.y - point.y) + b.m_fbPos.x*(point.y - a.m_fbPos.y))/2);
		const float pacArea = fabsf((point.x*(a.m_fbPos.y - c.m_fbPos.y) + a.m_fbPos.x*(c.m_fbPos.y - point.y) + c.m_fbPos.x*(point.y - a.m_fbPos.y))/2);
		const float pbcArea = fabsf((point.x*(b.m_fbPos.y - c.m_fbPos.y) + b.m_fbPos.x*(c.m_fbPos.y - point.y) + c.m_fbPos.x*(point.y - b.m_fbPos.y))/2);
		
		const float interp1OverW = pbcArea/totalArea * a.m_1OverW + pacArea/totalArea * b.m_1OverW + pabArea/totalArea * c.m_1OverW;
		
		if (texInfo.isValid())
		{
			const float interpUOverW = pbcArea/totalArea * a.m_uOverW + pacArea/totalArea * b.m_uOverW + pabArea/totalArea * c.m_uOverW;
			const float interpVOverW = pbcArea/totalArea * a.m_vOverW + pacArea/totalArea * b.m_vOverW + pabArea/totalArea * c.m_vOverW;
			
			res.m_texCoord.x = interpUOverW / interp1OverW;
			res.m_texCoord.y = interpVOverW / interp1OverW;

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
		
		res.m_z = pbcArea/totalArea * a.m_z + pacArea/totalArea * b.m_z + pabArea/totalArea * c.m_z;
		res.m_w = 1.0f / interp1OverW;
		
		return res;
	}
	
	bool isBackFace(const Vector2f& a, const Vector2f& b, const Vector2f& c)
	{
		// Omit back faces based on triangle winding based on the sign of the determinant of vectors
		// For more info check: http://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
		const float position = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
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

void SoftwareRenderer::setViewProjectionMatrix(const Matrix44& viewProjectionMatrix)
{
	m_viewProjectionMatrix = viewProjectionMatrix;
}

void SoftwareRenderer::drawPoints(std::vector<Vertex> vertices, const Matrix44& transform)
{
	OMBAssert(m_initialised, "Uninitialized!");

	transformVertices(vertices, transform);
	
	for (int i = 0; i < vertices.size(); ++i)
	{
		const Vertex& vertex = vertices[i];
		if (MathUtils::IsVertexInViewFrustum(vertex.m_pos))
		{
			const Vector2i fbPos = ndcCoordToFBCoord((Vector2f)vertex.m_pos);
			setPixelColor(fbPos, vertex.m_color);
		}
	}
}

void SoftwareRenderer::drawTriangles(std::vector<Vertex> vertices, const Matrix44& transform)
{
	OMBAssert(m_initialised, "Uninitialized!");
	OMBAssert(vertices.size() % 3 == 0, "Total number of vertices is not a multiple of 3!");

	transformVertices(vertices, transform);
	
	for (int i = 0; i < vertices.size() - 2; i += 3)
	{
		clipAndDrawTriangle(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
	}
}

void SoftwareRenderer::clipAndDrawTriangle(const Vertex& a, const Vertex& b, const Vertex& c)
{
	std::vector<Vertex> auxVertices;
	auxVertices.push_back(a);
	auxVertices.push_back(b);
	auxVertices.push_back(c);

	if (!MathUtils::IsTriangleInViewFrustum(a.m_pos, b.m_pos, c.m_pos))
	{
		auxVertices = MathUtils::ClipVerticesToFrustum(auxVertices);
		if (auxVertices.size() == 0)
		{
			// Triangle was completely out of the frustum
			return;
		}
	}

	// When triangles are clipped, they have to be drawn in a "triangle fan" way.
	// When they're not, it doesn't make a difference to do it this way.
	const Vertex& v0 = auxVertices[0];
	for (int j = 1; j < auxVertices.size() - 1; ++j)
	{
		const Vertex& v1 = auxVertices[j];
		const Vertex& v2 = auxVertices[j + 1];

		if (m_wireFrameModeEnabled)
		{
			drawTriangleWireframe(v0, v1, v2);
		}
		else
		{
#if USE_SLOW_TRIANGLE_METHOD
			drawTriangleSlow(v0, v1, v2);
#else
			drawTriangleFaster(v0, v1, v2);
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
	
void SoftwareRenderer::setWireFrameModeEnabled(bool enable)
{
	m_wireFrameModeEnabled = enable;
}
	
bool SoftwareRenderer::getWireFrameModeEnabled() const
{
	return m_wireFrameModeEnabled;
}
	
void SoftwareRenderer::setWireFrameModeColor(const Color& color)
{
	m_wireFrameModeColor = color;
}
	
void SoftwareRenderer::setBackFaceCullingEnabled(bool enable)
{
	m_backFaceCullingEnabled = enable;
}

bool SoftwareRenderer::getBackFaceCullingEnabled() const
{
	return m_backFaceCullingEnabled;
}

void SoftwareRenderer::drawTriangleStrip(std::vector<Vertex> vertices, const Matrix44& transform)
{
	OMBAssert(m_initialised, "Uninitialized!");

	transformVertices(vertices, transform);
	
	for (int i = 0; i < vertices.size() - 2; ++i)
	{
		const Vertex& a = vertices[i + 0];
		const Vertex& b = vertices[i + ((i % 2 == 0) ? 1 : 2)];
		const Vertex& c = vertices[i + ((i % 2 == 0) ? 2 : 1)];

		clipAndDrawTriangle(a, b, c);
	}
}

void SoftwareRenderer::transformVertices(std::vector<Vertex>& vertices, const Matrix44& modelTransform)
{
	Matrix44 finalTransMatrix = m_viewProjectionMatrix * modelTransform;

	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].m_pos = finalTransMatrix * vertices[i].m_pos;
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
void SoftwareRenderer::drawLine(const Vector2f& a, const Vector2f& b, const Color& color)
{
	const Vector2f aFB = ndcCoordToFBCoord(a);
	const Vector2f bFB = ndcCoordToFBCoord(b);
	
	Vector2f aToB = bFB - aFB;
	const double aToBLength = aToB.getLength();
	
	if (aToBLength > 0)
	{
		aToB.normalize();
		
		for (int i = 0; i <= aToBLength; ++i)
		{
			Vector2i fragment;
			fragment.x = aFB.x + round(aToB.x * i);
			fragment.y = aFB.y + round(aToB.y * i);
			
			if (fragment.x >= 0 && fragment.x < m_size.x && fragment.y >= 0 && fragment.y < m_size.y)
			{
				setPixelColor(fragment, color);
			}
		}
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
	const int upperMostYTr = getBiggestValue(yValues, sizeofarray(yValues));
	const int lowerMostYTr = getSmallestValue(yValues, sizeofarray(yValues));
	
	// Limit the paint area to the framebuffer, so we don't paint off screen
	const int upperMostY = std::min(upperMostYTr, m_size.y - 1);
	const int lowerMostY = std::max(lowerMostYTr, 0);
	
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
	
	for (int y = upperMostY; y >= lowerMostY; --y)
	{
		const int minXTr = ceil((slopePBA * y) + factorPBA);
		const int maxXTr = floor((slopePBB * y) + factorPBB);
		
		// Again, check we're not off screen
		const int maxX = std::min(maxXTr, m_size.x - 1);
		const int minX = std::max(minXTr, 0);
		
		for (int x = minX; x <= maxX; ++x)
		{
			const Vector2i position(x, y);
			TextureInfo texInfo;
			if (m_bindedTextureId != SoftwareRendererConsts::kInvalidTextureId)
			{
				texInfo = m_textures[m_bindedTextureId];
			}

			Vector2f aPosAsV2f(aFB.x, aFB.y);
			Vector2f bPosAsV2f(bFB.x, bFB.y);
			Vector2f cPosAsV2f(cFB.x, cFB.y);
			const VertexInterpInfo aInfo(aPosAsV2f, a.m_pos.z, a.m_color, a.m_texCoord.x / a.m_pos.w, a.m_texCoord.y / a.m_pos.w, 1.0f / a.m_pos.w);
			const VertexInterpInfo bInfo(bPosAsV2f, b.m_pos.z, b.m_color, b.m_texCoord.x / b.m_pos.w, b.m_texCoord.y / b.m_pos.w, 1.0f / b.m_pos.w);
			const VertexInterpInfo cInfo(cPosAsV2f, c.m_pos.z, c.m_color, c.m_texCoord.x / c.m_pos.w, c.m_texCoord.y / c.m_pos.w, 1.0f / c.m_pos.w);
			
			const InterpolateResult intRes = interpolate(Vector2f(position.x, position.y), aInfo, bInfo, cInfo, texInfo);
			
			if (intRes.m_z < getPixelZ(position))
			{
				setPixelColor(position, intRes.m_color);
				setPixelZ(position, intRes.m_z);
			}
		}
	}
}

void SoftwareRenderer::drawTriangleFaster(const Vertex& aRaw, const Vertex& bRaw, const Vertex& cRaw)
{
	Vertex a = aRaw;
	Vertex b = bRaw;
	Vertex c = cRaw;
	
	a.m_pos.x = a.m_pos.x / aRaw.m_pos.w;
	a.m_pos.y = a.m_pos.y / aRaw.m_pos.w;
	b.m_pos.x = b.m_pos.x / bRaw.m_pos.w;
	b.m_pos.y = b.m_pos.y / bRaw.m_pos.w;
	c.m_pos.x = c.m_pos.x / cRaw.m_pos.w;
	c.m_pos.y = c.m_pos.y / cRaw.m_pos.w;
	
	// Skip back faces if needed
	if (m_backFaceCullingEnabled && isBackFace((Vector2f)a.m_pos, (Vector2f)b.m_pos, (Vector2f)c.m_pos))
	{
		return;
	}
	
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
		           /  |                /  |                   A          B ______ auxPoint
		          /   |               /   |                  /|            \    |
		       B /    |      =>    B /____| auxPoint =>     / |      +      \   |
		         \    |              \    |                /  |              \  |
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

		Vector2f aPosAsV2f(a.m_pos.x, a.m_pos.y);
		Vector2f bPosAsV2f(b.m_pos.x, b.m_pos.y);
		Vector2f cPosAsV2f(c.m_pos.x, c.m_pos.y);
		const VertexInterpInfo aInfo(aPosAsV2f, a.m_pos.z, a.m_color, a.m_texCoord.x / a.m_pos.w, a.m_texCoord.y / a.m_pos.w, 1.0f / a.m_pos.w);
		const VertexInterpInfo bInfo(bPosAsV2f, b.m_pos.z, b.m_color, b.m_texCoord.x / b.m_pos.w, b.m_texCoord.y / b.m_pos.w, 1.0f / b.m_pos.w);
		const VertexInterpInfo cInfo(cPosAsV2f, c.m_pos.z, c.m_color, c.m_texCoord.x / c.m_pos.w, c.m_texCoord.y / c.m_pos.w, 1.0f / c.m_pos.w);
		
		const InterpolateResult intRes = interpolate(Vector2f(auxPointX, middle->m_pos.y), aInfo, bInfo, cInfo, texInfo);
		const Vertex auxVertex(Vector4f(auxPointX, middle->m_pos.y, intRes.m_z, intRes.m_w), intRes.m_color, intRes.m_texCoord);
		
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

	const Vector2i aFB = ndcCoordToFBCoord((Vector2f)a.m_pos / a.m_pos.w);
	const Vector2i bFB = ndcCoordToFBCoord((Vector2f)b.m_pos / b.m_pos.w);
	const Vector2i cFB = ndcCoordToFBCoord((Vector2f)c.m_pos / c.m_pos.w);
	
	// Skip back faces if needed
	if (m_backFaceCullingEnabled && isBackFace(aFB, bFB, cFB))
	{
		return;
	}
	
	// In order to avoid checking all the points in the surface, we just check those inside the imaginary bounding box surrounding the triangle
	const int xValues[] = {aFB.x, bFB.x, cFB.x};
	const int yValues[] = {aFB.y, bFB.y, cFB.y};
	
	const int leftMostXTr = getSmallestValue(xValues, sizeofarray(xValues));
	const int rightMostXTr = getBiggestValue(xValues, sizeofarray(xValues));
	const int upperMostYTr = getBiggestValue(yValues, sizeofarray(yValues));
	const int lowerMostYTr = getSmallestValue(yValues, sizeofarray(yValues));
	
	// Limits the bounding box to the framebuffer surface, so we don't try to paint off screen
	const int leftMostX = std::max(leftMostXTr, 0);
	const int rightMostX = std::min(rightMostXTr, m_size.x - 1);
	const int upperMostY = std::min(upperMostYTr, m_size.y - 1);
	const int lowerMostY = std::max(lowerMostYTr, 0);
	
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
				Vector2f aPosAsV2f(aFB.x, aFB.y);
				Vector2f bPosAsV2f(bFB.x, bFB.y);
				Vector2f cPosAsV2f(cFB.x, cFB.y);
				const VertexInterpInfo aInfo(aPosAsV2f, a.m_pos.z, a.m_color, a.m_texCoord.x / a.m_pos.w, a.m_texCoord.y / a.m_pos.w, 1.0f / a.m_pos.w);
				const VertexInterpInfo bInfo(bPosAsV2f, b.m_pos.z, b.m_color, b.m_texCoord.x / b.m_pos.w, b.m_texCoord.y / b.m_pos.w, 1.0f / b.m_pos.w);
				const VertexInterpInfo cInfo(cPosAsV2f, c.m_pos.z, c.m_color, c.m_texCoord.x / c.m_pos.w, c.m_texCoord.y / c.m_pos.w, 1.0f / c.m_pos.w);
				
				const InterpolateResult intRes = interpolate(Vector2f(candidate.x, candidate.y), aInfo, bInfo, cInfo, texInfo);
				if (intRes.m_z < getPixelZ(candidate))
				{
					setPixelColor(candidate, intRes.m_color);
					setPixelZ(candidate, intRes.m_z);
				}
			}
		}
	}
}
	
void SoftwareRenderer::drawTriangleWireframe(const Vertex& a, const Vertex& b, const Vertex& c)
{
	Vector2f divA = (Vector2f)a.m_pos / a.m_pos.w;
	Vector2f divB = (Vector2f)b.m_pos / b.m_pos.w;
	Vector2f divC = (Vector2f)c.m_pos / c.m_pos.w;
	
	if (!m_backFaceCullingEnabled || !isBackFace(divA, divB, divC))
	{
		drawLine(divA, divB, m_wireFrameModeColor);
		drawLine(divA, divC, m_wireFrameModeColor);
		drawLine(divB, divC, m_wireFrameModeColor);
	}
}

}