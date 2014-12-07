//
//  MathUtils.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "MathUtils.h"

#include "Matrix44.h"

#include <math.h>

namespace omb
{

float MathUtils::DegToRad(float deg)
{
	return deg * M_PI / 180;
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

}