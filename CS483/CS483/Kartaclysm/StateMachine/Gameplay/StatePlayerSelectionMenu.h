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
#include "ComponentRotateOverTime.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "AudioPlayer.h"
#include "RNGesus.h"

namespace Kartaclysm
{
	class StatePlayerSelectionMenu : public Kartaclysm::GameplayState
	{
	public:
		StatePlayerSelectionMenu();
		virtual ~StatePlayerSelectionMenu();

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
		enum CURRENT_SELECTION
		{
			SELECTION_CONTROL,
			SELECTION_DRIVER,
			SELECTION_KART
		};

		enum CONTROL_SELECTION
		{
			CONTROL_HUMAN,
			CONTROL_BOT
		};

		// TODO - should be loading karts/drivers from xml
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
			
			CURRENT_SELECTION		eCurrentSelection;

			CONTROL_SELECTION		eSelectedControl;
			DRIVER_SELECTION		eSelectedDriver;
			KART_SELECTION			eSelectedKart;

			int						iDriverSpeedStat;
			int						iDriverAccelerationStat;
			int						iDriverHandlingStat;
			int						iDriverDurabilityStat;

			int						iKartSpeedStat;
			int						iKartAccelerationStat;
			int						iKartHandlingStat;
			int						iKartDurabilityStat;

			HeatStroke::GameObject* pPressStartToJoin;
			HeatStroke::GameObject* pControlSelection;
			HeatStroke::GameObject* pDriverSelection;
			HeatStroke::GameObject* pKartSelection;
			HeatStroke::GameObject* pReadyButton;
			HeatStroke::GameObject* pDescriptionBoxes;
			HeatStroke::GameObject* pDriverAbilities;
			HeatStroke::GameObject* pKartAbilities;
			HeatStroke::GameObject* pStatDescription;
			HeatStroke::GameObject* pSpeedStatIcon;
			HeatStroke::GameObject* pAccelerationStatIcon;
			HeatStroke::GameObject* pHandlingStatIcon;
			HeatStroke::GameObject* pDurabilityStatIcon;
			HeatStroke::GameObject* pDriverKartDisplayConnector;
			HeatStroke::GameObject* pDriverDisplay;
			HeatStroke::GameObject* pKartDisplay;
			HeatStroke::GameObject* pHighlight;
		};

		std::vector<std::string> m_vDrivers;
		std::vector<std::string> m_vKarts;

		std::map<std::string, std::string> m_mContextParameters;
		std::string m_strMode;

		unsigned int m_uiNumPlayers;
		unsigned int m_uiNumAIRacers;
		PerPlayerMenuState m_mPerPlayerMenuState[4];

		void Initialize();
		void GenerateDriverList();
		void GenerateKartList();

		void AddPlayer(const unsigned int p_uiPlayerNum);
		void RemovePlayer(const unsigned int p_uiPlayerNum);
		void SetReadyStatus(const unsigned int p_uiPlayerNum, const bool p_bReady);
		void GoToTrackSelectionState();

		std::string GetRandomDriver() const;
		std::string GetRandomKart() const;
	};
}

#endif

