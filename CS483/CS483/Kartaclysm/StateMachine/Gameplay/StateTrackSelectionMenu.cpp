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

	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_noob_zone.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_shift_rift.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_up_and_over.xml");

	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/TrackSelectionMenu/track_selection_highlight_noob_zone.xml", "HighlightNoobZone");
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