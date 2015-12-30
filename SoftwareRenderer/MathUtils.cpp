//
//  MathUtils.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 11/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "MathUtils.h"

#include "Matrix44.h"
#include "Vector4.h"
#include "Vertex.h"

namespace omb
{

float MathUtils::DegToRad(float deg)
{
	return deg * 3.14159265358979323846 / 180;
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

bool MathUtils::IsVertexInViewFrustum(const Vector4f& vertex)
{
	return fabsf(vertex.x) <= fabsf(vertex.w) &&
		   fabsf(vertex.y) <= fabsf(vertex.w) &&
		   fabsf(vertex.z) <= fabsf(vertex.w);
}

bool MathUtils::IsTriangleInViewFrustum(const Vector4f& a, const Vector4f& b, const Vector4f& c)
{
	return MathUtils::IsVertexInViewFrustum(a) &&
		   MathUtils::IsVertexInViewFrustum(b) &&
		   MathUtils::IsVertexInViewFrustum(c);
}

void MathUtils::ClipVerticesInAxis(const std::vector<Vertex>& vertices, int axisIndex, int sign, std::vector<Vertex>& result)
{
	result.clear();
	for (int i = 0; i < vertices.size(); ++i)
	{
		const Vertex& previous = (i == 0) ? vertices[vertices.size() - 1] : vertices[i - 1];
		const float previousComponent = previous.m_pos.getComponent(axisIndex) * sign;
		bool isPreviousInFrustum = previousComponent <= previous.m_pos.w;
		
		const Vertex& current = vertices[i];
		const float currentComponent = current.m_pos.getComponent(axisIndex) * sign;
		bool isCurrentInFrustum = currentComponent <= current.m_pos.w;

		if (isPreviousInFrustum ^ isCurrentInFrustum)
		{
			float t = (previous.m_pos.w - previousComponent) /
				((previous.m_pos.w - previousComponent) - (current.m_pos.w - currentComponent));
			Vertex interpVertex = previous.lerp(current, t);
			result.push_back(interpVertex);
		}

		if (isCurrentInFrustum)
		{
			result.push_back(current);
		}
	}
}

void MathUtils::ClipVerticesToFrustum(std::vector<Vertex>& vertices)
{
	std::vector<Vertex> aux;

	ClipVerticesInAxis(vertices, 0, 1, aux);
	ClipVerticesInAxis(aux, 0, -1, vertices);
	ClipVerticesInAxis(vertices, 1, 1, aux);
	ClipVerticesInAxis(aux, 1, -1, vertices);
	ClipVerticesInAxis(vertices, 2, 1, aux);
	ClipVerticesInAxis(aux, 2, -1, vertices);
}

void MathUtils::TransformVertices(std::vector<Vertex>& vertices, const Matrix44 transform)
{
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].m_pos = transform * vertices[i].m_pos;
	}
}

}