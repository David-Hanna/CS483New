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
	m_iPausedPlayer(-1),
	m_pPauseDelegate(nullptr)
{
}

Kartaclysm::StatePaused::~StatePaused()
{
}

void Kartaclysm::StatePaused::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	printf("Entering State Paused.\n");

	m_bSuspended = false;

	// Tell the HUD to render a pause message
	/*HeatStroke::Event* pHudEvent = new HeatStroke::Event("Pause_HUD");
	pHudEvent->SetIntParameter("Display", 1);
	HeatStroke::EventManager::Instance()->TriggerEvent(pHudEvent);*/

	// Register listening for pause
	m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StatePaused::UnpauseGame, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);

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

	// Handle passed context parameters
	auto it = p_mContextParameters.find("Player");
	if (it != p_mContextParameters.end())
	{
		m_iPausedPlayer = atoi(it->second.c_str());
	}
}

void Kartaclysm::StatePaused::Suspend(const int p_iNewState)
{
	m_bSuspended = true;
	HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
}

void Kartaclysm::StatePaused::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);
}

void Kartaclysm::StatePaused::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		int iAccelerate, iBrake, iSlide;
		float fTurn;
		PlayerInputMapping::Instance()->QueryPlayerMovement(m_iPausedPlayer, iAccelerate, iBrake, iSlide, fTurn);
		fTurn *= -1.0f;

		if (iAccelerate)
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
		else if (iBrake)
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

	if (m_pPauseDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
		delete m_pPauseDelegate;
		m_pPauseDelegate = nullptr;
	}

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StatePaused::UnpauseGame(const HeatStroke::Event* p_pEvent)
{
	// Only the player who paused the game can unpause it
	int iPlayer = 0;
	p_pEvent->GetOptionalIntParameter("Player", iPlayer, iPlayer);

	if (m_iPausedPlayer == iPlayer)
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

		// Tell the HUD to stop rendering a pause message
		/*HeatStroke::Event* pHudEvent = new HeatStroke::Event("Pause_HUD");
		pHudEvent->SetIntParameter("Display", 0);
		HeatStroke::EventManager::Instance()->TriggerEvent(pHudEvent);*/
	}
}