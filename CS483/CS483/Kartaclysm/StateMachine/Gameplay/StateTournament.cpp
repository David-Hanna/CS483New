//------------------------------------------------------------------------
// StateTournament
// Author:	Bradley Cooper
//	
// Creates and manages tournament races and results.
//------------------------------------------------------------------------

#include "StateTournament.h"

Kartaclysm::StateTournament::StateTournament()
	:
	GameplayState("Tournament State"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_bReadyForNextRace(false),
	m_bFinished(false),
	m_uiRaceCount(0),
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
	m_bReadyForNextRace = false;
	m_bFinished = false;
	m_uiRaceCount = 0;
	m_mContextParams = p_mContextParameters;
	m_mRacerRankings.clear();

	m_pRaceFinishDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateTournament::RaceFinishCallback, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RaceFinish", m_pRaceFinishDelegate);

	m_pRaceInfoDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateTournament::RaceInfoCallback, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RaceInfo", m_pRaceInfoDelegate);

	// Shuffle tracks for tournament and push to player selection
	std::shuffle(std::begin(m_vTracks), std::end(m_vTracks), HeatStroke::Common::GetRNGesus());
	m_mContextParams["TrackDefinitionFile"] = m_vTracks[0];
	m_pStateMachine->Push(STATE_PLAYER_SELECTION_MENU, m_mContextParams);
}

void Kartaclysm::StateTournament::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	if (m_bReadyForNextRace)
	{
		m_bReadyForNextRace = false;
		m_pStateMachine->Push(STATE_RACING, m_mContextParams);
	}
	else if (m_bFinished)
	{
		m_bFinished = false;
		m_pStateMachine->Pop();
		m_pStateMachine->Push(STATE_RACE_COMPLETE_MENU, m_mContextParams);
		// TODO: Show some kind of congratulations screen?
	}
	else
	{
		// Quit tournament early or some other problem
		m_pStateMachine->Pop();
		if (m_pStateMachine->empty())
		{
			m_pStateMachine->Push(STATE_MAIN_MENU);
		}
	}
}

void Kartaclysm::StateTournament::Exit()
{
	m_bSuspended = true;

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
		std::string strPlayerX, strTime;
		int iPoints = 0;

		std::string strIndex = std::to_string(i);
		p_pEvent->GetRequiredStringParameter("racerId" + strIndex, strPlayerX);
		p_pEvent->GetRequiredStringParameter("racerTime" + strIndex, strTime); // unformatted
		p_pEvent->GetRequiredIntParameter("racerPoints" + strIndex, iPoints);
		
		RacerRanking* pRank = &m_mRacerRankings[strPlayerX]; // get or create map
		pRank->m_iPoints += iPoints;
		pRank->m_fTime += std::stof(strTime);

		m_mContextParams[strPlayerX + "_StartPosition"] = std::to_string(i);
	}

	if (++m_uiRaceCount < m_vTracks.size())
	{
		m_mContextParams["TrackDefinitionFile"] = m_vTracks[m_uiRaceCount];
		m_bReadyForNextRace = true;
	}
	else
	{
		m_bFinished = true;
		m_mContextParams = GenerateTournamentEndResults(&m_mRacerRankings);
	}
}

void Kartaclysm::StateTournament::RaceInfoCallback(const HeatStroke::Event* p_pEvent)
{
	p_pEvent->GetRequiredStringParameter("NumHumanRacers", m_mContextParams["NumHumanRacers"]);
	p_pEvent->GetRequiredStringParameter("NumAIRacers", m_mContextParams["NumAIRacers"]);
	int iNumHumanRacers = atoi(m_mContextParams.at("NumHumanRacers").c_str());
	int iNumAIRacers = atoi(m_mContextParams.at("NumAIRacers").c_str());

	for (int i = 0; i < iNumHumanRacers; ++i)
	{
		std::string strPlayerX = "Player" + std::to_string(i);

		p_pEvent->GetRequiredStringParameter(strPlayerX + "_KartDefinitionFile", m_mContextParams[strPlayerX + "_KartDefinitionFile"]);
		p_pEvent->GetRequiredStringParameter(strPlayerX + "_DriverDefinitionFile", m_mContextParams[strPlayerX + "_DriverDefinitionFile"]);
		p_pEvent->GetRequiredStringParameter(strPlayerX + "_CameraDefinitionFile", m_mContextParams[strPlayerX + "_CameraDefinitionFile"]);
	}

	for (int i = 0; i < iNumAIRacers; ++i)
	{
		std::string strAIRacer = "AI_racer" + std::to_string(i);

		p_pEvent->GetRequiredStringParameter(strAIRacer + "_KartDefinitionFile", m_mContextParams[strAIRacer + "_KartDefinitionFile"]);
		p_pEvent->GetRequiredStringParameter(strAIRacer + "_DriverDefinitionFile", m_mContextParams[strAIRacer + "_DriverDefinitionFile"]);
	}
}

std::map<std::string, std::string> Kartaclysm::StateTournament::GenerateTournamentEndResults(std::map<std::string, RacerRanking>* p_pRankings) const
{
	std::map<std::string, std::string> mResults;
	mResults["screenTitle"] = "Tournament";
	mResults["numRacers"] = std::to_string(m_mRacerRankings.size());

	int iRank = 0;
	while (!p_pRankings->empty())
	{
		auto it = p_pRankings->begin(), end = p_pRankings->end(), max = it;
		for (++it; it != end; ++it)
		{
			if (max->second.m_iPoints < it->second.m_iPoints)
			{
				max = it;
			}
		}

		std::string strIndex = std::to_string(iRank++);
		mResults["racerId" + strIndex] = max->first;
		mResults["racerTime" + strIndex] = std::to_string(max->second.m_fTime);
		mResults["racerPoints" + strIndex] = std::to_string(max->second.m_iPoints);

		p_pRankings->erase(max);
	}
	return mResults;
}