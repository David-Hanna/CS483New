//------------------------------------------------------------------------
// StateRaceCompleteMenu
// Author:	David Hanna
//	
// Shows the results of the race and continues to the Main Menu state.
//------------------------------------------------------------------------

#include "StateRaceCompleteMenu.h"

Kartaclysm::StateRaceCompleteMenu::StateRaceCompleteMenu()
	:
	GameplayState("Race Complete"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_bTournamentRace(false),
	m_bTournamentResults(false),
	m_bRenderOnce(false),
	m_bInsertedIntoDatabase(false)
{
}

Kartaclysm::StateRaceCompleteMenu::~StateRaceCompleteMenu()
{
}

void Kartaclysm::StateRaceCompleteMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_bRenderOnce = false;
	m_bInsertedIntoDatabase = false;
	std::map<std::string, std::string> mResults = p_mContextParameters;
	m_bTournamentResults = (mResults.find("trackName") == mResults.end());
	m_bTournamentRace = (mResults.find("racerPoints0") != mResults.end());
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_menu.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/background.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/RaceCompleteMenu/race_complete_message.xml");

	auto find = mResults.find("screenTitle");
	if (find != mResults.end())
	{
		dynamic_cast<HeatStroke::ComponentTextBox*>(m_pGameObjectManager->GetGameObject("title")->GetComponent("GOC_Renderable"))->SetMessage(find->second);
	}

	CreateDatabaseInsertStruct(mResults);
	SendRaceFinishEvent(mResults);
	RecordBestTime(mResults, "CS483/CS483/Kartaclysm/Data/Local/FastestTimes.xml");
	PopulateRaceResultsList(mResults);

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateRaceCompleteMenu::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->Update(p_fDelta);

	if (!m_bRenderOnce) return;

	if (!m_bInsertedIntoDatabase)
	{
		m_bInsertedIntoDatabase = true;
		if (!m_bTournamentResults)
		{
			CreateRaceInsertThread();
		}
	}

	bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
	PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

	if (bConfirm || bCancel)
	{
		m_pStateMachine->Pop();
		if (m_pStateMachine->empty()) // may be popped to StateTournament
		{
			m_pStateMachine->Push(STATE_MAIN_MENU);
		}
	}
}

void Kartaclysm::StateRaceCompleteMenu::PreRender()
{
	if (m_bSuspended) return;
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
	m_bRenderOnce = true;
}

void Kartaclysm::StateRaceCompleteMenu::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StateRaceCompleteMenu::CreateDatabaseInsertStruct(const std::map<std::string, std::string>& p_mRaceResults)
{
	m_mRaceDatabaseInsert = Database::InsertRace();
	if (m_bTournamentResults) return;

	m_mRaceDatabaseInsert.track_id = Database::StringToTrackPK(p_mRaceResults.at("trackName"));
	assert(m_mRaceDatabaseInsert.track_id != Database::eTrackError);

	int iNum_mRaceDatabaseInsertrs = std::stoi(p_mRaceResults.at("numRacers"));
	int iLaps = std::stoi(p_mRaceResults.at("numLaps"));
	int iHumanPlayers = std::stoi(p_mRaceResults.at("numHumans"));

	for (int i = 0; i < iNum_mRaceDatabaseInsertrs; ++i)
	{
		Database::InsertRacePlayer mPlayer;
		std::string strIndex = std::to_string(i);

		std::string strPlayerId = p_mRaceResults.at("racerId" + strIndex);
		switch (strPlayerId.at(0))
		{
			case 'a': mPlayer.player_num = iHumanPlayers + std::stoi(strPlayerId.substr(8)); // ai_racerX
				break;
			case 'P': mPlayer.player_num = std::stoi(strPlayerId.substr(6)); // PlayerX
				break;
			default: mPlayer.player_num = -1;
#ifdef _DEBUG
				assert(false && "Unknown racer GUID format");
#endif
				m_mRaceDatabaseInsert.valid = false;
				return;
		}

		std::string strPosition = p_mRaceResults.at("racerPosition" + strIndex);
		mPlayer.position = (strPosition == "dnf" ? -1 : std::stoi(strPosition));
		mPlayer.is_human = p_mRaceResults.at("racerIsHuman" + strIndex) != "0";
		mPlayer.driver = Database::StringToDriverPK(p_mRaceResults.at("racerDriver" + strIndex));
		mPlayer.kart = Database::StringToKartPK(p_mRaceResults.at("racerKart" + strIndex));
		assert(mPlayer.driver != Database::eDriverError);
		assert(mPlayer.kart != Database::eKartError);

		for (int i = 1; i <= iLaps; ++i)
		{
			auto find = p_mRaceResults.find("racer" + strIndex + "Lap" + std::to_string(i));
			if (find == p_mRaceResults.end())
			{
#ifdef _DEBUG
				assert(false && "Racer has less laps than required");
#endif
				m_mRaceDatabaseInsert.valid = false;
				return;
			}
			mPlayer.lap_times.push_back(std::stof(find->second));
		}

#ifdef _DEBUG
		if (p_mRaceResults.find("racer" + strIndex + "Lap" + std::to_string(iLaps + 1)) != p_mRaceResults.end())
		{
			assert(false && "Racer has more laps than required");
		}
#endif

		m_mRaceDatabaseInsert.race_players.push_back(mPlayer);
	}

	m_mRaceDatabaseInsert.valid = true;
}

void Kartaclysm::StateRaceCompleteMenu::CreateRaceInsertThread()
{
	if (!m_mRaceDatabaseInsert.valid) return;

	// For thread safety, must call this line before thread begins running
	bool bLastTournRace = (DatabaseManager::Instance()->RemainingTournamentRaces() == 1);

	auto thrInsertQuery = std::thread(
		&DatabaseManager::InsertRaceQuery,
		DatabaseManager::Instance(),
		m_mRaceDatabaseInsert);

	if (m_bTournamentRace && bLastTournRace)
	{
		// For thread safety, we must ensure this thread ends before the End Tournament query
		// TODO: Maybe queued queries would be better? This is the quick solution for now
		thrInsertQuery.join();
	}
	else
	{
		thrInsertQuery.detach();
	}
}

void Kartaclysm::StateRaceCompleteMenu::SendRaceFinishEvent(const std::map<std::string, std::string>& p_mRaceResults) const
{
	if (m_bTournamentResults) return;

	HeatStroke::Event* pEvent = new HeatStroke::Event("RaceFinish");

	int iNumRacers = std::stoi(p_mRaceResults.at("numRacers"));
	for (int i = 0; i < iNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);
		pEvent->SetStringParameter("racerId" + strIndex, p_mRaceResults.at("racerId" + strIndex));
		pEvent->SetStringParameter("racerTime" + strIndex, p_mRaceResults.at("racerTime" + strIndex));

		if (m_bTournamentRace)
		{
			pEvent->SetIntParameter("racerPoints" + strIndex, std::stoi(p_mRaceResults.at("racerPoints" + strIndex)));
		}
	}

	pEvent->SetIntParameter("PlayerCount", iNumRacers);
	HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
}

void Kartaclysm::StateRaceCompleteMenu::RecordBestTime(const std::map<std::string, std::string>& p_mRaceResults, const std::string& p_strXmlFilePath) const
{
	if (m_bTournamentResults) return;

	std::string strTrack = p_mRaceResults.at("trackName");
	std::replace(strTrack.begin(), strTrack.end(), ' ', '_');

	std::string strNewBestTime = Common::TimeStringFromFloat(std::stof(p_mRaceResults.at("racerTime0")));
	std::string strOldBestTime = "59:99.99";

	// read current fastest time
	// TODO: This could be passed down from TrackSelectionMenu to avoid another file reading operation
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pTrackElement = nullptr;
	bool bFileFound = (doc.LoadFile(p_strXmlFilePath.c_str()) == tinyxml2::XML_NO_ERROR);

	if (!bFileFound)
	{
		printf("StateRaceCompleteMenu: Error loading best times XML file");
	}
	else
	{
		pTrackElement = doc.FirstChildElement("BestTimes")->FirstChildElement(strTrack.c_str());
		if (pTrackElement != nullptr)
		{
			HeatStroke::EasyXML::GetOptionalStringAttribute(pTrackElement, "RaceTime", strOldBestTime, strOldBestTime);
		}
	}

	// save new track time if faster
	if (strNewBestTime.compare(strOldBestTime) < 0)
	{
		if (!bFileFound)
		{
			doc.InsertFirstChild(doc.NewElement("BestTimes"));
		}

		if (pTrackElement == nullptr)
		{
			pTrackElement = doc.NewElement(strTrack.c_str());
			doc.FirstChildElement("BestTimes")->InsertEndChild(pTrackElement);
		}

		pTrackElement->SetAttribute("RaceTime", strNewBestTime.c_str());
		if (doc.SaveFile(p_strXmlFilePath.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			printf("StateRaceCompleteMenu: Error saving best times XML file");
		}
	}
}

void Kartaclysm::StateRaceCompleteMenu::PopulateRaceResultsList(const std::map<std::string, std::string>& p_mRaceResults) const
{
	int iNumRacers = std::stoi(p_mRaceResults.at("numRacers"));
	for (int i = 0; i < iNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);

		std::string strRacerId = p_mRaceResults.at("racerId" + strIndex);
		for (unsigned int uiPadding = (strRacerId.at(0) == 'P' ? 7 : 2); uiPadding > 0; --uiPadding) strRacerId += " ";

		std::string strRacerTime = Common::TimeStringFromFloat(std::stof(p_mRaceResults.at("racerTime" + strIndex))) + "  ";

		std::string strRacerPoints = "";
		auto find = p_mRaceResults.find("racerPoints" + strIndex);
		if (find != p_mRaceResults.end())
		{
			strRacerPoints = find->second;
		}

		std::string strRacerResults = strRacerId + " " + strRacerTime + " " + strRacerPoints;
		dynamic_cast<HeatStroke::ComponentTextBox*>(m_pGameObjectManager->GetGameObject("results" + strIndex)->GetComponent("GOC_Renderable"))->SetMessage(strRacerResults);
		
		std::string strPositionSpriteFile = "results" + strIndex + "/position_" + p_mRaceResults.at("racerPosition" + strIndex) + ".xml";
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/RaceCompleteMenu/" + strPositionSpriteFile);
	}
}
