//------------------------------------------------------------------------
// StateTrackSelectionMenu
// Author:	David Hanna
//	
// Allows player one to select a track to race on.
// Continues to racing state.
//------------------------------------------------------------------------

#include "StateTrackSelectionMenu.h"

Kartaclysm::StateTrackSelectionMenu::StateTrackSelectionMenu()
	:
	GameplayState("Track Selection"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_iTrackSelection(0),
	m_pCurrentHighlight(nullptr),
	m_mTrackTimes()
{
	m_pTrackTimeDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateTrackSelectionMenu::TrackTimeCallback, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("TrackTimeScreen", m_pTrackTimeDelegate);
}

Kartaclysm::StateTrackSelectionMenu::~StateTrackSelectionMenu()
{
	HeatStroke::EventManager::Instance()->RemoveListener("TrackTimeScreen", m_pTrackTimeDelegate);
	delete m_pTrackTimeDelegate;
	m_pTrackTimeDelegate = nullptr;
}

void Kartaclysm::StateTrackSelectionMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_mContextParameters = p_mContextParameters;
	m_iTrackSelection = 0;

	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_menu.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/background.xml");
	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_noob_zone.xml", "HighlightNoobZone");

	std::vector<HeatStroke::GameObject*> vTracks;
	vTracks.push_back(m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_noob_zone.xml", "Noob_Zone"));
	vTracks.push_back(m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_shift_rift.xml", "Shift_Rift"));
	vTracks.push_back(m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_up_and_over.xml", "Up_And_Over"));

	LoadLocalTrackTimesFromXml("CS483/CS483/Kartaclysm/Data/Local/FastestTimes.xml", vTracks);
	FillRaceTimeTextboxes(vTracks);

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateTrackSelectionMenu::LoadLocalTrackTimesFromXml(const std::string& p_strFileName, const std::vector<HeatStroke::GameObject*>& p_vTracks)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(p_strFileName.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		printf("StateTrackSelectionMenu: Error loading best times XML file - not found\n");
		return;
	}

	tinyxml2::XMLElement* pTrackElement = doc.FirstChildElement("BestTimes");
	if (pTrackElement == nullptr)
	{
		printf("StateTrackSelectionMenu: Error loading best times XML file - formatting\n");
		return;
	}

	for (auto pTrack : p_vTracks)
	{
		// Always override current time with the local XML time
		auto pTrackTime = &m_mTrackTimes[pTrack->GetGUID()];

		std::string strTrackXmlName = pTrack->GetGUID();
		std::replace(strTrackXmlName.begin(), strTrackXmlName.end(), ' ', '_');
		HeatStroke::EasyXML::GetOptionalStringAttribute(pTrackElement->FirstChildElement(strTrackXmlName.c_str()), "RaceTime", pTrackTime->m_strLocalRace, pTrackTime->m_strLocalRace);
	}
}

void Kartaclysm::StateTrackSelectionMenu::FillRaceTimeTextboxes(const std::vector<HeatStroke::GameObject*>& p_vTracks)
{
	for (auto pTrack : p_vTracks)
	{
		auto pTrackTime = &m_mTrackTimes[pTrack->GetGUID()];

		std::vector<HeatStroke::GameObject*> vTimerGameObjects = pTrack->GetChildrenWithTag("TimeText");
		for (auto pTimerGO : vTimerGameObjects)
		{
			if (HeatStroke::ComponentTextBox* pTimerTextbox = dynamic_cast<HeatStroke::ComponentTextBox*>(pTimerGO->GetComponent("GOC_Renderable")))
			{
				if (pTimerGO->HasTag("LocalRace"))
				{
					pTimerTextbox->SetMessage(pTrackTime->m_strLocalRace);
				}
				else if (pTimerGO->HasTag("GlobalRace"))
				{
					pTimerTextbox->SetMessage(pTrackTime->m_strGlobalRace);
				}
			}
		}
	}
}

void Kartaclysm::StateTrackSelectionMenu::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->Update(p_fDelta);

	bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
	PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

	if (bConfirm)
	{
		switch (m_iTrackSelection)
		{
		case 0:
			m_mContextParameters.insert(std::pair<std::string, std::string>("TrackDefinitionFile", "CS483/CS483/Kartaclysm/Data/Tracks/noob_zone.xml"));
			break;
		case 1:
			m_mContextParameters.insert(std::pair<std::string, std::string>("TrackDefinitionFile", "CS483/CS483/Kartaclysm/Data/Tracks/shift_rift.xml"));
			break;
		case 2:
			m_mContextParameters.insert(std::pair<std::string, std::string>("TrackDefinitionFile", "CS483/CS483/Kartaclysm/Data/Tracks/up_and_over.xml"));
			break;
		}

		while (!m_pStateMachine->empty())
		{
			m_pStateMachine->Pop();
		}
		m_pStateMachine->Push(STATE_RACING, m_mContextParameters);
	}
	else if (bCancel)
	{
		m_pStateMachine->Pop();
	}
	else if (bUp)
	{
		switch (m_iTrackSelection)
		{
		case 1:
			m_iTrackSelection = 0;
			m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_noob_zone.xml");
			break;
		case 2:
			m_iTrackSelection = 1;
			m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_shift_rift.xml");
			break;
		}
	}
	else if (bDown)
	{
		switch (m_iTrackSelection)
		{
		case 0:
			m_iTrackSelection = 1;
			m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_shift_rift.xml");
			break;
		case 1:
			m_iTrackSelection = 2;
			m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_up_and_over.xml");
			break;
		}
	}
}

void Kartaclysm::StateTrackSelectionMenu::PreRender()
{
	if (m_bSuspended) return;
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateTrackSelectionMenu::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StateTrackSelectionMenu::TrackTimeCallback(const HeatStroke::Event* p_pEvent)
{
	int iTrackCount;
	p_pEvent->GetRequiredIntParameter("trackCount", iTrackCount);

	for (int i = 0; i < iTrackCount; ++i)
	{
		std::string strTrackName;
		float fTime;
		p_pEvent->GetRequiredStringParameter("track" + std::to_string(i), strTrackName);
		p_pEvent->GetRequiredFloatParameter(strTrackName + "0", fTime);
		//p_pEvent->GetRequiredIntParameter(strTrackName + "Count", iRaceCount);

		std::string strFormattedTrackName(strTrackName);
		std::replace(strFormattedTrackName.begin(), strFormattedTrackName.end(), ' ', '_');
		std::string strFormattedTime = FormatTime(std::to_string(fTime));

		auto pTrackTime = &m_mTrackTimes[strFormattedTrackName];
		if (pTrackTime->m_strGlobalRace == "--:--.--" ||
			strFormattedTime < pTrackTime->m_strGlobalRace)
		{
			pTrackTime->m_strGlobalRace = strFormattedTime;
		}
	}
}

std::string Kartaclysm::StateTrackSelectionMenu::FormatTime(const std::string& p_strUnformattedTime) const
{
	float fUnformattedTime = std::stof(p_strUnformattedTime);
	if (fUnformattedTime >= 3599.99f) return "59:99.99";

	int iMinutes = static_cast<int>(fUnformattedTime / 60.0f);
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