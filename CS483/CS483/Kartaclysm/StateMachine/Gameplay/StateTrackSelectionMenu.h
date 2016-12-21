//------------------------------------------------------------------------
// StateTrackSelectionMenu
// Author:	David Hanna
//	
// Allows player one to select a track to race on.
// Continues to racing state.
//------------------------------------------------------------------------

#ifndef STATE_TRACK_SELECTION_MENU_H
#define STATE_TRACK_SELECTION_MENU_H

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentTextbox.h"

#include "GameplayState.h"
#include "GameplayStatesEnum.h"
#include "EventManager.h"
#include "KeyboardInputBuffer.h"

namespace Kartaclysm
{
	class StateTrackSelectionMenu : public Kartaclysm::GameplayState
	{
	public:
		StateTrackSelectionMenu();
		virtual ~StateTrackSelectionMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;
		int m_iTrackSelection;
		HeatStroke::GameObject* m_pCurrentHighlight;

		// saved from player selection state to add to and pass on to racing state.
		std::map<std::string, std::string> m_mContextParameters; 
	};
}

#endif

