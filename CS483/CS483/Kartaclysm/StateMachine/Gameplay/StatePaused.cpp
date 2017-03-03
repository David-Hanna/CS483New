//------------------------------------------------------------------------
// StatePaused
// Author:	Bradley Cooper
//	
// Gameplay state for pausing during a race.
//------------------------------------------------------------------------

#include "StatePaused.h"

Kartaclysm::StatePaused::StatePaused()
	:
	GameplayState("Pause State"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_iPausedPlayer(-1),
	m_bTournament(false)
{
}

Kartaclysm::StatePaused::~StatePaused()
{
}

void Kartaclysm::StatePaused::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_mContextParameters = p_mContextParameters;

	// Initialize our GameObjectManager
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_overlay.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_options.xml");
	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_continue.xml");

	m_bTournament = (p_mContextParameters.at("Mode") == "Tournament");
	if (m_bTournament)
	{
		// Cannot restart race during a tournament
		// TODO: Another quick fix to destroy game object before it renders for one frame
		m_pGameObjectManager->ForceInstantDestroyGameObject(m_pGameObjectManager->GetGameObject("restart"));
	}

	m_iOptionSelection = 0;
	m_bSkipFirstFrame = true;
	m_iPausedPlayer = atoi(p_mContextParameters.at("Player").c_str());

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/RocketPower.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/RocketPower.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StatePaused::Suspend(const int p_iNewState)
{ 
	m_bSuspended = true;
	Exit();
}
void Kartaclysm::StatePaused::Unsuspend(const int p_iPrevState)
{ 
	m_bSuspended = false;
	Enter(m_mContextParameters);
}

void Kartaclysm::StatePaused::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

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
		switch (m_iOptionSelection)
		{
		case 0: // continue
			m_pStateMachine->Pop();
			break;
		case 1: // options
			m_pStateMachine->Push(STATE_OPTIONS_MENU);
			break;
		case 2: // restart (skipped in tournament mode)
			assert(!m_bTournament && "Cannot restart race during tournament mode");
			m_pStateMachine->Pop();
			HeatStroke::EventManager::Instance()->TriggerEvent(new HeatStroke::Event("RaceRestart"));
			break;
		case 3: // quit
			m_pStateMachine->Pop(); // pause
			m_pStateMachine->Pop(); // race
			if (m_pStateMachine->empty()) // may have tournament state
			{
				m_pStateMachine->Push(STATE_MAIN_MENU);
			}
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
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_options.xml");
			break;
		case 3:
			if (m_bTournament)
			{
				m_iOptionSelection = 1;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_options.xml");
			}
			else
			{
				m_iOptionSelection = 2;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_restart.xml");
			}
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
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_options.xml");
			break;
		case 1:
			if (m_bTournament)
			{
				m_iOptionSelection = 3;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_quit.xml");
			}
			else
			{
				m_iOptionSelection = 2;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_restart.xml");
			}
			break;
		case 2:
			m_iOptionSelection = 3;
			m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
			m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PauseMenu/pause_highlight_quit.xml");
			break;
		}
	}
}

void Kartaclysm::StatePaused::PreRender()
{
	if (m_bSuspended) return;

	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StatePaused::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}
