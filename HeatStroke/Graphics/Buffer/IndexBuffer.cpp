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
IndexBuffer::IndexBuffer(unsigned int p_uiNumIndices) : m_uiLength(p_uiNumIndices * sizeof(GLushort))
{
	glGenBuffers(1, &m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	WriteBufferData(nullptr, m_uiLength);
}

//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
IndexBuffer::IndexBuffer(const void* p_pData, unsigned int p_uiNumIndices) : m_uiLength(p_uiNumIndices * sizeof(GLushort))
{
	glGenBuffers(1, &m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	WriteBufferData(p_pData);
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
void IndexBuffer::WriteBufferData(const void* p_pData, int p_iLength, GLenum p_eUsage /*= GL_STATIC_DRAW*/)
{
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_iLength == -1 ? m_uiLength : p_iLength, p_pData, p_eUsage);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Update buffer with given data without reallocating storage
//----------------------------------------------------------
void IndexBuffer::UpdateBufferData(const void* p_pData, int p_iOffset, int p_iLength)
{
	Bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, p_iOffset, p_iLength, p_pData);
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

