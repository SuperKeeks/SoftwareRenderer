//
//  Matrix33.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 12/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Matrix33.h"

#include "Vector3.h"

namespace omb
{

Matrix33::Matrix33()
{
	memset(m_matrix, 0, sizeof(float) * kRowsCols * kRowsCols);
}

const float Matrix33::operator()(size_t row, size_t col) const
{
	OMBAssert(row < kRowsCols, "Row index is out of bounds! (max = %d, actual = %zu)", kRowsCols-1, row);
	OMBAssert(col < kRowsCols, "Column index is out of bounds! (max = %d, actual = %zu)", kRowsCols-1, col);
	return m_matrix[row][col];
}

float& Matrix33::operator()(size_t row, size_t col)
{
	OMBAssert(row < kRowsCols, "Row index is out of bounds! (max = %d, actual = %zu)", kRowsCols-1, row);
	OMBAssert(col < kRowsCols, "Column index is out of bounds! (max = %d, actual = %zu)", kRowsCols-1, col);
	return m_matrix[row][col];
}

Vector3f Matrix33::operator*(const Vector3f& rhs) const
{
	Vector3f result;
	
	result.x = (*this)(0, 0) * rhs.x + (*this)(0, 1) * rhs.y + (*this)(0, 2) * rhs.z;
	result.y = (*this)(1, 0) * rhs.x + (*this)(1, 1) * rhs.y + (*this)(1, 2) * rhs.z;
	result.z = (*this)(2, 0) * rhs.x + (*this)(2, 1) * rhs.y + (*this)(2, 2) * rhs.z;
	
	return result;
}

}