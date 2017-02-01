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
	m_pCurrentHighlight(nullptr)
{
}

Kartaclysm::StateTrackSelectionMenu::~StateTrackSelectionMenu()
{
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

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");

	std::vector<HeatStroke::GameObject*> vTracks;
	vTracks.push_back(m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_noob_zone.xml", "Noob_Zone"));
	vTracks.push_back(m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_shift_rift.xml", "Shift_Rift"));
	vTracks.push_back(m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_up_and_over.xml", "Up_and_Over"));

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pTrackElement = nullptr;
	if (doc.LoadFile("CS483/CS483/Kartaclysm/Data/DevConfig/FastestTimes.xml") == tinyxml2::XML_NO_ERROR)
	{
		pTrackElement = doc.FirstChildElement("BestTimes");
		auto it = vTracks.begin(), end = vTracks.end();
		for (; it != end; it++)
		{
			LoadBestTrackTime(pTrackElement, (*it)->GetGUID(), (*it)->GetChildrenWithTag("TimeText"));
		}
	}
	else
	{
		printf("StateTrackSelectionMenu: Error loading best times XML file");
	}

	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_noob_zone.xml", "HighlightNoobZone");

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateTrackSelectionMenu::LoadBestTrackTime(tinyxml2::XMLElement* p_pBestTimesElement, const std::string& p_strTrack, const std::vector<HeatStroke::GameObject*>& p_vTrackTimers)
{
	// TODO: Expand to include best lap time
	std::string strBestRaceTime = "--:--.--";

	if (p_pBestTimesElement != nullptr)
	{
		tinyxml2::XMLElement* pTrackElement = p_pBestTimesElement->FirstChildElement(p_strTrack.c_str());
		if (pTrackElement != nullptr)
		{
			HeatStroke::EasyXML::GetOptionalStringAttribute(pTrackElement, "RaceTime", strBestRaceTime, strBestRaceTime);
		}
	}

	auto it = p_vTrackTimers.begin(), end = p_vTrackTimers.end();
	for (; it != end; it++)
	{
		HeatStroke::ComponentTextBox* pTimer = dynamic_cast<HeatStroke::ComponentTextBox*>((*it)->GetComponent("GOC_Renderable"));

		if ((*it)->HasTag("RaceTime"))
		{
			pTimer->SetMessage(strBestRaceTime);
		}
	}
}

void Kartaclysm::StateTrackSelectionMenu::Update(const float p_fDelta)
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

			m_pStateMachine->Pop();
			m_pStateMachine->Push(STATE_RACING, m_mContextParameters);
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
}

void Kartaclysm::StateTrackSelectionMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateTrackSelectionMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}