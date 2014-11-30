//
//  Vector2.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Vector2.h"

#include <cmath>

namespace omb
{

// Vector2i

Vector2i::Vector2i()
{
}

Vector2i::Vector2i(const int x, const int y)
: x(x)
, y(y)
{}

double Vector2i::getLength() const
{
	return sqrt(x*x + y*y);
}

void Vector2i::normalize()
{
	const double length = getLength();
	
	x /= length;
	y /= length;
}

Vector2i Vector2i::operator-(const Vector2i& rhs) const
{
	return Vector2i(x - rhs.x, y - rhs.y);
}
	
// Vector2f

Vector2f::Vector2f()
{
}

Vector2f::Vector2f(const float x, const float y)
: x(x)
, y(y)
{}

double Vector2f::getLength() const
{
	return sqrt(x*x + y*y);
}

void Vector2f::normalize()
{
	const double length = getLength();
	
	x /= length;
	y /= length;
}

Vector2f::operator Vector2i() const
{
	return Vector2i(x, y);
}

Vector2f Vector2f::operator+(const Vector2f& rhs) const
{
	return Vector2f(x + rhs.x, y + rhs.y);
}

Vector2f Vector2f::operator-(const Vector2f& rhs) const
{
	return Vector2f(x - rhs.x, y - rhs.y);
}

Vector2f Vector2f::operator*(const Vector2f& rhs) const
{
	return Vector2f(x * rhs.x, y * rhs.y);
}
	
Vector2f Vector2f::operator*(const float rhs) const
{
	return Vector2f(x * rhs, y * rhs);
}
	
Vector2f Vector2f::operator/(const float rhs) const
{
	return Vector2f(x / rhs, y / rhs);
}

}