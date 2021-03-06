//----------------------------------------------------------------------------
// ComponentHudFps.h
// Author: Bradley Cooper
//
// Component that displays Frames per Second (FPS) on the HUD.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_FPS_H
#define COMPONENT_HUD_FPS_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "TextBox.h"
#include <vector>
#include <iomanip>
#include <sstream>

namespace Kartaclysm
{
	class ComponentHudFps : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_Fps"; }

		virtual ~ComponentHudFps();

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
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHudFps(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath
			);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		float m_fFPS;
		float m_fTimeSinceLastShown;
		int m_iFrameCounter;
		std::vector<float> m_lFrameSpeeds;

		HeatStroke::Font* m_pFont;
		HeatStroke::TextBox m_mTextBox;
	};
}

#endif // COMPONENT_HUD_FPS_H