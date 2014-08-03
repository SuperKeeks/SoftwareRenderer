#pragma once

#include "Utils.h"

#include <stdint.h>
#include <vector>

namespace omb
{

// Forward declarations
struct Color;
struct Vertex;

class SoftwareRenderer
{
public:
	SoftwareRenderer();
	~SoftwareRenderer();
	
	void init(const int width, const int height);
	void clear(const Color& clearColor);
	
	// All drawing functions expect vertices in NDC [-1, 1] range
	
	// Draws a set of single pixel points
	void drawPoints(const std::vector<Vertex>& vertices);
	
	// Draws triangle using the "classic" OpenGL GL_TRIANGLE_STRIP primitive
	void drawTriangleStrip(const std::vector<Vertex>& vertices);
	
	uint8_t* getFrameBuffer() { return m_frameBuffer; }

private:
	static const int kBytesPerPixel = 4;
	static const int kMaxWidth = 2048;
	static const int kMaxHeight = 2048;
	
	bool m_initialised = false;
	uint8_t m_frameBuffer[kMaxWidth * kMaxHeight * kBytesPerPixel];
	Vector2i m_size;
	Vector2i m_halfSize;
	
	Vector2i ndcCoordToFBCoord(const Vector2f& ndcCoord);
	void setPixelColor(const Vector2i& pos, const Color& color);
	void drawLine(const Vector2f& aFB, const Vector2f& bFB, const Color& color);
	void drawTriangleSlow(const Vertex& a, const Vertex& b, const Vertex& c);
	void drawTriangleFaster(const Vertex& a, const Vertex& b, const Vertex& c);
	void drawSubTriangle(const Vertex& a, const Vertex& b, const Vertex& c); // This is for being used with drawTriangleFaster
};

}