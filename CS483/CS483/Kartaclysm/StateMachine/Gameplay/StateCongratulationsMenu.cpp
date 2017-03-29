//------------------------------------------------------------------------
// StateCongratulationsMenu
// Author:	Bradley Cooper
//	
// Shows a congratulations message after a tournament.
// Continues to main menu.
//------------------------------------------------------------------------

#include "StateCongratulationsMenu.h"

Kartaclysm::StateCongratulationsMenu::StateCongratulationsMenu()
	:
	GameplayState("Congratulations"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_bSkipToMainMenu(false),
	m_vMedals()
{
}

Kartaclysm::StateCongratulationsMenu::~StateCongratulationsMenu()
{
}

void Kartaclysm::StateCongratulationsMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	std::map<std::string, std::string> mResults = p_mContextParameters;
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_menu.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/background.xml");



	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateCongratulationsMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
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
}

void Kartaclysm::StateCongratulationsMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateCongratulationsMenu::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}