#include <cstdio>

namespace omb
{
	
struct Vector2
{
	float m_x;
	float m_y;
	
	Vector2(const float x, const float y)
	: m_x(x)
	, m_y(y)
	{}
	
	Vector2 operator+(const Vector2& rhs) const
	{
		return Vector2(m_x + rhs.m_x, m_y + rhs.m_y);
	}
	
	Vector2 operator-(const Vector2& rhs) const
	{
		return Vector2(m_x - rhs.m_x, m_y - rhs.m_y);
	}
	
	Vector2 operator*(const Vector2& rhs) const
	{
		return Vector2(m_x * rhs.m_x, m_y * rhs.m_y);
	}
};
	
struct Vector3
{
	float m_x;
	float m_y;
	float m_z;
	
	Vector3(const float x, const float y, const float z)
	: m_x(x)
	, m_y(y)
	, m_z(z)
	{}
	
	operator Vector2() const
	{
		return Vector2(m_x, m_y);
	}
};

struct Color
{
	uint8_t m_r;
	uint8_t m_g;
	uint8_t m_b;
	uint8_t m_a;
	
	Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
	: m_r(r)
	, m_g(g)
	, m_b(b)
	, m_a(a)
	{
	}
};
	
struct Vertex
{
	Vector3 m_pos;
	Color m_color;
	
	Vertex(const Vector3& pos, const Color& color)
	: m_pos(pos)
	, m_color(color)
	{
	}
};

// Assert function inspired by http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
#if !DEBUG

#define OMBAssert(condition, errmsg, ...) do { (void)sizeof(x); } while(0)

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