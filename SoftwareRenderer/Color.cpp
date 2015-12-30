//
//  Color.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 29/12/15.
//  Copyright (c) 2015 Enrique Alcor Martín. All rights reserved.
//

#include "Color.h"

namespace omb
{

Color Color::lerp(const Color& rhs, float t) const
{
	return (*this) * (1.0f - t) + rhs * t;
}

Color Color::operator+(const Color& rhs) const
{
	return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}

Color Color::operator*(const float rhs) const
{
	return Color(r * rhs, g * rhs, b * rhs, a * rhs);
}

}