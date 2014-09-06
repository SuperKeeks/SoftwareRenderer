#pragma once

#include <cmath>
#include <cstdio>
#include <stdint.h>

namespace omb
{
	
#define sizeofarray(x) (sizeof(x) / sizeof(x[0]))
	
struct Vector2i
{
	int x;
	int y;
	
	Vector2i()
	{
	}
	
	Vector2i(const int x, const int y)
	: x(x)
	, y(y)
	{}
	
	double getLength() const
	{
		return sqrt(x*x + y*y);
	}
	
	void normalize()
	{
		const double length = getLength();
		
		x /= length;
		y /= length;
	}
	
	Vector2i operator-(const Vector2i& rhs) const
	{
		return Vector2i(x - rhs.x, y - rhs.y);
	}
};
	
struct Vector2f
{
	float x;
	float y;
	
	Vector2f()
	{
	}
	
	Vector2f(const float x, const float y)
	: x(x)
	, y(y)
	{}
	
	double getLength() const
	{
		return sqrt(x*x + y*y);
	}
	
	void normalize()
	{
		const double length = getLength();
		
		x /= length;
		y /= length;
	}
	
	operator Vector2i() const
	{
		return Vector2i(x, y);
	}
	
	Vector2f operator+(const Vector2f& rhs) const
	{
		return Vector2f(x + rhs.x, y + rhs.y);
	}
	
	Vector2f operator-(const Vector2f& rhs) const
	{
		return Vector2f(x - rhs.x, y - rhs.y);
	}
	
	Vector2f operator*(const Vector2f& rhs) const
	{
		return Vector2f(x * rhs.x, y * rhs.y);
	}
};
	
struct Vector3f
{
	float x;
	float y;
	float z;
	
	Vector3f(const float x, const float y, const float z)
	: x(x)
	, y(y)
	, z(z)
	{}
	
	double getLength() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	
	void normalize()
	{
		const double length = getLength();
		
		x /= length;
		y /= length;
		z /= length;
	}
	
	operator Vector2f() const
	{
		return Vector2f(x, y);
	}
	
	Vector3f operator+(const Vector3f& rhs) const
	{
		return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	
	Vector3f operator-(const Vector3f& rhs) const
	{
		return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	
	Vector3f operator*(const float rhs) const
	{
		return Vector3f(x * rhs, y * rhs, z * rhs);
	}
};

struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	
	Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
	{
	}
};
	
struct Vertex
{
	Vector3f m_pos;
	Color m_color;
	
	Vertex(const Vector3f& pos, const Color& color)
	: m_pos(pos)
	, m_color(color)
	{
	}
};

// Assert function inspired by http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
#if !DEBUG

#define OMBAssert(condition, errmsg, ...) do { (void)sizeof(condition); } while(0)

#else

#define OMBAssert(condition, errmsg, ...) \
do \
{ \
	if (!(condition)) \
	{ \
		std::printf("%s:%d: Assertion failed: ", (__FILE__), (__LINE__)); \
		std::printf(errmsg, ##__VA_ARGS__); \
		std::printf("\r"); \
		asm("int $3"); \
	} \
} while(0) \

#endif

}