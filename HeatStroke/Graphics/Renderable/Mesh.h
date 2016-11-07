//-----------------------------------------------------------------------------
// Mesh
// Author: David Hanna
//
// A structure of data necessary to render one 'shape' using the graphics engine.
// Includes files for convenience.
//-----------------------------------------------------------------------------

#ifndef MESH_H
#define MESH_H

#include "Renderable.h"

#include "BufferManager.h"
#include "VertexBuffer.h"

#include "VertexDeclaration.h"

#include "Texture.h"
#include "TextureManager.h"

#include "Material.h"
#include "MaterialManager.h"

namespace HeatStroke
{
	struct Mesh
	{
		VertexBuffer*		m_pVertexBuffer;
		VertexDeclaration*	m_pVertexDeclaration;
		Texture*			m_pTexture;
		Material*			m_pMaterial;
	};
}

#endif