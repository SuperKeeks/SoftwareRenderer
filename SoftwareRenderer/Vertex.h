//
//  Vertex.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 04/10/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#pragma once

#include "Color.h"
#include "Vector4.h"

namespace omb
{

struct Vertex
{
	Vector4f m_pos;
	Color m_color;
	
	Vertex(const Vector4f& pos, const Color& color)
	: m_pos(pos)
	, m_color(color)
	{
	}
};
	
}
