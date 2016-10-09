//-----------------------------------------------------------------------------
// File:			IndexBuffer.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------

#include "IndexBuffer.h"
#include "Common.h"

namespace HeatStroke
{
//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
IndexBuffer::IndexBuffer(unsigned int p_uiNumIndices) : m_uiLength(p_uiNumIndices*2)
{
	glGenBuffers(1, &m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Destructor
//----------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1,&m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Fills this Index buffer with the given data
//----------------------------------------------------------
void IndexBuffer::Write(const void* p_pData, int p_iLength)
{
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_iLength == -1 ? m_uiLength : p_iLength, p_pData, GL_STATIC_DRAW);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Binds this buffer
//----------------------------------------------------------
void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

}

