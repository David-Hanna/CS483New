//------------------------------------------------------------------------
// StatePlayerSelectionMenu
// Author:	David Hanna
//	
// Allows player to select their character and kart.
// Allows new players to join the game.
// Continues to the track selection state.
//------------------------------------------------------------------------

#ifndef STATE_PLAYER_SELECTION_MENU_H
#define STATE_PLAYER_SELECTION_MENU_H

#include "GameplayState.h"
#include "GameplayStatesEnum.h"
#include "EventManager.h"
#include "KeyboardInputBuffer.h"

namespace Kartaclysm
{
	class StatePlayerSelectionMenu : public Kartaclysm::GameplayState
	{
	public:
		StatePlayerSelectionMenu();
		virtual ~StatePlayerSelectionMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;
	};
}

#endif

