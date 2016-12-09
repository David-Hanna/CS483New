//------------------------------------------------------------------------
// StateMainMenu
// Author:	Bradley Cooper
//	
// Gameplay state first viewed when the game loads.
//------------------------------------------------------------------------

#ifndef STATE_MAIN_MENU_H
#define STATE_MAIN_MENU_H

#include <functional>

#include "GameplayState.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class StateMainMenu : public Kartaclysm::GameplayState
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		StateMainMenu();
		virtual ~StateMainMenu();

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
	};
} // namespace Kartaclysm

#endif

