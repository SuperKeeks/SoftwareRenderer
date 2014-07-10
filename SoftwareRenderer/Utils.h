#include <cstdio>

namespace omb
{

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