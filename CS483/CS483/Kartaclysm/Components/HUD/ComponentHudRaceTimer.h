//----------------------------------------------------------------------------
// ComponentHudRaceTimer.h
// Author: Bradley Cooper
//
// Component that handles an ability icon on a player's HUD.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_RACE_TIMER_H
#define COMPONENT_HUD_RACE_TIMER_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "TextBox.h"

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

		virtual void Init() override {}
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
			float p_fWidth,
			float p_fHeight,
			float p_fLabelOffset
			);

		virtual void RaceTimerCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			float& p_fWidth,
			float& p_fHeight,
			float& p_fLabelOffset
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		HeatStroke::Font m_mFont;
		HeatStroke::TextBox m_mLabelTextBox;
		HeatStroke::TextBox m_mTimerTextBox;

		bool m_bPaused;
		std::string m_strEventName;

		float m_fTime;
		float m_fLabelOffset;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_RACE_TIMER