//----------------------------------------------------------------------------
// ComponentHudPopup.h
// Author: Bradley Cooper
//
// Component to display a textbox which can be toggled on or off by event.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_POPUP_H
#define COMPONENT_HUD_POPUP_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentHudPopup : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_Popup"; }

		virtual ~ComponentHudPopup();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;
		virtual void SyncTransform() override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHudPopup(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strMTLFileName,
			const std::string& p_strMaterialName,
			const std::string& p_strEventName,
			float p_fDelay
			);

		virtual void ToggleCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strMTLFileName,
			std::string& p_strMaterialName,
			std::string& p_strEventName,
			float& p_fDelay
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		std::string m_strPositionFilePrefix;
		HeatStroke::SpriteInstance m_mSprite;
		std::string m_strEventName;

		float m_fDelay;
		float m_fWaitingToRender;
		bool m_bDisplaying;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_POPUP_H