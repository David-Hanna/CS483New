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
#include "SceneManager.h"
#include "EventManager.h"
#include "Sprite.h"

namespace Kartaclysm
{
	class ComponentHudAbility : public HeatStroke::ComponentRenderable
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
		virtual void SyncTransform() override;

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
			const std::string& p_strFontFilePath,
			float p_fWidth,
			float p_fHeight,
			const std::string& p_strMTLFileName,
			const std::string& p_strMaterialName,
			const std::string& p_strAbility
			);

		virtual void AbilityCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			float& p_fWidth,
			float& p_fHeight,
			std::string& p_strMTLFileName,
			std::string& p_strMaterialName,
			std::string& p_strAbility
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		HeatStroke::Font m_mFont;
		HeatStroke::TextBox m_mTextBox;
		HeatStroke::Sprite m_mActiveSprite;
		HeatStroke::Sprite m_mInactiveSprite;

		bool m_bReady;
		bool m_bHasCharges;
		std::string m_strEventName;
		
		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_ABILITY_H