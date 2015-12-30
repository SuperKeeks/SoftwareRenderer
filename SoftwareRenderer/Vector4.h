//
//  Vector3.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 30/11/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#pragma once

namespace omb
{

struct Vector2f;
struct Vector3f;

struct Vector4f
{
	float x;
	float y;
	float z;
	float w;
	
	Vector4f();
	Vector4f(const float x, const float y, const float z, const float w = 1.0f);
	
	double getLength() const;
	void normalize();
	float getComponent(int index) const;
	Vector4f lerp(const Vector4f& rhs, float t) const;
	
	operator Vector2f() const;
	operator Vector3f() const;
	Vector4f operator+(const Vector4f& rhs) const;
	Vector4f operator-(const Vector4f& rhs) const;
	Vector4f operator*(const float rhs) const;
	Vector4f operator/(const float rhs) const;
};
	
}
