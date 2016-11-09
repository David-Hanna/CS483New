//-----------------------------------------------------------------------------
// Sprite
// Author: David Hanna
//
// Renderable 2D textured quad.
//-----------------------------------------------------------------------------

#ifndef SPRITE_H
#define SPRITE_H

#include "Mesh.h"
#include "MTLFile.h"

#include <string>

namespace HeatStroke
{
	class Sprite : public Renderable
	{
	public:
		Sprite(const std::string& p_strMTLFileName, const std::string& p_strMaterialName);
		~Sprite();

		void		 Update(float p_fDelta);
		virtual void Render(const SceneCamera* p_pCamera = nullptr) override;

		Mesh& GetMesh()										   { return m_mMesh; }
		void  SetTransform(const glm::mat4& p_mWorldTransform) { m_mWorldTransform = p_mWorldTransform; }

	private:
		static float		  m_vQuadVertex[];
		static unsigned short m_vQuadIndex[];

		Mesh		m_mMesh;
		glm::mat4	m_mWorldTransform;
	};
}

#endif