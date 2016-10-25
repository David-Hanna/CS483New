//----------------------------------------------------------------------------
// KartGame.h
// Author: David MacIntosh
//
// Class that handles core game logic.
//----------------------------------------------------------------------------

#ifndef KART_GAME_H
#define KART_GAME_H

#include "Game.h"
#include "KeyboardInputBuffer.h"
#include "StateMachine.h"
#include "GameplayState.h"
#include "StateRacing.h"
#include "SceneManager.h"

namespace Kartaclysm
{
	class KartGame : public HeatStroke::Game
	{
	public:
		KartGame() {}
		~KartGame() {}
		
	private:
		//--------------------------------------------------------------------------
		// Private methods
		//--------------------------------------------------------------------------
		
		bool Init();
		void Update(const float p_fDelta);
		void PreRender();
		void Render();
		void Shutdown();

		//--------------------------------------------------------------------------
		// Private variables
		//--------------------------------------------------------------------------
		// State machine to hold game states
		HeatStroke::StateMachine* m_pGameStates;
	};
} // namespace Kartaclysm

#endif // KART_GAME_H