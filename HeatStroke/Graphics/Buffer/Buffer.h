//-----------------------------------------------------------------------------
// File:			Buffer.h
// Original Author:	Gordon Wood
//
// Base class for a buffer that is used by the rendering API. Concrete
// subclasses will be for an IndexBuffer and a VertexBuffer
//-----------------------------------------------------------------------------
#ifndef BUFFER_H
#define BUFFER_H

namespace HeatStroke
{
class Buffer
{
	friend class BufferManager;
	public:
		//-------------------------------------------------------------------------
		// PUBLIC INTERFACE
		//-------------------------------------------------------------------------
		virtual void Bind() = 0;
		virtual void Write(const void* p_pData, int p_iLength = -1, GLenum p_eUsage = GL_STATIC_DRAW) = 0;
		//-------------------------------------------------------------------------

	protected:
		//-------------------------------------------------------------------------
		// PROTECTED MEMBERS
		//-------------------------------------------------------------------------
		// Made protected to enforce creation and deletion via BufferManager
		Buffer() {}
		virtual ~Buffer() {}
		//-------------------------------------------------------------------------
};

}

#endif
