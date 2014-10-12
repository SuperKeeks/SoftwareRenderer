// Copyright 2014 Enrique Alcor Martín. All rights reserved.

#pragma once

#include "Assert.h"

#include "Vector3.h"

#include <cstring>

namespace omb
{

struct Matrix33
{
	Matrix33();
	
	const float operator()(size_t row, size_t col) const;
	float& operator()(size_t i, size_t col);
	Vector3f operator*(const Vector3f& rhs) const;
	
private:
	static const unsigned int kRowsCols = 3;
	float m_matrix[kRowsCols][kRowsCols];
};
	
}
