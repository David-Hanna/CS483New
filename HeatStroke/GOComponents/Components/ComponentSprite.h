//------------------------------------------------------------------------
// ComponentSprite
// Author:	David Hanna
//	
// Provides 3D rendering of a .obj model to the screen for a GameObject.
//------------------------------------------------------------------------

#ifndef COMPONENT_SPRITE_H
#define COMPONENT_SPRITE_H

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ComponentSprite : public ComponentRenderable
	{
	public:
		virtual const std::string ComponentID() const override { return std::string("GOC_Sprite"); }

		virtual ~ComponentSprite();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		virtual void Init() override						{}
		virtual void Update(const float p_fDelta) override	{}

	protected:
		ComponentSprite(GameObject* p_pGameObject, const std::string& p_strMTLFileName, const std::string& p_strMaterialName);

		virtual void SyncTransform();

	private:
		SpriteInstance m_mSpriteInstance;

		// Prevent copying
		ComponentSprite(const ComponentSprite&) = delete;
		ComponentSprite& operator=(const ComponentSprite&) = delete;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strMTLFileName,
			std::string& p_strMaterialName
		);
	};
}

#endif