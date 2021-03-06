//----------------------------------------------------------------------------
// ComponentHudRaceTimer.h
// Author: Bradley Cooper
//
// Component that displays race time on the HUD.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_RACE_TIMER_H
#define COMPONENT_HUD_RACE_TIMER_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "TextBox.h"
#include "TimeFormat.h"

namespace Kartaclysm
{
	class ComponentHudRaceTimer : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_RaceTimer"; }

		virtual ~ComponentHudRaceTimer();

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
		// Protected types
		//--------------------------------------------------------------------------
		typedef std::function<void(const HeatStroke::Event*)>* EventDelegate;

		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHudRaceTimer(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath,
			float p_fLabelOffset
			);

		virtual void LapCountCallback(const HeatStroke::Event* p_pEvent);
		virtual void FlashLapTimer(const float p_fLapDisplayTimer, const float p_fInterval);
		virtual void DisableLapFlash();

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			float& p_fLabelOffset
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		HeatStroke::Font* m_pFont;
		HeatStroke::TextBox m_LabelTextBox;
		HeatStroke::TextBox m_TimerTextBox;

		std::string m_strEventName;

		float m_fTime;
		float m_fLapDisplayTimer;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_RACE_TIMER_H