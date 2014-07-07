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

}