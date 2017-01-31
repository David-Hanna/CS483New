//------------------------------------------------------------------------
// StatePaused
// Author:	Bradley Cooper
//	
// Gameplay state for pausing during a race.
//------------------------------------------------------------------------

#ifndef STATE_PAUSED_H
#define STATE_PAUSED_H

#include <functional>
#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentTextbox.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"

namespace Kartaclysm
{
	class StatePaused : public Kartaclysm::GameplayState
	{
	public:
		StatePaused();
		virtual ~StatePaused();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState);
		void Unsuspend(const int p_iPrevState);
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		int m_iPausedPlayer;
		int m_iOptionSelection;
		HeatStroke::GameObject* m_pCurrentHighlight;
		bool m_bSkipFirstFrame;
	};
}

#endif

