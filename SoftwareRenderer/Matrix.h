// Copyright 2014 Enrique Alcor Martín. All rights reserved.

#pragma once

#include "Utils.h"

#include <cstdio>

namespace omb
{

template <int rows, int cols>
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

typedef Matrix<3, 3> Matrix33;

template <int rows, int cols>
Matrix<rows, cols>::Matrix()
: m_rows(rows)
, m_cols(cols)
{
}

template <int rows, int cols>
const float Matrix<rows, cols>::operator()(size_t row, size_t col) const
{
	OMBAssert(row < m_rows, "Row index is out of bounds! (max = %lu, actual = %zu)", m_rows-1, row);
	OMBAssert(col < m_cols, "Column index is out of bounds! (max = %lu, actual = %zu)", m_cols-1, col);
	return m_matrix[row][col];
}

template <int rows, int cols>
float& Matrix<rows, cols>::operator()(size_t row, size_t col)
{
	OMBAssert(row < m_rows, "Row index is out of bounds! (max = %lu, actual = %zu)", m_rows-1, row);
	OMBAssert(col < m_cols, "Column index is out of bounds! (max = %lu, actual = %zu)", m_cols-1, col);
	return m_matrix[row][col];
}

}
