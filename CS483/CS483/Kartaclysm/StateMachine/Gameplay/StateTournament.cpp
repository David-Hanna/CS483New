//------------------------------------------------------------------------
// StateTournament
// Author:	Bradley Cooper
//	
// Creates and manages tournament races and results.
//------------------------------------------------------------------------

#include "StateTournament.h"

Kartaclysm::StateTournament::StateTournament()
	:
	GameplayState("Mode Selection"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_iRaceCount(0),
	m_mContextParams(),
	m_mRacerRankings()
{
	m_vTracks = std::vector<std::string> {
		"CS483/CS483/Kartaclysm/Data/Tracks/noob_zone.xml",
		"CS483/CS483/Kartaclysm/Data/Tracks/shift_rift.xml",
		"CS483/CS483/Kartaclysm/Data/Tracks/up_and_over.xml"
	};
}

Kartaclysm::StateTournament::~StateTournament()
{
}

void Kartaclysm::StateTournament::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_iRaceCount = 0;
	m_mRacerRankings.clear();
	m_mContextParams.clear();

	m_pRaceFinishDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateTournament::RaceFinishCallback, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RaceFinish", m_pRaceFinishDelegate);

	m_pRaceInfoDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateTournament::RaceInfoCallback, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RaceInfo", m_pRaceInfoDelegate);

	// Shuffle tracks for tournament and push to player selection
	std::random_shuffle(m_vTracks.begin(), m_vTracks.end());
	std::map<std::string, std::string> mContextParams;
	mContextParams["TrackDefinitionFile"] = m_vTracks[0];
	m_pStateMachine->Push(STATE_PLAYER_SELECTION_MENU, mContextParams);
}

void Kartaclysm::StateTournament::Suspend(const int p_iNewState)
{
	m_bSuspended = true;
}

void Kartaclysm::StateTournament::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;

	// TODO: Handle popping from STATE_RACE_COMPLETE

	if (p_iPrevState == STATE_PLAYER_SELECTION_MENU)
	{
		m_pStateMachine->Pop();
	}
}

void Kartaclysm::StateTournament::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		printf("StateTournament should not be updating");
		m_pStateMachine->Pop();
		return;
	}
}

void Kartaclysm::StateTournament::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	if (m_pRaceFinishDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RaceFinish", m_pRaceFinishDelegate);
		delete m_pRaceFinishDelegate;
		m_pRaceFinishDelegate = nullptr;
	}

	if (m_pRaceInfoDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RaceInfo", m_pRaceInfoDelegate);
		delete m_pRaceInfoDelegate;
		m_pRaceInfoDelegate = nullptr;
	}
}

void Kartaclysm::StateTournament::RaceFinishCallback(const HeatStroke::Event* p_pEvent)
{
	int iNumPlayers;
	p_pEvent->GetRequiredIntParameter("PlayerCount", iNumPlayers);

	for (int i = 0; i < iNumPlayers; ++i)
	{
		std::string strPlayer, strTime;

		std::string strIndex = std::to_string(i);
		p_pEvent->GetRequiredStringParameter("racerId" + strIndex, strPlayer);
		p_pEvent->GetRequiredStringParameter("racerTime" + strIndex, strTime);

		int iPoints = 0;
		switch (i)
		{
			case 0: iPoints = 10; break;
			case 1: iPoints = 8; break;
			case 2: iPoints = 6; break;
			case 3: iPoints = 4; break;
			case 4: iPoints = 3; break;
			case 5: iPoints = 2; break;
			case 6: iPoints = 1; break;
			default: iPoints = 0; break;
		}

		RacerRanking* pRank = &m_mRacerRankings[strPlayer]; // get or create map
		pRank->m_iPoints += iPoints;
		AddToTime(pRank->m_strTime, strTime);
	}

	if (++m_iRaceCount < 3)
	{
		// TODO: Also needs to know racer loadouts
		// TODO: Change starting position between races
		std::map<std::string, std::string> mContextParams;
		mContextParams["TrackDefinitionFile"] = m_vTracks[m_iRaceCount];

		m_pStateMachine->Push(STATE_RACING, mContextParams);
		// TODO: CANNOT PUSH DURING THIS EVENT
	}
	else
	{
		// TODO: End tournament
		/*std::map<std::string, std::string> mContextParams;
		m_pStateMachine->Push(STATE_RACE_COMPLETE_MENU, mContextParams);*/
		printf("Tournament ended");
		auto it = m_mRacerRankings.begin(), end = m_mRacerRankings.end();
		for (; it != end; ++it)
		{
			printf("%s:\t%i\t%s", it->first, it->second.m_iPoints, it->second.m_strTime);
		}
	}
}

void Kartaclysm::StateTournament::RaceInfoCallback(const HeatStroke::Event* p_pEvent)
{
	// TODO: Store racer and driver into contect parameters
}

void Kartaclysm::StateTournament::AddToTime(std::string& p_strBaseTime, const std::string& p_strAddedTime) const
{
	if (p_strAddedTime == "") return;

	if (p_strBaseTime == "")
	{
		p_strBaseTime = p_strAddedTime;
		return;
	}

	// TODO: Add times together
	return;
}