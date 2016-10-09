//-----------------------------------------------------------------------------
// File:			VertexDeclaration.h
// Original Author:	Gordon Wood
//
// Class representing a complete vertex declaration, including all the
// attributes that comprise the vertex, and the associated source buffers
//-----------------------------------------------------------------------------

#ifndef VERTEXDECLARATION_H
#define VERTEXDECLARATION_H

#include <vector>
#include "Types.h"

namespace HeatStroke
{
class VertexBuffer;
class IndexBuffer;

class VertexDeclaration
{
	public:
		VertexDeclaration();
		~VertexDeclaration();
	
		void Begin();
		void SetVertexBuffer(HeatStroke::VertexBuffer* p_pVB);
		void SetIndexBuffer(HeatStroke::IndexBuffer* p_pIB);
		void AppendAttribute(HeatStroke::Attribute p_attr, int p_iNumComponents, HeatStroke::ComponentType p_type, int p_iOffset = -1);
		void End();

		void Bind();

	private:
		struct AttributeInfo
		{
			HeatStroke::Attribute		m_attr;
			HeatStroke::ComponentType	m_type;
			int					m_iOffset;
			int					m_iNumComponents;
		};

		std::vector<AttributeInfo>	m_attrs;
		HeatStroke::VertexBuffer*			m_pVB;
		HeatStroke::IndexBuffer*			m_pIB;
		GLuint						m_vao;
};

}

#endif

