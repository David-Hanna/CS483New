//------------------------------------------------------------------------
// StatePaused
// Author:	Bradley Cooper
//	
// Gameplay state for pausing during a race.
//------------------------------------------------------------------------

#include "StatePaused.h"

Kartaclysm::StatePaused::StatePaused()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_iPausedPlayer(-1)
{
}

Kartaclysm::StatePaused::~StatePaused()
{
}

void Kartaclysm::StatePaused::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	printf("Entering State Paused.\n");

	m_bSuspended = false;

	// Initialize our GameObjectManager
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_options.xml");
	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_continue.xml");

	m_iOptionSelection = 0;
	m_bSkipFirstFrame = true;
	m_iPausedPlayer = atoi(p_mContextParameters.at("Player").c_str());
}

void Kartaclysm::StatePaused::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		if (m_bSkipFirstFrame)
		{
			// Done to avoid having the Pause button be pressed on first frame, which may also be the confirm button
			m_bSkipFirstFrame = false;
			return;
		}

		bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(m_iPausedPlayer, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (bConfirm)
		{
			m_pStateMachine->Pop();

			switch (m_iOptionSelection)
			{
			case 0: // continue
				break;
			case 1: // restart
				HeatStroke::EventManager::Instance()->TriggerEvent(new HeatStroke::Event("RaceRestart"));
				break;
			case 2: // quit
				m_pStateMachine->Pop();
				m_pStateMachine->Push(STATE_MAIN_MENU);
				break;
			}
		}
		else if (bCancel)
		{
			m_pStateMachine->Pop();
		}
		else if (bUp)
		{
			switch (m_iOptionSelection)
			{
			case 1:
				m_iOptionSelection = 0;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_continue.xml");
				break;
			case 2:
				m_iOptionSelection = 1;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_restart.xml");
				break;
			}
		}
		else if (bDown)
		{
			switch (m_iOptionSelection)
			{
			case 0:
				m_iOptionSelection = 1;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_restart.xml");
				break;
			case 1:
				m_iOptionSelection = 2;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_quit.xml");
				break;
			}
		}
	}
}

void Kartaclysm::StatePaused::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StatePaused::Exit()
{
	printf("Exiting State Paused.\n");

	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}