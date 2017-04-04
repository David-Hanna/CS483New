//------------------------------------------------------------------------
// StateTournament
// Author:	Bradley Cooper
//	
// Creates and manages tournament races and results.
//------------------------------------------------------------------------

#ifndef STATE_TOURNAMENT_H
#define STATE_TOURNAMENT_H

#include <thread>
#include <algorithm>
#include <random>

#include "Common.h"
#include "RNGesus.h"
#include "GameplayState.h"
#include "EventManager.h"
#include "TimeFormat.h"
#include "DatabaseManager.h"

namespace Kartaclysm
{
	class StateTournament : public Kartaclysm::GameplayState
	{
	public:
		StateTournament();
		virtual ~StateTournament();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)		{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)	{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender() {}
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

		void RaceFinishCallback(const HeatStroke::Event* p_pEvent);
		void RaceInfoCallback(const HeatStroke::Event* p_pEvent);

		struct RacerRanking {
			int		m_iPoints;
			float	m_fTime;
		};

		std::map<std::string, std::string> GenerateTournamentEndResults(std::map<std::string, RacerRanking>* p_pRankings) const;

	private:
		bool m_bReadyForNextRace;
		bool m_bFinished;
		bool m_bCongrats;
		unsigned int m_uiRaceCount;
		std::vector<std::string> m_vTracks;
		std::map<std::string, std::string> m_mContextParams;
		std::map<std::string, RacerRanking> m_mRacerRankings;

		std::function<void(const HeatStroke::Event*)>* m_pRaceFinishDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRaceInfoDelegate;
	};
}

#endif

