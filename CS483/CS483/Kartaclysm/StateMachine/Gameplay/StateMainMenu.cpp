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
	m_bRenderedOnce(false)
{
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

void Kartaclysm::StateMainMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (!m_bPreloadCalled)
		{
			if (m_bRenderedOnce)
			{
				m_bPreloadCalled = true;

				std::set<Database::TrackPK> vTrackIds;
				vTrackIds.insert(Database::eNoobZone);
				vTrackIds.insert(Database::eShiftRift);
				vTrackIds.insert(Database::eUpAndOver);

				auto thrTrackTimeQuery = std::async(
					&DatabaseManager::SelectFastestTimes,
					DatabaseManager::Instance(),
					vTrackIds,
					1);

				std::thread thrLoadSoundEffects(
					&HeatStroke::AudioPlayer::PreloadSoundEffects,
					HeatStroke::AudioPlayer::Instance(),
					"CS483/CS483/Kartaclysm/Data/DevConfig/Preload.xml");

				HeatStroke::ModelManager::Instance()->Preload("CS483/CS483/Kartaclysm/Data/DevConfig/Preload.xml");
				
				thrLoadSoundEffects.join(); // blocks execution
				DatabaseManager::TrackTimes mTrackTimes = thrTrackTimeQuery.get(); // blocks execution
				SendTrackTimesEvent(mTrackTimes, vTrackIds);

				m_pGameObjectManager->DestroyGameObject(m_pGameObjectManager->GetGameObject("LoadingMessage"));
				m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/MainMenu/press_start.xml", "PressStart");
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/load.wav");
			}
		}
		else if (bConfirm)
		{
			m_pStateMachine->Pop();
			m_pStateMachine->Push(STATE_MODE_SELECTION_MENU);
		}
	}
}

void Kartaclysm::StateMainMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
	m_bRenderedOnce = true;
}

void Kartaclysm::StateMainMenu::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StateMainMenu::SendTrackTimesEvent(const DatabaseManager::TrackTimes& p_mTrackTimes, const std::set<Database::TrackPK>& p_vTrackIds) const
{
	if (p_mTrackTimes.empty()) return;

	HeatStroke::Event* pEvent = new HeatStroke::Event("TrackTimeScreen");
	unsigned int uiCount = 0;

	for (auto eTrackId : p_vTrackIds)
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

