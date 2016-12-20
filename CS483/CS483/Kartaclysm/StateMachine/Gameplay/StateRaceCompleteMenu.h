//------------------------------------------------------------------------
// StateRaceCompleteMenu
// Author:	David Hanna
//	
// Shows the results of the race and continues to the Main Menu state.
//------------------------------------------------------------------------

#ifndef STATE_RACE_COMPLETE_MENU_H
#define STATE_RACE_COMPLETE_MENU_H

#include "GameplayState.h"
#include "GameplayStatesEnum.h"
#include "EventManager.h"
#include "KeyboardInputBuffer.h"

namespace Kartaclysm
{
	class StateRaceCompleteMenu : public Kartaclysm::GameplayState
	{
	public:
		StateRaceCompleteMenu();
		virtual ~StateRaceCompleteMenu();

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

