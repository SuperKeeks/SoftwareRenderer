//
//  Quaternion.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Quaternion.h"

#include "Matrix33.h"
#include "Matrix44.h"
#include "Vector3.h"

#include <math.h>

namespace omb
{
	
Quaternion::Quaternion()
{
}

Quaternion::Quaternion(const Vector3f& axis, float angleRad)
{
	Vector3f normalizedAxis = axis;
	normalizedAxis.normalize();
	const float sinRes = sin(0.5 * angleRad);
	
	x = normalizedAxis.x * sinRes;
	y = normalizedAxis.y * sinRes;
	z = normalizedAxis.z * sinRes;
	w = cos(0.5 * angleRad);
}
	
Quaternion::operator Matrix33() const
{
	Matrix33 asMatrix;
	
	asMatrix(0, 0) = 1 - 2*y*y -2*z*z;
	asMatrix(0, 1) = 2*x*y - 2*w*z;
	asMatrix(0, 2) = 2*x*z + 2*w*y;
	
	asMatrix(1, 0) = 2*x*y + 2*w*z;
	asMatrix(1, 1) = 1 - 2*x*x - 2*z*z;
	asMatrix(1, 2) = 2*y*z - 2*w*x;
	
	asMatrix(2, 0) = 2*x*z - 2*w*y;
	asMatrix(2, 1) = 2*y*z + 2*w*x;
	asMatrix(2, 2) = 1 - 2*x*x - 2*y*y;
	
	return asMatrix;
}
	
Quaternion::operator Matrix44() const
{
	Matrix44 asMatrix;
	
	asMatrix(0, 0) = 1 - 2*y*y -2*z*z;
	asMatrix(0, 1) = 2*x*y - 2*w*z;
	asMatrix(0, 2) = 2*x*z + 2*w*y;
	asMatrix(0, 3) = 0;
	
	asMatrix(1, 0) = 2*x*y + 2*w*z;
	asMatrix(1, 1) = 1 - 2*x*x - 2*z*z;
	asMatrix(1, 2) = 2*y*z - 2*w*x;
	asMatrix(1, 3) = 0;
	
	asMatrix(2, 0) = 2*x*z - 2*w*y;
	asMatrix(2, 1) = 2*y*z + 2*w*x;
	asMatrix(2, 2) = 1 - 2*x*x - 2*y*y;
	asMatrix(2, 3) = 0;
	
	asMatrix(3, 0) = 0;
	asMatrix(3, 1) = 0;
	asMatrix(3, 2) = 0;
	asMatrix(3, 3) = 1;
	
	return asMatrix;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
	Quaternion result;
	
	result.x = w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y;
	result.y = w*rhs.y + y*rhs.w + z*rhs.x - x*rhs.z;
	result.z = w*rhs.z + z*rhs.w + x*rhs.y - y*rhs.x;
	result.w = w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z;
	
	return result;
}

void Quaternion::operator*=(const Quaternion& rhs)
{
	*this = *this * rhs;
}

}
