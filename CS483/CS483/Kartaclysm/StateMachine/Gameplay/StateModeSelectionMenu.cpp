//------------------------------------------------------------------------
// StateModeSelectionMenu
// Author:	Bradley Cooper
//	
// Allows player one to select racing mode or options menu.
//------------------------------------------------------------------------

#include "StateModeSelectionMenu.h"

Kartaclysm::StateModeSelectionMenu::StateModeSelectionMenu()
	:
	GameplayState("Mode Selection"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_iModeSelection(0),
	m_pCurrentHighlight(nullptr)
{
}

Kartaclysm::StateModeSelectionMenu::~StateModeSelectionMenu()
{
}

void Kartaclysm::StateModeSelectionMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");

	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/ModeSelectionMenu/mode_selection_highlight_time_trial.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/ModeSelectionMenu/mode_options.xml");

	HeatStroke::AudioPlayer::Instance()->StopMusic();
	HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
	HeatStroke::AudioPlayer::Instance()->PlayMusic();
}

void Kartaclysm::StateModeSelectionMenu::Update(const float p_fDelta)
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
			switch (m_iModeSelection)
			{
			case 0:
				m_pStateMachine->Pop();
				m_pStateMachine->Push(STATE_PLAYER_SELECTION_MENU);
				break;
			case 1:
				m_pStateMachine->Pop();
				m_pStateMachine->Push(STATE_PLAYER_SELECTION_MENU); // Tournament mode stubbed
				break;
			case 2:
				m_pStateMachine->Push(STATE_OPTIONS_MENU);
				break;
			}
		}
		else if (bUp)
		{
			switch (m_iModeSelection)
			{
			case 1:
				m_iModeSelection = 0;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/ModeSelectionMenu/mode_selection_highlight_time_trial.xml");
				break;
			case 2:
				m_iModeSelection = 1;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/ModeSelectionMenu/mode_selection_highlight_tournament.xml");
				break;
			}
		}
		else if (bDown)
		{
			switch (m_iModeSelection)
			{
			case 0:
				m_iModeSelection = 1;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/ModeSelectionMenu/mode_selection_highlight_tournament.xml");
				break;
			case 1:
				m_iModeSelection = 2;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/ModeSelectionMenu/mode_selection_highlight_options.xml");
				break;
			}
		}
	}
}

void Kartaclysm::StateModeSelectionMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateModeSelectionMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}