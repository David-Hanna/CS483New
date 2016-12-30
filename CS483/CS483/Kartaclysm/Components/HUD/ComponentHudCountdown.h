//----------------------------------------------------------------------------
// ComponentHudCountdown.h
// Author: Bradley Cooper
//
// Component that displays beginning race countdown.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_COUNTDOWN_H
#define COMPONENT_HUD_COUNTDOWN_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "TextBox.h"
#include <vector>

namespace Kartaclysm
{
	class ComponentHudCountdown : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_Countdown"; }

		virtual ~ComponentHudCountdown();

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
		ComponentHudCountdown(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath
			);

		virtual void CountdownCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::Font* m_pFont;
		HeatStroke::TextBox m_mTextBox;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_COUNTDOWN_H