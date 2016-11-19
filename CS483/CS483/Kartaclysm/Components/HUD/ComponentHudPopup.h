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
#include "TextBox.h"

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

		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override {}
		virtual void SyncTransform() override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHudPopup(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath,
			float p_fWidth,
			float p_fHeight,
			const std::string& p_strEventName,
			const std::string& p_strMessage
			);

		virtual void ToggleCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			float& p_fWidth,
			float& p_fHeight,
			std::string& p_strEventName,
			std::string& p_strMessage
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		HeatStroke::Font m_mFont;
		HeatStroke::TextBox m_mTextBox;

		std::string m_strEventName;
		bool m_bDisplaying;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_POPUP_H