//----------------------------------------------------------------------------
// ComponentHudAbility.h
// Author: Bradley Cooper
//
// Component that handles an ability icon on a player's HUD.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_ABILITY_H
#define COMPONENT_HUD_ABILITY_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "ComponentSprite.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentHudAbility : public HeatStroke::ComponentSprite
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_Ability"; }

		virtual ~ComponentHudAbility();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override {}

	protected:

		//--------------------------------------------------------------------------
		// Protected types
		//--------------------------------------------------------------------------
		typedef std::function<void(const HeatStroke::Event*)>* EventDelegate;

		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHudAbility( 
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strMTLFileName,
			const std::string& p_strMaterialName
			);

		virtual void SyncTransform() { HeatStroke::ComponentSprite::SyncTransform(); }

		virtual void AbilityCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strOBJFileName,
			std::string& p_strMaterialName
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		// Information for creating textboxes
		// HeatStroke::Font*	m_pFont;
		// std::string			m_strVertexProgramPath;
		// std::string			m_strFragmentProgramPath;
	};
}

#endif // COMPONENT_HUD_H