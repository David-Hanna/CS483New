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
	m_bTournamentResults(false)
{
}

Kartaclysm::StateRaceCompleteMenu::~StateRaceCompleteMenu()
{
}

void Kartaclysm::StateRaceCompleteMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
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
