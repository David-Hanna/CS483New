//------------------------------------------------------------------------
// StateOptionsMenu
// Author:	Bradley Cooper
//	
// Allows player to change and save game options.
// Accessed by and returns to main menu or pause menu.
// Passing OptionsXML parameter loads options from XML and calls Pop().
//------------------------------------------------------------------------

#ifndef STATE_OPTIONS_MENU_H
#define STATE_OPTIONS_MENU_H

#include "EasyXML.h"
#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentTextbox.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentMenuSlider.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "AudioPlayer.h"

namespace Kartaclysm
{
	class StateOptionsMenu : public Kartaclysm::GameplayState
	{
	public:
		StateOptionsMenu();
		virtual ~StateOptionsMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		void LoadOptionsFromXml(const std::string& p_strXmlFilePath);

		std::string m_strXmlFilePath;
		int m_iPlayer;
		int m_iOptionSelection;
		HeatStroke::GameObject* m_pCurrentHighlight;

		ComponentMenuSlider* m_pCurrentSlider;
	};
}

#endif

