//
//  MD2.cpp
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 21/6/15.
//  Copyright (c) 2015 Enrique Alcor Martín. All rights reserved.
//

#include "MD2.h"

#include "Assert.h"

#include <stdio.h>

namespace omb
{

MD2Model* MD2Utils::LoadModel(const char* filePath)
{
	MD2Model* model = nullptr;
	
	FILE* file = fopen(filePath, "r");
	if (file != NULL)
	{
		model = new MD2Model();
		
		fread(&model->m_header, sizeof(MD2Header), 1, file);
		
		OMBAssert(model->m_header.m_ident == 844121161, "Unexpected magic number");
		OMBAssert(model->m_header.m_version == 8, "Unexpected MD2 version");
		
		fclose(file);
	}
	
	return model;
}

}