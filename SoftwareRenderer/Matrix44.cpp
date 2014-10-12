//
//  Matrix44.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 12/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Matrix44.h"

namespace omb
{
	
Matrix44::Matrix44()
{
	memset(m_matrix, 0, sizeof(float) * kRowsCols * kRowsCols);
}

const float Matrix44::operator()(size_t row, size_t col) const
{
	OMBAssert(row < kRowsCols, "Row index is out of bounds! (max = %u, actual = %zu)", kRowsCols-1, row);
	OMBAssert(col < kRowsCols, "Column index is out of bounds! (max = %u, actual = %zu)", kRowsCols-1, col);
	return m_matrix[row][col];
}

float& Matrix44::operator()(size_t row, size_t col)
{
	OMBAssert(row < kRowsCols, "Row index is out of bounds! (max = %u, actual = %zu)", kRowsCols-1, row);
	OMBAssert(col < kRowsCols, "Column index is out of bounds! (max = %u, actual = %zu)", kRowsCols-1, col);
	return m_matrix[row][col];
}
	
}