//
//  Vector3.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Vector3.h"

#include "Matrix44.h"
#include "Vector2.h"

#include <cmath>

namespace omb
{
	
Vector3f::Vector3f()
{}

Vector3f::Vector3f(const float x, const float y, const float z)
: x(x)
, y(y)
, z(z)
{}

double Vector3f::getLength() const
{
	return sqrt(x*x + y*y + z*z);
}

void Vector3f::normalize()
{
	const double length = getLength();
	
	x /= length;
	y /= length;
	z /= length;
}

Vector3f::operator Vector2f() const
{
	return Vector2f(x, y);
}

Vector3f Vector3f::operator+(const Vector3f& rhs) const
{
	return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3f Vector3f::operator-(const Vector3f& rhs) const
{
	return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3f Vector3f::operator*(const float rhs) const
{
	return Vector3f(x * rhs, y * rhs, z * rhs);
}

}