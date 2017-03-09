//------------------------------------------------------------------------
// StateRaceCompleteMenu
// Author:	David Hanna
//	
// Shows the results of the race and continues to the Main Menu state.
//------------------------------------------------------------------------

#ifndef STATE_RACE_COMPLETE_MENU_H
#define STATE_RACE_COMPLETE_MENU_H

#include <thread>

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentTextbox.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "EasyXML.h"
#include "AudioPlayer.h"
#include "DatabaseManager.h"

namespace Kartaclysm
{
	class StateRaceCompleteMenu : public Kartaclysm::GameplayState
	{
	public:
		StateRaceCompleteMenu();
		virtual ~StateRaceCompleteMenu();

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
		bool m_bTournamentRace;
		bool m_bTournamentResults;

		void AddRacerPositionToMap(std::map<std::string, std::string>* p_pRaceResults) const;
		void AddRaceToDatabase(const std::map<std::string, std::string>& p_mRaceResults);
		void SendRaceFinishEvent(const std::map<std::string, std::string>& p_mRaceResults) const;
		void RecordBestTime(const std::map<std::string, std::string>& p_mRaceResults, const std::string& p_strXmlFilePath) const;
		void PopulateRaceResultsList(const std::map<std::string, std::string>& p_mRaceResults) const;
		std::string FormatTime(const std::string& p_strUnformattedTime) const;
	};
}

#endif

