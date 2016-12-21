//----------------------------------------------------------------------------
// KartGame.h
// Author: David MacIntosh
//
// Class that handles core game logic.
//----------------------------------------------------------------------------

#ifndef KARTGAME_H
#define KARTGAME_H

#include "Game.h"
#include "KeyboardInputBuffer.h"
#include "JoystickInputBuffer.h"
#include "InputActionMapping.h"
#include "PlayerInputMapping.h"
#include "EventManager.h"
#include "StateMachine.h"
#include "GameplayState.h"
#include "StateRacing.h"
#include "StatePaused.h"
#include "StateMainMenu.h"
#include "StateTrackSelect.h"
#include "SceneManager.h"
#include "AudioPlayer.h"

namespace Kartaclysm
{
	class KartGame : public HeatStroke::Game
	{
	public:
		KartGame() {}
		~KartGame() {}
		
	private:
		HeatStroke::StateMachine* m_pGameStates;

		bool Init();
		void Update(const float p_fDelta);
		void PreRender();
		void Render();
		void Shutdown();
	};
}

#endif
