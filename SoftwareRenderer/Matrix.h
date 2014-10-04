// Copyright 2014 Enrique Alcor Martín. All rights reserved.

#pragma once

#include "Assert.h"

#include <cstdio>
#include <cstring>

namespace omb
{

template <unsigned int rows, unsigned int cols>
class Matrix
{
public:
	Matrix();
	
	const float operator()(size_t row, size_t col) const;
	float& operator()(size_t i, size_t col);
private:
	float m_matrix[rows][cols];
	size_t m_rows;
	size_t m_cols;
};

template <unsigned int rows, unsigned int cols>
Matrix<rows, cols>::Matrix()
: m_rows(rows)
, m_cols(cols)
{
	memset(m_matrix, 0, sizeof(float) * m_rows * m_cols);
}

template <unsigned int rows, unsigned int cols>
const float Matrix<rows, cols>::operator()(size_t row, size_t col) const
{
	OMBAssert(row < m_rows, "Row index is out of bounds! (max = %lu, actual = %zu)", m_rows-1, row);
	OMBAssert(col < m_cols, "Column index is out of bounds! (max = %lu, actual = %zu)", m_cols-1, col);
	return m_matrix[row][col];
}

template <unsigned int rows, unsigned int cols>
float& Matrix<rows, cols>::operator()(size_t row, size_t col)
{
	OMBAssert(row < m_rows, "Row index is out of bounds! (max = %lu, actual = %zu)", m_rows-1, row);
	OMBAssert(col < m_cols, "Column index is out of bounds! (max = %lu, actual = %zu)", m_cols-1, col);
	return m_matrix[row][col];
}

typedef Matrix<3, 3> Matrix33;
typedef Matrix<4, 4> Matrix44;
	
namespace MatrixUtils
{
	Matrix44 CreateScaleMatrix(const float scale);
}

}
