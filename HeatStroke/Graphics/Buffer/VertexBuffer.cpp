//-----------------------------------------------------------------------------
// File:			VertexBuffer.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------
#include "VertexBuffer.h"
#include "Common.h"

namespace HeatStroke
{
//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
VertexBuffer::VertexBuffer(unsigned int p_uiLength) : m_uiLength(p_uiLength)
{
	glGenBuffers(1, &m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	Write(nullptr, m_uiLength);
}

//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
VertexBuffer::VertexBuffer(const void* p_pData, unsigned int p_uiLength) : m_uiLength(p_uiLength)
{
	glGenBuffers(1, &m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	Write(p_pData);
}

//----------------------------------------------------------
// Destructor
//----------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1,&m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Fills this vertex buffer with the given data
//----------------------------------------------------------
void VertexBuffer::Write(const void* p_pData, int p_iLength, GLenum p_eUsage /*= GL_STATIC_DRAW*/)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, p_iLength == -1 ? m_uiLength : p_iLength, p_pData, p_eUsage);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Update buffer with given data without reallocating storage
//----------------------------------------------------------
void VertexBuffer::Update(const void* p_pData, int p_iOffset, int p_iLength)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, p_iOffset, p_iLength, p_pData);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Binds this buffer
//----------------------------------------------------------
void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

}
