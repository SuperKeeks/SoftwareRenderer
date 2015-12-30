//
//  Vertex.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Mart�n on 29/12/15.
//  Copyright (c) 2015 Enrique Alcor Mart�n. All rights reserved.
//

#include "Vertex.h"

namespace omb
{

Vertex Vertex::lerp(const Vertex& rhs, float t) const
{
	Vertex result;
	result.m_pos = m_pos * (1.0f - t) + rhs.m_pos * t;
	result.m_color = m_color * (1.0f - t) + rhs.m_color * t;
	result.m_texCoord = m_texCoord * (1.0f - t) + rhs.m_texCoord * t;

	return result;
}

}