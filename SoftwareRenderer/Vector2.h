//
//  Vector2.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#ifndef SoftwareRenderer_Vector2_h
#define SoftwareRenderer_Vector2_h

namespace omb
{

struct Vector2i
{
	int x;
	int y;
	
	Vector2i();
	Vector2i(const int x, const int y);
	
	double getLength() const;
	void normalize();
	
	Vector2i operator-(const Vector2i& rhs) const;
};

struct Vector2f
{
	float x;
	float y;
	
	Vector2f();
	Vector2f(const float x, const float y);
	
	double getLength() const;
	void normalize();
	
	operator Vector2i() const;
	
	Vector2f operator+(const Vector2f& rhs) const;
	Vector2f operator-(const Vector2f& rhs) const;
	Vector2f operator*(const Vector2f& rhs) const;
};
	
}

#endif
