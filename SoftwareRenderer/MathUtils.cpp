//
//  MathUtils.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "MathUtils.h"

#include "Matrix44.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace omb
{

float MathUtils::DegToRad(float deg)
{
	return deg * M_PI / 180;
}
	
Matrix44 MathUtils::CreateIdentityMatrix()
{
	Matrix44 identity;
	
	identity(0, 0) = 1;
	identity(1, 1) = 1;
	identity(2, 2) = 1;
	identity(3, 3) = 1;
	
	return identity;
}
	
Matrix44 MathUtils::CreateScaleMatrix(const float scale)
{
	Matrix44 scaleMatrix;
	
	scaleMatrix(0, 0) = scale;
	scaleMatrix(1, 1) = scale;
	scaleMatrix(2, 2) = scale;
	scaleMatrix(3, 3) = 1;
	
	return scaleMatrix;
}
	
Matrix44 MathUtils::CreateTranslationMatrix(const float transX, const float transY, const float transZ)
{
	Matrix44 translateMatrix;
	
	translateMatrix(0, 0) = 1;
	translateMatrix(1, 1) = 1;
	translateMatrix(2, 2) = 1;
	translateMatrix(3, 3) = 1;
	
	translateMatrix(0, 3) = transX;
	translateMatrix(1, 3) = transY;
	translateMatrix(2, 3) = transZ;
	
	return translateMatrix;
}

}