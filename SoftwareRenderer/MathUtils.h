//
//  MathUtils.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#pragma once

#include <vector>

namespace omb
{

struct Matrix44;
struct Vector4f;
struct Vertex;

namespace MathUtils
{
	float DegToRad(float deg);
	
	Matrix44 CreateIdentityMatrix();
	Matrix44 CreateScaleMatrix(const float scale);
	Matrix44 CreateScaleMatrix(const float scaleX, const float scaleY, const float scaleZ);
	Matrix44 CreateTranslationMatrix(const float transX, const float transY, const float transZ);
	Matrix44 CreateOrthographicMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far);
	Matrix44 CreatePerspectiveMatrix(const float frustumScale, const float near, const float far);

	bool IsVertexInViewFrustum(const Vector4f& vertex);
	bool IsTriangleInViewFrustum(const Vector4f& a, const Vector4f& b, const Vector4f& c);
	void ClipVerticesInAxis(const std::vector<Vertex>& vertices, int axisIndex, int sign, std::vector<Vertex>& result);
	void ClipVerticesToFrustum(std::vector<Vertex>& vertices);
	void TransformVertices(std::vector<Vertex>& vertices, const Matrix44 transform);
}
	
}
