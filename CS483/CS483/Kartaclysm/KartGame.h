//----------------------------------------------------------------------------
// KartGame.h
// Author: David MacIntosh
//
// Class the handles core game logic.
//----------------------------------------------------------------------------

#ifndef KARTGAME_H
#define KARTGAME_H

#include "Game.h"
#include "GameObjectManager.h"

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

		bool Init();
		void Update(const float p_fDelta);
		void PreRender();
		void Render();
		void Shutdown();

		//--------------------------------------------------------------------------
		// Public variables
		//--------------------------------------------------------------------------

		HeatStroke::GameObjectManager *p_gameObjectManager;
	};
}

#endif // KARTGAME_H