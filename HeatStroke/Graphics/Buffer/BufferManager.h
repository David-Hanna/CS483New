//-----------------------------------------------------------------------------
// File:			BufferManager.h
// Original Author:	Gordon Wood
//
// Class to manage Buffers. This isn't strictly needed in our use-cases as all
// it does is delegate through to the buffer classes themselves. However, it's 
// provided as an intermediate step. In a future expansion, you may want to
// store a copy of all buffers created so you can later run through them and
// reload them if, say, your context is lost. This is left as an exercise.
//-----------------------------------------------------------------------------
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Common.h"
#include "Types.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <string>
#include <map>

namespace HeatStroke
{
	class BufferManager
	{
	public:
		static VertexBuffer* CreateVertexBuffer(unsigned int p_uiLength);
		static VertexBuffer* CreateVertexBuffer(const void* p_pData, unsigned int p_uiLength);

		static IndexBuffer* CreateIndexBuffer(unsigned int p_uiNumIndices);
		static IndexBuffer* CreateIndexBuffer(const void* p_pData, unsigned int p_uiLength);

		static void DestroyBuffer(Buffer* p_pBuf);
	};
}

#endif




