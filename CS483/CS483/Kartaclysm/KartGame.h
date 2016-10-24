//----------------------------------------------------------------------------
// KartGame.h
// Author: David MacIntosh
//
// Class that handles core game logic.
//----------------------------------------------------------------------------

#ifndef KARTGAME_H
#define KARTGAME_H

#include "Game.h"

#include "Game.h"
#include "KeyboardInputBuffer.h"
#include "StateMachine.h"
#include "GameplayState.h"
#include "StateRacing.h"

#include "LineDrawer.h"

namespace Kartaclysm
{
	class KartGame : public HeatStroke::Game
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------

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

		HeatStroke::LineDrawer *lineDrawer;

		// State machine to hold game states
		HeatStroke::StateMachine* m_pGameStates;
	};
} // namespace Kartaclysm

#endif // KART_GAME_H
