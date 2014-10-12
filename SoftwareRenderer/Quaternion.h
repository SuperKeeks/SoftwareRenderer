//
//  Quaternion.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#pragma once

namespace omb
{

struct Matrix33;
struct Matrix44;
struct Vector3f;

struct Quaternion
{
	float x, y, z, w;
	
	Quaternion();
	Quaternion(const Vector3f& axis, float angleRad);
	
	operator Matrix33() const;
	operator Matrix44() const;
	Quaternion operator*(const Quaternion& rhs) const;
	void operator*=(const Quaternion& rhs);
};
	
}
