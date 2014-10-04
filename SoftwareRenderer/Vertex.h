//
//  Vertex.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#ifndef SoftwareRenderer_Vertex_h
#define SoftwareRenderer_Vertex_h

#include "Color.h"
#include "Vector3.h"

namespace omb
{

struct Vertex
{
	Vector3f m_pos;
	Color m_color;
	
	Vertex(const Vector3f& pos, const Color& color)
	: m_pos(pos)
	, m_color(color)
	{
	}
};
	
}

#endif
