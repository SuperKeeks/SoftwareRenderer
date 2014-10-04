//
//  Vector3.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#ifndef SoftwareRenderer_Vector3_h
#define SoftwareRenderer_Vector3_h

#include "Matrix.h"

namespace omb
{
	
struct Vector2f;

struct Vector3f
{
	float x;
	float y;
	float z;
	
	Vector3f(const float x, const float y, const float z);
	
	double getLength() const;
	void normalize();
	
	operator Vector2f() const;
	Vector3f operator+(const Vector3f& rhs) const;
	Vector3f operator-(const Vector3f& rhs) const;
	Vector3f operator*(const float rhs) const;
	Vector3f operator*(const Matrix44& rhs) const;
	void operator*=(const Matrix44& rhs);
};
	
}

#endif
