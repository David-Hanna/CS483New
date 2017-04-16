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
#include "RNGesus.h"

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

		void SendKartDisableEvent();
		void DetermineAIBoost(unsigned int p_uiNumAIRacers);
		void UpdateHumanBoostValues();
		void DetermineHumanBoost(bool* p_pCurrentBoostValue, const bool p_bAcceleratingThisFrame, const bool p_bAcceleratingLastFrame);
		void SendHudCountdownEvent(const float p_fDelta) const;
		void SendCountdownBoostEvent() const;

	private:
		unsigned int m_uiNumHumanRacers;
		float m_fTimer;

		std::map<std::string, bool> m_mRacerBoostMap;
		std::map<int, bool> m_mPreviousFrameAccelerating;
	};
}

#endif

