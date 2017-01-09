//------------------------------------------------------------------------
// StateMainMenu
// Author:	David Hanna
//	
// Main menu state shows title screen and continues with enter key.
//------------------------------------------------------------------------

#ifndef STATE_MAIN_MENU_H
#define STATE_MAIN_MENU_H

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "AudioPlayer.h"
#include "KeyboardInputBuffer.h"

namespace Kartaclysm
{
	class StateMainMenu : public Kartaclysm::GameplayState
	{
	public:
		StateMainMenu();
		virtual ~StateMainMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;
	};
}

#endif

