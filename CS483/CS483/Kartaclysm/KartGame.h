//----------------------------------------------------------------------------
// KartGame.h
// Author: David MacIntosh
//
// Class the handles core game logic.
//----------------------------------------------------------------------------

#ifndef KARTGAME_H
#define KARTGAME_H

#include "Game.h"

class KartGame : public HeatStroke::Game
{
	public :
		//--------------------------------------------------------------------------
		// Public methods.
		//--------------------------------------------------------------------------

		KartGame() {}
		~KartGame() {}

		bool Init();
		void Update(const float p_fDelta);
		void PreRender();
		void Render();
		void Shutdown();
};

#endif // KARTGAME_H