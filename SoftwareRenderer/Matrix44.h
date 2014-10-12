// Copyright 2014 Enrique Alcor Martín. All rights reserved.

#pragma once

#include "Assert.h"

#include <cstring>

namespace omb
{

struct Matrix44
{
	static const unsigned int kRowsCols = 4;
	float m_matrix[kRowsCols][kRowsCols];
	
	Matrix44();
	
	const float operator()(size_t row, size_t col) const;
	float& operator()(size_t i, size_t col);
};

}
