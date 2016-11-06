//------------------------------------------------------------------------
// StatePaused
// Author:	Bradley Cooper
//	
// Gameplay state for pausing during a race.
//------------------------------------------------------------------------

#ifndef STATE_PAUSED_H
#define STATE_PAUSED_H

#include <functional>

#include "GameplayState.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class StatePaused : public Kartaclysm::GameplayState
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		StatePaused();
		virtual ~StatePaused();

		// Inherited
		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState);
		void Unsuspend(const int p_iPrevState);
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		// Inherited
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		int m_iPausedPlayer;
		std::function<void(const HeatStroke::Event*)>* m_pPauseDelegate;

		void UnpauseGame(const HeatStroke::Event* p_pEvent);
	};
} // namespace Kartaclysm

#endif

