//
//  Matrix.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "Matrix.h"

namespace omb
{

Matrix44 MatrixUtils::CreateScaleMatrix(const float scale)
{
	Matrix44 scaleMatrix;
	
	scaleMatrix(0, 0) = scale;
	scaleMatrix(1, 1) = scale;
	scaleMatrix(2, 2) = scale;
	scaleMatrix(3, 3) = 1.0f;
	
	return scaleMatrix;
}

}