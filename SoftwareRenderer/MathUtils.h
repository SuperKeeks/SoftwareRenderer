//
//  MathUtils.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#pragma once

namespace omb
{

struct Matrix44;

namespace MathUtils
{
	float DegToRad(float deg);
	
	Matrix44 CreateIdentityMatrix();
	Matrix44 CreateScaleMatrix(const float scale);
	Matrix44 CreateTranslationMatrix(const float transX, const float transY, const float transZ);
}
	
}
