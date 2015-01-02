//
//  Vector3.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 30/11/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Vector4.h"

#include "Vector2.h"
#include "Vector3.h"

#include <cmath>

namespace omb
{
	
Vector4f::Vector4f()
: x(0)
, y(0)
, z(0)
, w(0)
{}

Vector4f::Vector4f(const float x, const float y, const float z, const float w)
: x(x)
, y(y)
, z(z)
, w(w)
{}

double Vector4f::getLength() const
{
	return sqrt(x*x + y*y + z*z + w*w);
}

void Vector4f::normalize()
{
	const double length = getLength();
	
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}

Vector4f::operator Vector2f() const
{
	return Vector2f(x, y);
}
	
Vector4f::operator Vector3f() const
{
	return Vector3f(x, y, z);
}

Vector4f Vector4f::operator+(const Vector4f& rhs) const
{
	return Vector4f(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Vector4f Vector4f::operator-(const Vector4f& rhs) const
{
	return Vector4f(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Vector4f Vector4f::operator*(const float rhs) const
{
	return Vector4f(x * rhs, y * rhs, z * rhs, w * rhs);
}
	
Vector4f Vector4f::operator/(const float rhs) const
{
	return Vector4f(x / rhs, y / rhs, z / rhs, w / rhs);
}
	
}