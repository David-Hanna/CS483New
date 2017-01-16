//------------------------------------------------------------------------
// GameplayState
// Author:	Bradley Cooper
//	
// Base class for any gameplay state, extending from a regular state.
//------------------------------------------------------------------------

#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include "State.h"
#include "GameObjectManager.h"
#include "StateMachine.h"

namespace Kartaclysm
{
	class GameplayState : public HeatStroke::State
	{
	public:
		enum GameplayStates
		{
			STATE_MAIN_MENU = 0,
			STATE_PLAYER_SELECTION_MENU,
			STATE_TRACK_SELECTION_MENU,
			STATE_RACING,
			STATE_PAUSED,
			STATE_RACE_COMPLETE_MENU,
			STATE_OPTIONS_MENU,
			STATE_COUNTDOWN
		};

	public:
		GameplayState(const std::string& p_strStateName = "Gameplay State") : State(p_strStateName) {}
		virtual ~GameplayState() {}

		// Inherited
		virtual void Enter(const std::map<std::string, std::string>& p_mContextParameters) = 0;
		virtual void Suspend(const int p_iNewState) = 0;
		virtual void Unsuspend(const int p_iPrevState) = 0;
		virtual void Update(const float p_fDelta) = 0;
		virtual void Exit() = 0;

		// Required interface for all subclasses
		virtual void PreRender() = 0;

	protected:
		// Game Object Manager for this state
		HeatStroke::GameObjectManager* m_pGameObjectManager;

		// Update and render based on suspension status
		bool m_bSuspended;
	};
} // namespace Kartaclysm

#endif

