//-----------------------------------------------------------------------------
// File:			VertexBuffer.h
// Original Author:	Gordon Wood
//
// Derived class from HeatStroke::Buffer, this one describing specifically a vertex
// buffer
//-----------------------------------------------------------------------------
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "Types.h"
#include "Buffer.h"

namespace HeatStroke
{
class VertexBuffer : public Buffer
{
	friend class BufferManager;

	public:
		virtual unsigned int GetLength() { return m_uiLength; }
		virtual void Bind();
		virtual void WriteBufferData(const void* p_pData, int p_iLength = -1, GLenum p_eUsage = GL_STATIC_DRAW);
		virtual void UpdateBufferData(const void* p_pData, int p_iOffset, int p_iLength);

	private:
		unsigned int		m_uiLength;
		GLuint				m_uiBuffer;

		// Made private to enforce creation and deletion via BufferManager
		VertexBuffer(unsigned int p_uiLength);
		VertexBuffer(const void* p_pData, unsigned int p_uiLength);
		virtual ~VertexBuffer();
};

}

#endif
