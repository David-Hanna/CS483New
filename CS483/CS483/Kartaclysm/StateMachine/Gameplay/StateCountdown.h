//------------------------------------------------------------------------
// StateCountdown
// Author:	Bradley Cooper
//	
// Gameplay state for the countdown at start of races.
//------------------------------------------------------------------------

#ifndef STATE_COUNTDOWN_H
#define STATE_COUNTDOWN_H

#include <functional>

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"

namespace Kartaclysm
{
	class StateCountdown : public Kartaclysm::GameplayState
	{
	public:
		StateCountdown();
		virtual ~StateCountdown();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState) { m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState) { m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender() {}
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		int m_iPlayerCount;
		float m_fTimer;

		std::map<int, bool> m_vGainsBoost;
		std::map<int, bool> m_vAccelerating;
	};
}

#endif

