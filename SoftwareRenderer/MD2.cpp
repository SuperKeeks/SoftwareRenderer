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
	
	FILE* file;
	file = fopen(filePath, "r");
	if (file != NULL)
	{
		model = new MD2Model();
		
		fread(&model->m_header, sizeof(MD2Header), 1, file);
		
		OMBAssert(model->m_header.m_ident == 844121161, "Unexpected magic number");
		OMBAssert(model->m_header.m_version == 8, "Unexpected MD2 version");
		
		// Allocate space in std vectors
		model->m_textures.resize(model->m_header.m_numTextures);
		model->m_texCoords.resize(model->m_header.m_numTexCoords);
		model->m_triangles.resize(model->m_header.m_numTriangles);
		model->m_frames.resize(model->m_header.m_numFrames);
		model->m_glCmds.resize(model->m_header.m_numGLCmds);
		
		// Read all the data
		fseek(file, model->m_header.m_offsetTextures, SEEK_SET);
		fread(model->m_textures.data(), sizeof(MD2TextureName), model->m_header.m_numTextures, file);
		
		fseek(file, model->m_header.m_offsetTexCoords, SEEK_SET);
		fread(model->m_texCoords.data(), sizeof(MD2TexCoord), model->m_header.m_numTexCoords, file);
		
		fseek(file, model->m_header.m_offsetTriangles, SEEK_SET);
		fread(model->m_triangles.data(), sizeof(MD2Triangle), model->m_header.m_numTriangles, file);

		fseek(file, model->m_header.m_offsetFrames, SEEK_SET);
		for (int i = 0; i < model->m_header.m_numFrames; ++i)
		{
			MD2Frame& frame = model->m_frames[i];
			
			// Allocate space for vertices
			frame.m_vertices.resize(model->m_header.m_numVertices);
			
			fread(&frame, sizeof(MD2Frame) - sizeof(std::vector<MD2Vertex>), 1, file);
			fread(frame.m_vertices.data(), sizeof(MD2Vertex), model->m_header.m_numVertices, file);
		}
		
		fseek(file, model->m_header.m_offsetGLCmds, SEEK_SET);
		fread(model->m_glCmds.data(), sizeof(int), model->m_header.m_numGLCmds, file);		
		
		fclose(file);
	}
	
	return model;
}

}