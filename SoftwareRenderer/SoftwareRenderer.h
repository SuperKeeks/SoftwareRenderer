#pragma once

#include "Color.h"
#include "Matrix44.h"
#include "Vector2.h"

#include <stdint.h>
#include <vector>

namespace omb
{

// Forward declarations
struct Vertex;
	
struct TextureInfo
{
	unsigned char* m_data = nullptr;
	Vector2i m_size;
	uint8_t m_bytesPerPixel = 0;
	
	bool isValid() const
	{
		return m_data;
	}
};
	
namespace SoftwareRendererConsts
{
	static const int16_t kInvalidTextureId = -1;
}

class SoftwareRenderer
{
public:
	SoftwareRenderer();
	~SoftwareRenderer();
	
	void init(const int width, const int height);
	void clear(const Color& clearColor);

	void setViewProjectionMatrix(const Matrix44& viewProjectionMatrix);
	
	// All drawing functions expect vertices in NDC [-1, 1] range
	
	// Draws a set of single pixel points
	void drawPoints(std::vector<Vertex> vertices, const Matrix44& transform);
	
	// Draws triangle using the "classic" OpenGL GL_TRIANGLES primitive
	void drawTriangles(std::vector<Vertex> vertices, const Matrix44& transform);
	
	// Draws triangle using the "classic" OpenGL GL_TRIANGLE_STRIP primitive
	void drawTriangleStrip(std::vector<Vertex> vertices, const Matrix44& transform);
	
	// Texture methods
	int16_t loadTexture(const char* fileName);
	void unloadTexture(int16_t textureId);
	void bindTexture(int16_t textureId);
	void unbindTexture();
	
	// Wireframe mode
	void setWireFrameModeEnabled(bool enable);
	bool getWireFrameModeEnabled() const;
	void setWireFrameModeColor(const Color& color);
	
	// Backface culling
	void setBackFaceCullingEnabled(bool enable);
	bool getBackFaceCullingEnabled() const;
	
	// Returns the framebuffer, ready to show on screen
	uint8_t* getFrameBuffer() { return m_frameBuffer; }

private:
	static const uint8_t kBytesPerPixel = 4;
	static const int kMaxWidth = 2048;
	static const int kMaxHeight = 2048;
	static const uint8_t kMaxTextures = 128;
	
	bool m_initialised = false;
	uint8_t m_frameBuffer[kMaxWidth * kMaxHeight * kBytesPerPixel];
	float m_zBuffer[kMaxWidth * kMaxHeight];
	Matrix44 m_viewProjectionMatrix;
	TextureInfo m_textures[kMaxTextures];
	Vector2i m_size;
	Vector2i m_halfSize;
	int16_t m_bindedTextureId = SoftwareRendererConsts::kInvalidTextureId;
	bool m_wireFrameModeEnabled = false;
	Color m_wireFrameModeColor = Color(0, 255, 0, 255);
	bool m_backFaceCullingEnabled = true;
	
	void transformVertices(std::vector<Vertex>& vertices, const Matrix44& modelTransform);
	Vector2i ndcCoordToFBCoord(const Vector2f& ndcCoord);
	void setPixelColor(const Vector2i& pos, const Color& color);
	void setPixelZ(const Vector2i& pos, const float z);
	float getPixelZ(const Vector2i& pos);
	void drawLine(const Vector2f& a, const Vector2f& b, const Color& color);
	void clipAndDrawTriangle(const Vertex& a, const Vertex& b, const Vertex& c);
	void drawTriangleSlow(const Vertex& a, const Vertex& b, const Vertex& c);
	void drawTriangleFaster(const Vertex& a, const Vertex& b, const Vertex& c);
	void drawTriangleWireframe(const Vertex& a, const Vertex& b, const Vertex& c);
	
	// This is for being used with drawTriangleFaster
	// The only condition is that 2 points have to share the same Y coordinate (flat top or flat bottom of the triangle)
	// If a triangle doesn't meet this condition, it would need to be split in 2 smaller triangles that meet it and then
	// this function can be called twice to draw them independently
	void drawSubTriangle(const Vertex& a, const Vertex& b, const Vertex& c);
};

}