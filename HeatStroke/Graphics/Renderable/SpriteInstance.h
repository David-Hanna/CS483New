//-----------------------------------------------------------------------------
// SpriteInstance
// Author: David Hanna
//
// Adds world transform and render behaviour to a sprite.
//-----------------------------------------------------------------------------

#ifndef SPRITE_INSTANCE_H
#define SPRITE_INSTANCE_H

#include "Renderable.h"
#include "SpriteManager.h"

namespace HeatStroke
{
	class SpriteInstance : public Renderable
	{
	public:
		SpriteInstance(const std::string& p_strMTLFileName, const std::string& p_strMaterialName);
		SpriteInstance(Sprite* p_pSprite);

		virtual void Render(const SceneCamera* p_pCamera = nullptr) override;

		Sprite*		GetSprite()											{ return m_pSprite; }

		void		SetTransform(const glm::mat4& p_mWorldTransform)	{ m_mWorldTransform = p_mWorldTransform; }
		glm::mat4&	GetTransform()										{ return m_mWorldTransform; }

	private:
		Sprite*		 m_pSprite;
		glm::mat4	 m_mWorldTransform;
	};
}

#endif SPRITE_INSTANCE_H