//-----------------------------------------------------------------------------
// File:			BufferManager.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------
#include "BufferManager.h"

namespace HeatStroke
{
	//----------------------------------------------------------
	// Creates a new Vertex Buffer
	//----------------------------------------------------------
	VertexBuffer* BufferManager::CreateVertexBuffer(unsigned int p_uiLength)
	{
		return new VertexBuffer(p_uiLength);
	}

	//----------------------------------------------------------
	// Creates a new Vertex Buffer
	//----------------------------------------------------------
	VertexBuffer* BufferManager::CreateVertexBuffer(const void* p_pData, unsigned int p_uiLength)
	{
		return new VertexBuffer(p_pData, p_uiLength);
	}

	//----------------------------------------------------------
	// Creates a new Index Buffer
	//----------------------------------------------------------
	IndexBuffer* BufferManager::CreateIndexBuffer(unsigned int p_uiNumIndices)
	{
		return new IndexBuffer(p_uiNumIndices);
	}

	//----------------------------------------------------------
	// Creates a new Index Buffer
	//----------------------------------------------------------
	IndexBuffer* BufferManager::CreateIndexBuffer(const void* p_pData, unsigned int p_uiNumIndices)
	{
		return new IndexBuffer(p_pData, p_uiNumIndices);
	}

	//----------------------------------------------------------
	// Destroys a buffer. 
	//----------------------------------------------------------
	void BufferManager::DestroyBuffer(Buffer* p_pBuf)
	{
		DELETE_IF(p_pBuf);
	}
}




