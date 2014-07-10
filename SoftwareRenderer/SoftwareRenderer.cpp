//
//  SoftwareRenderer.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 06/07/14.
//  Copyright (c) 2014 Enrique Alcor Martín. All rights reserved.
//

#include "SoftwareRenderer.h"

#include "Utils.h"

namespace omb
{

	SoftwareRenderer::SoftwareRenderer()
	{
	}
	
	SoftwareRenderer::~SoftwareRenderer()
	{
	}
	
	void SoftwareRenderer::init(const int width, const int height)
	{
		m_width = width;
		m_height = height;
		m_initialised = true;
	}
	
	void SoftwareRenderer::clear(const Color& clearColor)
	{
		OMBAssert(m_initialised, "Not initialised!");
	
		for (int i = 0; i < m_width * m_height * 4; ++i)
		{
			if (i % 4 == 0)
			{
				m_frameBuffer[i] = clearColor.m_r;
			}
			else if (i % 4 == 1)
			{
				m_frameBuffer[i] = clearColor.m_g;
			}
			else if (i % 4 == 2)
			{
				m_frameBuffer[i] = clearColor.m_b;
			}
			else if (i % 4 == 3)
			{
				m_frameBuffer[i] = clearColor.m_a;
			}
		}
	}

}