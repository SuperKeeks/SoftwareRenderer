//
//  Color.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#ifndef SoftwareRenderer_Color_h
#define SoftwareRenderer_Color_h

#include <stdint.h>

namespace omb
{

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
	
}

#endif
