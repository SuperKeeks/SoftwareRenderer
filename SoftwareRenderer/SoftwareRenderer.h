#include <stdint.h>

namespace omb
{

// Forward declarations
struct Color;

class SoftwareRenderer
{
public:
	SoftwareRenderer();
	~SoftwareRenderer();
	
	void init(const int width, const int height);
	void clear(const Color& clearColor);
	
	uint8_t* getFrameBuffer() { return m_frameBuffer; }

private:
	bool m_initialised = false;
	uint8_t m_frameBuffer[2048 * 2048 * 4];
	int m_width;
	int m_height;
};

}