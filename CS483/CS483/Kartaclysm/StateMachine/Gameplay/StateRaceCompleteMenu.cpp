//------------------------------------------------------------------------
// StateRaceCompleteMenu
// Author:	David Hanna
//	
// Shows the results of the race and continues to the Main Menu state.
//------------------------------------------------------------------------

#include "StateRaceCompleteMenu.h"

Kartaclysm::StateRaceCompleteMenu::StateRaceCompleteMenu()
	:
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

	RecordBestTime(p_mContextParameters, "CS483/CS483/Kartaclysm/Data/DevConfig/FastestTimes.xml");
	PopulateRaceResultsList(p_mContextParameters);

	printf("Entering Race Complete Menu State.\n");
}

void Kartaclysm::StateRaceCompleteMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_ENTER))
		{
			m_pStateMachine->Pop();
			m_pStateMachine->Push(STATE_MAIN_MENU, std::map<std::string, std::string>());
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
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	printf("Exiting Race Complete Menu state.\n");
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
		std::string strRacerId = p_mRaceResults.at("racerId" + strIndex);
		std::string strRacerTime = FormatTime(p_mRaceResults.at("racerTime" + strIndex));
		std::string strRacerResults = strIndex + " " + strRacerId + " " + strRacerTime;
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