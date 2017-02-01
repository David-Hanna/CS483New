//------------------------------------------------------------------------
// StateModeSelectionMenu
// Author:	Bradley Cooper
//	
// Allows player one to select racing mode or options menu.
//------------------------------------------------------------------------

#ifndef STATE_MODE_SELECTION_MENU_H
#define STATE_MODE_SELECTION_MENU_H

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentTextbox.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "AudioPlayer.h"

namespace Kartaclysm
{
	class StateModeSelectionMenu : public Kartaclysm::GameplayState
	{
	public:
		StateModeSelectionMenu();
		virtual ~StateModeSelectionMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;
		int m_iModeSelection;
		HeatStroke::GameObject* m_pCurrentHighlight;

	private:

	};
}

#endif

