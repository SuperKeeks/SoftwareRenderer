//
//  MD2.h
//  SoftwareRenderer
//
//  Created by Enrique Alcor Martín on 21/6/15.
//  Copyright (c) 2015 Enrique Alcor Martín. All rights reserved.
//
//  Resources used:
//    -Wikipedia: https://en.wikipedia.org/wiki/MD2_(file_format)
//    -MD2 file format: http://tfc.duke.free.fr/coding/md2-specs-en.html
//

#pragma once

#include "Vector3.h"

#include <stdint.h>
#include <vector>

namespace omb
{

struct MD2Header
{
	int32_t m_ident;           // Magic number. Should be "IDP2"
	int32_t m_version;         // MD2 version. Should be 8
	
	int32_t m_textureWidth;    // Texture width
	int32_t m_textureHeight;   // Texture height
	
	int32_t m_frameSize;       // Size of each frame in bytes
	
	int32_t m_numTextures;     // Number of textures
	int32_t m_numVertices;     // Number of vertices
	int32_t m_numTexCoords;    // Number of texture coordinates
	int32_t m_numTriangles;    // Number of triangles
	int32_t m_numGLCmds;       // Number of OpenGL commands
	int32_t m_numFrames;       // Number of frames
	
	int32_t m_offsetTextures;  // Offset to texture names
	int32_t m_offsetTexCoords; // Offset to texture coordinates
	int32_t m_offsetTriangles; // Offset to triangles
	int32_t m_offsetFrames;    // Offset to frames
	int32_t m_offsetGLCmds;    // Offset to OpenGL commands
	int32_t m_offsetEnd;       // Offset to end of file
};
	
struct MD2TextureName
{
	char m_name[64];
};
	
struct MD2TexCoord
{
	// MD2 texture coordinates come as short values, as opposed as floats (used by OpenGL and this software renderer)
	// In order to get the OpenGL equivalent value, 's' needs to be divided by textureWidth and 't' by textureHeight
	short m_s;
	short m_t;
};
	
struct MD2Triangle
{
	unsigned short m_vertexIdxs[3];
	unsigned short m_texCoordIdxs[3];
};
	
struct MD2Vertex
{
	unsigned char m_position[3];
	unsigned char m_normalIndex;
};
	
struct MD2Frame
{
	Vector3f m_scale;
	Vector3f m_translate;
	char m_name[16];
	std::vector<MD2Vertex> m_vertices;
};

struct MD2Model
{
	MD2Header m_header;
	
	std::vector<MD2TextureName> m_textures;
	std::vector<MD2TexCoord> m_texCoords;
	std::vector<MD2Triangle> m_triangles;
	std::vector<MD2Frame> m_frames;
	std::vector<int> m_glCmds;
};

namespace MD2Utils
{
	// Loads a MD2 model. It is responsability of the caller to release the memory.
	// Returns nullptr if the model couldn't be loaded
	MD2Model* LoadModel(const char* filePath);
}

}
