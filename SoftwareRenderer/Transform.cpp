//
//  Transform.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 17/01/16.
//  Copyright (c) 2016 Enrique Alcor Martín. All rights reserved.
//

#include "Transform.h"

#include "MathUtils.h"
#include "Matrix44.h"
#include "Quaternion.h"

namespace omb
{
	Transform::Transform()
	: m_scale(1, 1, 1)
	, m_rotation(0, 0, 0)
	, m_position(0, 0, 0)
	{
	}
	
	Matrix44 Transform::calculateFinalTransform() const
	{
		const Matrix44 scaleMatrix = MathUtils::CreateScaleMatrix(m_scale.x, m_scale.y, m_scale.z);
		
		const Matrix44 translationMatrix = MathUtils::CreateTranslationMatrix(m_position.x, m_position.y, m_position.z);
		
		const Quaternion rotationTransX(Vector3f(1.0f, 0, 0), MathUtils::DegToRad(m_rotation.x));
		const Quaternion rotationTransY(Vector3f(0, 1.0f, 0), MathUtils::DegToRad(m_rotation.y));
		const Quaternion rotationTransZ(Vector3f(0, 0, 1.0f), MathUtils::DegToRad(m_rotation.z));
		const Quaternion allRotations = rotationTransX * rotationTransY * rotationTransZ;
		const Matrix44 allRotationsMatrix = allRotations;
		
		const Matrix44 result = translationMatrix * allRotationsMatrix * scaleMatrix;
		
		return result;
	}
}
