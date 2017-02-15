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
	std::random_shuffle(m_vTracks.begin(), m_vTracks.end());
	m_mContextParams["TrackDefinitionFile"] = m_vTracks[0];
	m_pStateMachine->Push(STATE_PLAYER_SELECTION_MENU, m_mContextParams);
}

void Kartaclysm::StateTournament::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
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

		std::string strIndex = std::to_string(i);
		p_pEvent->GetRequiredStringParameter("racerId" + strIndex, strPlayerX);
		p_pEvent->GetRequiredStringParameter("racerTime" + strIndex, strTime); // unformatted

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
		m_mContextParams.clear();
		m_mContextParams["Mode"] = "Tournament";
		m_mContextParams["numRacers"] = std::to_string(m_mRacerRankings.size());

		int iRank = 0;
		while (!m_mRacerRankings.empty())
		{
			// Find player with highest overall points (TODO: currently breaks ties with race times)
			auto it = m_mRacerRankings.begin(), end = m_mRacerRankings.end();
			std::string strMaxPointPlayer = it->first;
			for (++it; it != end; ++it)
			{
				if (m_mRacerRankings.at(strMaxPointPlayer).m_iPoints == it->second.m_iPoints)
				{
					if (m_mRacerRankings.at(strMaxPointPlayer).m_fTime > it->second.m_fTime)
					{
						strMaxPointPlayer = it->first;
					}
				}
				else if (m_mRacerRankings.at(strMaxPointPlayer).m_iPoints < it->second.m_iPoints)
				{
					strMaxPointPlayer = it->first;
				}
			}

			std::string strIndex = std::to_string(iRank++);
			m_mContextParams["racerId" + strIndex] = strMaxPointPlayer;
			m_mContextParams["racerTime" + strIndex] = std::to_string(m_mRacerRankings.at(strMaxPointPlayer).m_fTime);
			m_mContextParams["racerPoints" + strIndex] = std::to_string(m_mRacerRankings.at(strMaxPointPlayer).m_iPoints);

			m_mRacerRankings.erase(m_mRacerRankings.find(strMaxPointPlayer));
		}
	}
}

void Kartaclysm::StateTournament::RaceInfoCallback(const HeatStroke::Event* p_pEvent)
{
	p_pEvent->GetRequiredStringParameter("PlayerCount", m_mContextParams["PlayerCount"]);
	int iPlayerCount = atoi(m_mContextParams.at("PlayerCount").c_str());

	for (int i = 0; i < iPlayerCount; ++i)
	{
		std::string strPlayerX = "Player" + std::to_string(i);

		p_pEvent->GetRequiredStringParameter(strPlayerX + "_KartDefinitionFile", m_mContextParams[strPlayerX + "_KartDefinitionFile"]);
		p_pEvent->GetRequiredStringParameter(strPlayerX + "_DriverDefinitionFile", m_mContextParams[strPlayerX + "_DriverDefinitionFile"]);
		p_pEvent->GetRequiredStringParameter(strPlayerX + "_CameraDefinitionFile", m_mContextParams[strPlayerX + "_CameraDefinitionFile"]);
	}
}

std::string Kartaclysm::StateTournament::FormatTime(float p_fUnformattedTime) const
{
	int iMinutes = static_cast<int>(p_fUnformattedTime) / 60;
	float fSeconds = fmod(p_fUnformattedTime, 60.0f);

	std::string strMinutes = std::to_string(iMinutes);
	std::string strSeconds = std::to_string(fSeconds);

	if (fSeconds < 10.0f)
	{
		strSeconds = "0" + strSeconds;
	}
	if (iMinutes < 10)
	{
		strMinutes = "0" + strMinutes;
	}
	else if (iMinutes > 60)
	{
		strMinutes = "59";
		strSeconds = "99.99999";
	}
	strSeconds = strSeconds.substr(0, 5);

	return strMinutes + ":" + strSeconds;
}