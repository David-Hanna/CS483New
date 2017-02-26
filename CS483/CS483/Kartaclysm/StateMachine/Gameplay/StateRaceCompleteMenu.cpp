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
	m_bSuspended(true)
{
}

Kartaclysm::StateRaceCompleteMenu::~StateRaceCompleteMenu()
{
}

void Kartaclysm::StateRaceCompleteMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");	

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/RaceCompleteMenu/race_complete_message.xml");

	std::string strMode = p_mContextParameters.at("Mode");
	if (strMode == "Tournament")
	{
		dynamic_cast<HeatStroke::ComponentTextBox*>(m_pGameObjectManager->GetGameObject("title")->GetComponent("GOC_Renderable"))->SetMessage("Tournament");
	}
	else if (strMode == "Single")
	{
		SendRaceFinishEvent(p_mContextParameters);
		AddRaceToDatabase(p_mContextParameters);
		RecordBestTime(p_mContextParameters, "CS483/CS483/Kartaclysm/Data/Local/FastestTimes.xml");
	}
	else
	{
		assert(false && "Unknown mode");
	}

	PopulateRaceResultsList(p_mContextParameters);

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateRaceCompleteMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (bConfirm)
		{
			m_pStateMachine->Pop();
			if (m_pStateMachine->empty()) // may be popped to StateTournament
			{
				m_pStateMachine->Push(STATE_MAIN_MENU);
			}
		}
	}
}

void Kartaclysm::StateRaceCompleteMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
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

void Kartaclysm::StateRaceCompleteMenu::AddRaceToDatabase(const std::map<std::string, std::string>& p_mRaceResults)
{
	int iLaps = std::stoi(p_mRaceResults.at("numLaps"));
	if (iLaps <= 1) return;

	Database::InsertRace mRace;
	
	// TODO: Should be a ParsePkFromString() method somewhere. Doing it lazy style for now
	std::string strTrack = p_mRaceResults.at("trackName");
	if (strTrack == "Up and Over")
		mRace.track_id = Database::eUpAndOver;
	else if (strTrack == "Shift Rift")
		mRace.track_id = Database::eShiftRift;
	else if (strTrack == "Noob Zone")
		mRace.track_id = Database::eNoobZone;
	else
		assert(false && "Unknown track name");

	int iNumRacers = std::stoi(p_mRaceResults.at("numRacers"));
	for (int i = 0; i < iNumRacers; ++i)
	{
		Database::InsertRacePlayer mPlayer;

		std::string strIndex = std::to_string(i);
		mPlayer.player_num = std::stoi(p_mRaceResults.at("racerId" + strIndex).substr(6));
		mPlayer.is_human = true; // TODO: hardcoded

		std::string strDriver = p_mRaceResults.at("racerDriver" + strIndex);
		if (strDriver == "Cleopapa")
			mPlayer.driver = Database::eCleopapa;
		else if (strDriver == "Clockmaker")
			mPlayer.driver = Database::eClockmaker;
		else if (strDriver == "Kingping")
			mPlayer.driver = Database::eKingpin;
		else
			assert(false && "Unknown driver name");

		std::string strKart = p_mRaceResults.at("racerKart" + strIndex);
		if (strKart == "Juggernaut")
			mPlayer.kart = Database::eJuggernaut;
		else if (strKart == "Showoff")
			mPlayer.kart = Database::eShowoff;
		else if (strKart == "Speedster")
			mPlayer.kart = Database::eSpeedster;
		else
			assert(false && "Unknown kart name");

		for (int i = 1; i <= iLaps; ++i)
		{
			mPlayer.lap_times.push_back(std::stof(p_mRaceResults.at("racer" + strIndex + "Lap" + std::to_string(i))));
		}

		mRace.race_players.push_back(mPlayer);
	}

	DatabaseManager::Instance()->InsertRaceQuery(mRace);
}

void Kartaclysm::StateRaceCompleteMenu::SendRaceFinishEvent(const std::map<std::string, std::string>& p_mRaceResults)
{
	HeatStroke::Event* pEvent = new HeatStroke::Event("RaceFinish");

	int iNumRacers = std::stoi(p_mRaceResults.at("numRacers"));
	for (int i = 0; i < iNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);
		pEvent->SetStringParameter("racerId" + strIndex, p_mRaceResults.at("racerId" + strIndex));
		pEvent->SetStringParameter("racerTime" + strIndex, p_mRaceResults.at("racerTime" + strIndex));
	}

	pEvent->SetIntParameter("PlayerCount", iNumRacers);
	HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
}

void Kartaclysm::StateRaceCompleteMenu::RecordBestTime(const std::map<std::string, std::string>& p_mRaceResults, const std::string& p_strXmlFilePath)
{
	std::string strTrack = p_mRaceResults.at("trackName");
	std::replace(strTrack.begin(), strTrack.end(), ' ', '_');

	std::string strNewBestTime = FormatTime(p_mRaceResults.at("racerTime0"));
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

void Kartaclysm::StateRaceCompleteMenu::PopulateRaceResultsList(const std::map<std::string, std::string>& p_mRaceResults)
{
	int iNumRacers = std::stoi(p_mRaceResults.at("numRacers"));
	for (int i = 0; i < iNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);
		std::string strPosition = std::to_string(i + 1);
		std::string strRacerId = p_mRaceResults.at("racerId" + strIndex);
		std::string strRacerTime = FormatTime(p_mRaceResults.at("racerTime" + strIndex));

		std::string strRacerPoints = "";
		auto find = p_mRaceResults.find("racerPoints" + strIndex);
		if (find != p_mRaceResults.end())
		{
			strRacerPoints = find->second;
		}

		std::string strRacerResults = strPosition + " " + strRacerId + " " + (strRacerPoints == "" ? strRacerTime : strRacerPoints);
		dynamic_cast<HeatStroke::ComponentTextBox*>(m_pGameObjectManager->GetGameObject("results" + strIndex)->GetComponent("GOC_Renderable"))->SetMessage(strRacerResults);
	}
}

std::string Kartaclysm::StateRaceCompleteMenu::FormatTime(const std::string& p_strUnformattedTime) const
{
	float fUnformattedTime = std::stof(p_strUnformattedTime);
	int iMinutes = (int)fUnformattedTime / 60;
	float fSeconds = fmod(fUnformattedTime, 60.0f);

	std::string strMinutes = std::to_string(iMinutes);
	std::string strSeconds = std::to_string(fSeconds);
	if (iMinutes < 10)
	{
		strMinutes = "0" + strMinutes;
	}
	if (fSeconds < 10.0f)
	{
		strSeconds = "0" + strSeconds;
	}
	strSeconds = strSeconds.substr(0, 5);

	return strMinutes + ":" + strSeconds;
}