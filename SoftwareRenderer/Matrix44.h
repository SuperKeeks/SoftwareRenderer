// Copyright 2014 Enrique Alcor Martín. All rights reserved.

#pragma once

#include "Assert.h"

#include <cstring>

namespace omb
{
	
struct Vector4f;

struct Matrix44
{
	Matrix44();
	
	const float operator()(size_t row, size_t col) const;
	float& operator()(size_t i, size_t col);
	Vector4f operator*(const Vector4f& rhs) const;
	Matrix44 operator*(const Matrix44& rhs) const;
	
private:
	static const unsigned int kRowsCols = 4;
	float m_matrix[kRowsCols][kRowsCols];
};

}
