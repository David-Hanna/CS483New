//----------------------------------------------------------------------------
// ComponentHudLapCount.h
// Author: Bradley Cooper
//
// Component that handles player lap count.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_LAP_COUNT_H
#define COMPONENT_HUD_LAP_COUNT_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "TextBox.h"

namespace Kartaclysm
{
	class ComponentHudLapCount : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_LapCount"; }

		virtual ~ComponentHudLapCount();

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
		ComponentHudLapCount(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath,
			float p_fLabelOffset
			);

		virtual void LapCountCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			float& p_fLabelOffset
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		HeatStroke::Font m_mFont;
		HeatStroke::TextBox m_mLabelTextBox;
		HeatStroke::TextBox m_mLapTextBox;

		std::string m_strEventName;

		int m_iLap;
		float m_fLabelOffset;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_LAP_COUNT_H