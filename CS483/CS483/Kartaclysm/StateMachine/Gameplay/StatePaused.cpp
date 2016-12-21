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
	HeatStroke::Event* pHudEvent = new HeatStroke::Event("Pause_HUD");
	pHudEvent->SetIntParameter("Display", 1);
	HeatStroke::EventManager::Instance()->TriggerEvent(pHudEvent);

	// Register listening for pause
	m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StatePaused::UnpauseGame, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);

	// Initialize our GameObjectManager
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	// Register component factory methods

	// Handle passed context parameters
	auto it = p_mContextParameters.find("Player");
	if (it != p_mContextParameters.end())
	{
		m_iPausedPlayer = atoi(it->second.c_str());
	}

	// Load the GameObjects from XML.
}

void Kartaclysm::StatePaused::Suspend(const int p_iNewState)
{
	m_bSuspended = true;

	if (m_pPauseDelegate != nullptr)
	{
		// If this is the first frame suspended: remove listener
		HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
		delete m_pPauseDelegate;
		m_pPauseDelegate = nullptr;
	}
}

void Kartaclysm::StatePaused::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;
	
	if (m_pPauseDelegate == nullptr)
	{
		m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StatePaused::UnpauseGame, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);
	}
}

void Kartaclysm::StatePaused::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);
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
	// Get the player who paused the game
	int iPlayer = 0;
	p_pEvent->GetOptionalIntParameter("Player", iPlayer, iPlayer);

	// Pop current state
	m_pStateMachine->Pop();

	// Tell the HUD to stop rendering a pause message
	HeatStroke::Event* pHudEvent = new HeatStroke::Event("Pause_HUD");
	pHudEvent->SetIntParameter("Display", 0);
	HeatStroke::EventManager::Instance()->TriggerEvent(pHudEvent);
}