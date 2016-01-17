//
//  Transform.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 17/01/16.
//  Copyright (c) 2016 Enrique Alcor Martín. All rights reserved.
//

#pragma once

#include "Vector3.h"

namespace omb
{

struct Matrix44;
	
struct Transform
{
	Vector3f m_scale;
	Vector3f m_rotation;
	Vector3f m_position;
	
	Transform();
	Matrix44 calculateFinalTransform() const;
};
}