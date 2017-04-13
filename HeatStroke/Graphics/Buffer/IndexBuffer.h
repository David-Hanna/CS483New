//-----------------------------------------------------------------------------
// File:			IndexBuffer.h
// Original Author:	Gordon Wood
//
// Derived class from HeatStroke::Buffer, this one describing specifically a index
// buffer
//-----------------------------------------------------------------------------
#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "Types.h"
#include "Buffer.h"

namespace HeatStroke
{
class IndexBuffer : public Buffer
{
	friend class BufferManager;


	public:
		virtual int GetNumIndices() const { return m_uiLength / sizeof(GLushort); }
		virtual void Bind();
		virtual void Write(const void* p_pData, int p_iLength = -1, GLenum p_eUsage = GL_STATIC_DRAW);
		virtual void Update(const void* p_pData, int p_iOffset, int p_iLength);

	private:
		unsigned int		m_uiLength;
		GLuint				m_uiBuffer;

		// Made private to enforce creation and deletion via BufferManager
		IndexBuffer(unsigned int p_uiNumIndices);
		IndexBuffer(const void* p_pData, unsigned int p_uiLength);
		virtual ~IndexBuffer();
};

}

#endif

