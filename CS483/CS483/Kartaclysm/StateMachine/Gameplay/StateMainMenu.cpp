//------------------------------------------------------------------------
// StateMainMenu
// Author:	David Hanna
//	
// Main menu state shows title screen and continues with enter key.
//------------------------------------------------------------------------

#include "StateMainMenu.h"

Kartaclysm::StateMainMenu::StateMainMenu()
	:
	GameplayState("Main Menu"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_bPreloadCalled(false),
	m_bRenderedOnce(false),
	m_bRunTrackTimeQuery(true),
	m_vTrackIds()
{
	m_vTrackIds.insert(Database::eNoobZone);
	m_vTrackIds.insert(Database::eShiftRift);
	m_vTrackIds.insert(Database::eUpAndOver);
}

Kartaclysm::StateMainMenu::~StateMainMenu()
{
}

void Kartaclysm::StateMainMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_bRenderedOnce = false;

	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_menu.xml", "Camera");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/background.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/title_image.xml", "TitleImage");

	if (!m_bPreloadCalled)
	{
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/loading_message.xml", "LoadingMessage");
	}
	else if (m_bRunTrackTimeQuery)
	{
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/checking_internet.xml", "CheckingInternet");
	}
	else
	{
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/press_start.xml", "PressStart");
	}

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateMainMenu::Suspend(const int p_iNewState)
{
	Exit();
	m_bRunTrackTimeQuery = false;
}

void Kartaclysm::StateMainMenu::Unsuspend(const int p_iPrevState)
{
	Enter(std::map<std::string, std::string>());
}

void Kartaclysm::StateMainMenu::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->Update(p_fDelta);

	bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
	PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

	if (!m_bRenderedOnce) return;

	if (!m_bPreloadCalled)
	{
		m_bPreloadCalled = true;
		m_bRunTrackTimeQuery = false;

		std::future<DatabaseManager::TrackTimes> thrTrackTimeQuery = CreateTrackTimesThread();

		std::thread thrLoadSoundEffects(
			&HeatStroke::AudioPlayer::PreloadSoundEffects,
			HeatStroke::AudioPlayer::Instance(),
			"CS483/CS483/Kartaclysm/Data/DevConfig/Preload.xml");

		HeatStroke::ModelManager::Instance()->Preload("CS483/CS483/Kartaclysm/Data/DevConfig/Preload.xml");
		
		thrLoadSoundEffects.join(); // blocks execution until thread ends
		EndTrackTimesThread(thrTrackTimeQuery); // blocks execution

		m_pGameObjectManager->DestroyGameObject(m_pGameObjectManager->GetGameObject("LoadingMessage"));
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/press_start.xml", "PressStart");
		HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/load.wav");
	}
	else if (m_bRunTrackTimeQuery)
	{
		m_bRunTrackTimeQuery = false;

		// Because database is in threaded mode, we must (redundantly) put the query into a thread
		EndTrackTimesThread(CreateTrackTimesThread());

		m_pGameObjectManager->DestroyGameObject(m_pGameObjectManager->GetGameObject("CheckingInternet"));
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/press_start.xml", "PressStart");
		HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/load.wav");
	}
	else if (bConfirm)
	{
		m_pStateMachine->Pop();
		m_pStateMachine->Push(STATE_MODE_SELECTION_MENU);
	}
}

void Kartaclysm::StateMainMenu::PreRender()
{
	if (m_bSuspended) return;
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
	m_bRenderedOnce = true;
}

void Kartaclysm::StateMainMenu::Exit()
{
	m_bSuspended = true;
	m_bRunTrackTimeQuery = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StateMainMenu::SendTrackTimesEvent(const DatabaseManager::TrackTimes& p_mTrackTimes) const
{
	if (p_mTrackTimes.empty()) return;

	HeatStroke::Event* pEvent = new HeatStroke::Event("TrackTimeScreen");
	unsigned int uiCount = 0;

	for (auto eTrackId : m_vTrackIds)
	{
		auto find = p_mTrackTimes.find(eTrackId);
		if (find == p_mTrackTimes.end()) continue;

		auto vTimes = find->second;
		if (vTimes.empty()) continue;

		std::string strTrack = Database::TrackPKToString(eTrackId);
		assert(strTrack != "");

		pEvent->SetStringParameter("track" + std::to_string(uiCount++), strTrack);
		pEvent->SetIntParameter(strTrack + "Count", vTimes.size());
		for (unsigned int i = 0; i < vTimes.size(); ++i)
		{
			pEvent->SetFloatParameter(strTrack + std::to_string(i), vTimes.at(i));
		}
	}

	pEvent->SetIntParameter("trackCount", uiCount);
	HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
}

std::future<Kartaclysm::DatabaseManager::TrackTimes> Kartaclysm::StateMainMenu::CreateTrackTimesThread() const
{
	DatabaseManager::Instance()->TryToConnect();

	return std::async(
		&DatabaseManager::SelectFastestTimes,
		DatabaseManager::Instance(),
		m_vTrackIds,
		1);
}

void Kartaclysm::StateMainMenu::EndTrackTimesThread(std::future<Kartaclysm::DatabaseManager::TrackTimes>& p_thrTrackTimeQuery) const
{
	DatabaseManager::TrackTimes mTrackTimes = p_thrTrackTimeQuery.get(); // blocks execution
	SendTrackTimesEvent(mTrackTimes);

	if (mTrackTimes.empty())
	{
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/no_internet.xml", "NoInternet");
	}
}