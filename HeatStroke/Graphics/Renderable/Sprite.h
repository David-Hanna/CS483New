//-----------------------------------------------------------------------------
// File:			Sprite.h
// Original Author:	Carel Boers
//
// Class representing a 2D textured quad.
//
//-----------------------------------------------------------------------------
#ifndef W_SPRITE_H
#define W_SPRITE_H

#include "Types.h"
#include "Renderable.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BufferManager.h"
#include "Material.h"
#include "VertexDeclaration.h"
#include "TextureManager.h"
#include "ProgramManager.h"
#include <string>
#include <map>
#include <vector>

namespace HeatStroke
{
class Sprite : public Renderable
{
	public:
		//-------------------------------------------------------------------------
		// PUBLIC INTERFACE
		//-------------------------------------------------------------------------
		Sprite(const std::string& p_strTexture, const std::string& p_strVertexProgram, const std::string& p_strFragmentProgram, const glm::vec2& p_vDimensions);
		~Sprite();

		void Update(float p_fDelta);
		virtual void Render(const SceneCamera* p_pCamera = nullptr) override;

		void SetTransform(const glm::mat4& p_mWorldTransform) { m_mWorldTransform = p_mWorldTransform; }
		//-------------------------------------------------------------------------

	private:
		//-------------------------------------------------------------------------
		// PRIVATE MEMBERS
		//-------------------------------------------------------------------------

		HeatStroke::VertexBuffer*			m_pVB;
		HeatStroke::VertexDeclaration*	m_pDecl;
		HeatStroke::Program*				m_pProgram;
		GLuint						m_uiTex;

		glm::mat4					m_mWorldTransform;

		//-------------------------------------------------------------------------
};

}

#endif



