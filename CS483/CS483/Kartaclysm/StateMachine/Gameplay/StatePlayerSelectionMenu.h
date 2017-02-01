//------------------------------------------------------------------------
// StatePlayerSelectionMenu
// Author:	David Hanna
//	
// Allows player to select their character and kart.
// Allows new players to join the game.
// Continues to the track selection state.
//------------------------------------------------------------------------

#ifndef STATE_PLAYER_SELECTION_MENU_H
#define STATE_PLAYER_SELECTION_MENU_H

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentTextbox.h"
#include "ComponentPerspectiveCamera.h"
#include "Component3DModel.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"

namespace Kartaclysm
{
	class StatePlayerSelectionMenu : public Kartaclysm::GameplayState
	{
	public:
		StatePlayerSelectionMenu();
		virtual ~StatePlayerSelectionMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		enum DRIVER_SELECTION
		{
			DRIVER_CLEOPAPA,
			DRIVER_CLOCKMAKER,
			DRIVER_KINGPIN
		};

		enum KART_SELECTION
		{
			KART_JUGGERNAUT,
			KART_SHOWOFF,
			KART_SPEEDSTER
		};

		typedef struct PerPlayerMenuState
		{
			bool					bJoined;
			bool					bReady;
			bool					bDriverHighlighted;

			DRIVER_SELECTION		eSelectedDriver;
			KART_SELECTION			eSelectedKart;

			HeatStroke::GameObject* pPressStartToJoin;
			HeatStroke::GameObject* pDriverSelection;
			HeatStroke::GameObject* pKartSelection;
			HeatStroke::GameObject* pReadyButton;
			HeatStroke::GameObject* pDescriptionBoxes;
			HeatStroke::GameObject* pDriverAbilities;
			HeatStroke::GameObject* pKartAbilities;
			HeatStroke::GameObject* pStats;
			HeatStroke::GameObject* pDriverDisplay;
			HeatStroke::GameObject* pKartDisplay;
			HeatStroke::GameObject* pHighlight;
		};

		unsigned int m_uiNumPlayers;
		PerPlayerMenuState m_mPerPlayerMenuState[4];

		void AddPlayer(const unsigned int m_uiPlayerNum);
		void GoToTrackSelectionState();
	};
}

#endif

