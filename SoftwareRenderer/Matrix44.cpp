//
//  Matrix44.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 12/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Matrix44.h"

#include "Vector4.h"

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
	
Vector4f Matrix44::operator*(const Vector4f& rhs) const
{
	Vector4f result;
	
	result.x = (*this)(0, 0) * rhs.x + (*this)(0, 1) * rhs.y + (*this)(0, 2) * rhs.z + (*this)(0, 3) * rhs.w;
	result.y = (*this)(1, 0) * rhs.x + (*this)(1, 1) * rhs.y + (*this)(1, 2) * rhs.z + (*this)(1, 3) * rhs.w;
	result.z = (*this)(2, 0) * rhs.x + (*this)(2, 1) * rhs.y + (*this)(2, 2) * rhs.z + (*this)(2, 3) * rhs.w;
	result.w = (*this)(3, 0) * rhs.x + (*this)(3, 1) * rhs.y + (*this)(3, 2) * rhs.z + (*this)(3, 3) * rhs.w;
	
	return result;
}
	
Matrix44 Matrix44::operator*(const Matrix44& rhs) const
{
	Matrix44 result;
	
	for (int i = 0; i < kRowsCols; ++i)
	{
		for (int j = 0; j < kRowsCols; ++j)
		{
			float temp = 0;
			
			for (int k = 0; k < kRowsCols; ++k)
			{
				temp += (*this)(i, k) * rhs(k, j);
			}
			
			result(i, j) = temp;
		}
	}
	
	return result;
}
	
}