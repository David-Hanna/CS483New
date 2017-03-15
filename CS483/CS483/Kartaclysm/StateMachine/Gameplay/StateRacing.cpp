//------------------------------------------------------------------------
// StateRacing
// Author:	Bradley Cooper
//	
// Gameplay state for racing.
//------------------------------------------------------------------------

#include "StateRacing.h"
#include "KeyboardInputBuffer.h"

#include "CollisionManager.h"

// TODO - move to custom Random class once it's complete
std::random_device Kartaclysm::StateRacing::s_Rand;
std::mt19937 Kartaclysm::StateRacing::s_RNGesus = std::mt19937(Kartaclysm::StateRacing::s_Rand());

Kartaclysm::StateRacing::StateRacing()
	:
	GameplayState("Racing"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_pPauseDelegate(nullptr),
	m_uiNumRacers(0),
	m_bCountdown(false)
{
	GenerateRandomDriverList();
	GenerateRandomKartList();
}

Kartaclysm::StateRacing::~StateRacing()
{
}

void Kartaclysm::StateRacing::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_vRaceResults.clear();

	// Register listeners
	m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::PauseGame, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);

	m_pRacerFinishedRaceDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::RacerFinishedRace, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RacerFinishedRace", m_pRacerFinishedRaceDelegate);

	m_pRaceFinishedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::FinishRace, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RaceFinished", m_pRaceFinishedDelegate);

	m_pRaceRestartDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::RestartRace, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RaceRestart", m_pRaceRestartDelegate);

	// Initialize our GameObjectManager
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	// Register component factory methods
	m_pGameObjectManager->RegisterComponentFactory("GOC_3DModel", HeatStroke::Component3DModel::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AmbientLight", HeatStroke::ComponentAmbientLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_DirectionalLight", HeatStroke::ComponentDirectionalLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PointLight", HeatStroke::ComponentPointLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_SphereCollider", HeatStroke::ComponentSphereCollider::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_WallCollider", HeatStroke::ComponentWallCollider::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_ParticleEffect", HeatStroke::ComponentParticleEffect::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_KartStats", ComponentKartStats::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_DriverStatMods", ComponentDriverStatMods::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_AbilityConditions", ComponentAbilityConditions::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AbilityIcon", ComponentAbilityIcon::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_BoostAbility", ComponentBoostAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_WheelieAbility", ComponentWheelieAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_ArmorPlateAbility", ComponentArmorPlateAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_MaintainAbility", ComponentMaintainAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_StrikeAbility", ComponentStrikeAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TurkeyAbility", ComponentTurkeyAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_ClockAbility", ComponentClockAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TinkerAbility", ComponentTinkerAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_BedazzleAbility", ComponentBedazzleAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_RainAbility", ComponentRainAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Projectile", ComponentProjectile::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_SelfDestruct", ComponentSelfDestruct::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_SimplePhysics", ComponentSimplePhysics::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Ability", ComponentHudAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_RaceTimer", ComponentHudRaceTimer::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Position", ComponentHudPosition::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_LapCount", ComponentHudLapCount::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Countdown", ComponentHudCountdown::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Fps", ComponentHudFps::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Popup", ComponentHudPopup::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_Track", ComponentTrack::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TrackPiece", ComponentTrackPiece::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AIDriver", ComponentAIDriver::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Racer", ComponentRacer::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_Effect", ComponentEffect::CreateComponent);
	
	// Store passed context parameters and begin race
	m_mContextParams = p_mContextParameters;
	SendRaceInfoEvent();
	BeginRace();

	HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/engine.wav", true);

	// I have to do this horrible spaghetti crap because game object manager isn't a SINGLETON
	HeatStroke::CollisionManager::Instance()->SetGameObjectManager(m_pGameObjectManager);
}

void Kartaclysm::StateRacing::SendRaceInfoEvent()
{
	HeatStroke::Event* pEvent = new HeatStroke::Event("RaceInfo");
	auto it = m_mContextParams.begin(), end = m_mContextParams.end();
	for (; it != end; ++it)
	{
		pEvent->SetStringParameter(it->first, it->second);
	}
	HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
}

void Kartaclysm::StateRacing::BeginRace()
{
	// Destroy game objects in case we are restarting the race
	m_pGameObjectManager->DestroyAllGameObjects();

	// Load Lights and Tracks
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Lights/light.xml", "AmbientAndDirectionalLight");
	HeatStroke::GameObject* pTrack = m_pGameObjectManager->CreateGameObject(m_mContextParams.at("TrackDefinitionFile"), "Track");
	ComponentTrack* pTrackComponent = static_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));

	// Load racers
	m_uiNumRacers = atoi(m_mContextParams.at("PlayerCount").c_str());
	for (unsigned int i = 0; i < m_uiNumRacers; i++)
	{
		std::string strPlayerX = "Player" + std::to_string(i);

		std::string kartFile = m_mContextParams.at(strPlayerX + "_KartDefinitionFile");
		std::string driverFile = m_mContextParams.at(strPlayerX + "_DriverDefinitionFile");
		std::string cameraFile = m_mContextParams.at(strPlayerX + "_CameraDefinitionFile");
		int startPosition = atoi(m_mContextParams.at(strPlayerX + "_StartPosition").c_str());

		// generate racers
		HeatStroke::GameObject* pRacer = GenerateRacer(kartFile, driverFile, cameraFile, strPlayerX);
		pTrackComponent->RegisterRacer(pRacer);
		pRacer->GetTransform().Translate(glm::vec3((startPosition % 2 == 0 ? -0.5f : 0.5f), 0.0f, -0.5f * startPosition));
	}

	// AI racers
	HeatStroke::GameObject* pAIRacer1 = GenerateAIRacer(1);
	pTrackComponent->RegisterAIRacer(pAIRacer1);
	pAIRacer1->GetTransform().Translate(glm::vec3(-1.0f, 0.0f, -1.0f));

	// Uncomment additional AI racers at your peril!! ya dingus
	//HeatStroke::GameObject* pAIRacer2 = GenerateAIRacer(2);
	//pTrackComponent->RegisterAIRacer(pAIRacer2);
	//pAIRacer2->GetTransform().Translate(glm::vec3(0.0f, 0.0f, -2.0f));

	//HeatStroke::GameObject* pAIRacer3 = GenerateAIRacer(3);
	//pTrackComponent->RegisterAIRacer(pAIRacer3);
	//pAIRacer3->GetTransform().Translate(glm::vec3(1.0f, 0.0f, -3.0f));

	// Set inital position sprites on racer HUDs
	pTrackComponent->TriggerRaceStandingsUpdateEvent();

	if (PlayerInputMapping::Instance()->SetSplitscreenPlayers(m_uiNumRacers))
	{
		// TODO: Properly handle race mode
		//PlayerInputMapping::Instance()->EnableRaceMode();
	}
	else
	{
#ifdef _DEBUG
		assert(false && "Failed to set number of players.");
#endif
	}

	// Even if this song is playing, we want it to restart when the race restarts too
	HeatStroke::AudioPlayer::Instance()->StopMusic();
	HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/RocketPower.ogg");
	HeatStroke::AudioPlayer::Instance()->PlayMusic();

	// Set conditions for beginning countdown
	m_bCountdown = true;
	HeatStroke::Event* pDisableEvent = new HeatStroke::Event("KartCountdown");
	pDisableEvent->SetIntParameter("Disable", 1);
	HeatStroke::EventManager::Instance()->TriggerEvent(pDisableEvent);
}

HeatStroke::GameObject* Kartaclysm::StateRacing::GenerateRacer
(
	const std::string& p_strKartDefinitionFile, 
	const std::string& p_strDriverDefinitionFile, 
	const std::string& p_strCameraDefinitionFile, 
	const std::string& p_strGuid /*= ""*/
)
{
	std::string strRacerDefinitionFile = "CS483/CS483/Kartaclysm/Data/Racer/racer.xml";
	HeatStroke::GameObject* pRacer = m_pGameObjectManager->CreateGameObject(strRacerDefinitionFile, p_strGuid);

	CreateHUDForRacer(p_strGuid);

	HeatStroke::GameObject* pKart = m_pGameObjectManager->CreateGameObject(p_strKartDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pDriver = m_pGameObjectManager->CreateGameObject(p_strDriverDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pCamera = m_pGameObjectManager->CreateGameObject(p_strCameraDefinitionFile, "", pRacer);
	pCamera->GetTransform().SetTranslation(glm::vec3(0.0f, 0.4f, -1.2f));

	ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(pRacer->GetComponent("GOC_Racer"));
	pRacerComponent->SetKart(pKart);
	pRacerComponent->SetDriver(pDriver);

	return pRacer;
}

// TODO - move selection of AI kart/drivers to StatePlayerSelectionMenu
//			Waiting for StatePlayerSelectionMenu to be changed to allow for determining number of AI racers
HeatStroke::GameObject* Kartaclysm::StateRacing::GenerateAIRacer(
	int p_iIndex
)
{
	std::string strRacerDefinitionFile = "CS483/CS483/Kartaclysm/Data/Racer/racer_ai.xml";
	HeatStroke::GameObject* pRacer = m_pGameObjectManager->CreateGameObject(strRacerDefinitionFile, "ai_racer" + std::to_string(p_iIndex));

	std::string strDriverDefinitionFile = GetRandomDriver();
	std::string strKartDefinitionFile = GetRandomKart();
	HeatStroke::GameObject* pDriver = m_pGameObjectManager->CreateGameObject(strDriverDefinitionFile, "ai_driver" + std::to_string(p_iIndex), pRacer);
	HeatStroke::GameObject* pKart = m_pGameObjectManager->CreateGameObject(strKartDefinitionFile, "ai_kart" + std::to_string(p_iIndex), pRacer);

	ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(pRacer->GetComponent("GOC_Racer"));
	pRacerComponent->SetKart(pKart);
	pRacerComponent->SetDriver(pDriver);
	
	ComponentKartController* kartController = static_cast<ComponentKartController*>(pRacer->GetComponent("GOC_KartController"));
	if (kartController != nullptr)
	{
		kartController->SetAI(true);
	}

	return pRacer;
}

std::string Kartaclysm::StateRacing::GetRandomDriver() const
{
	auto range = std::uniform_int_distribution<int>(0, m_vDrivers.size() - 1);
	return m_vDrivers[range(s_RNGesus)];
}

std::string Kartaclysm::StateRacing::GetRandomKart() const
{
	auto range = std::uniform_int_distribution<int>(0, m_vKarts.size() - 1);
	return m_vKarts[range(s_RNGesus)];
}

void Kartaclysm::StateRacing::CreateHUDForRacer(const std::string& p_strGuid)
{
	switch (m_uiNumRacers)
	{
	case 1:
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_full.xml", p_strGuid + "_HUD");
		break;
	case 2:
		if (p_strGuid == "Player0")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top.xml", p_strGuid + "_HUD");
		}
		else if (p_strGuid == "Player1")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom.xml", p_strGuid + "_HUD");
		}
		break;
	case 3:
		if (p_strGuid == "Player0")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_left.xml", p_strGuid + "_HUD");
		}
		else if (p_strGuid == "Player1")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_right.xml", p_strGuid + "_HUD");
		}
		else if (p_strGuid == "Player2")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom_left.xml", p_strGuid + "_HUD");
		}
		break;
	case 4:
		if (p_strGuid == "Player0")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_left.xml", p_strGuid + "_HUD");
		}
		else if (p_strGuid == "Player1")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_right.xml", p_strGuid + "_HUD");
		}
		else if (p_strGuid == "Player2")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom_left.xml", p_strGuid + "_HUD");
		}
		else if (p_strGuid == "Player3")
		{
			m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom_right.xml", p_strGuid + "_HUD");
		}
		break;
	default:
		assert(false && "Unknown number of players.");
		break;
	}
}

void Kartaclysm::StateRacing::Suspend(const int p_iNewState)
{
	m_bSuspended = (p_iNewState != STATE_COUNTDOWN);
	HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
	HeatStroke::AudioPlayer::Instance()->StopSoundEffect("Assets/Sounds/engine.wav");
}

void Kartaclysm::StateRacing::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);
	HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/engine.wav", true);
}

void Kartaclysm::StateRacing::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		// TODO: Currently needs to start countdown after it has updated once, otherwise it does not render models
		if (m_bCountdown)
		{
			m_bCountdown = false;
			m_pStateMachine->Push(GameplayState::STATE_COUNTDOWN);
			return;
		}
	}
}

void Kartaclysm::StateRacing::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateRacing::Exit()
{
	m_bSuspended = true;

	PlayerInputMapping::Instance()->DisableRaceMode();

	if (m_pPauseDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
		delete m_pPauseDelegate;
		m_pPauseDelegate = nullptr;
	}

	if (m_pRacerFinishedRaceDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RacerFinishedRace", m_pRacerFinishedRaceDelegate);
		delete m_pRacerFinishedRaceDelegate;
		m_pRacerFinishedRaceDelegate = nullptr;
	}

	if (m_pRaceFinishedDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RaceFinished", m_pRaceFinishedDelegate);
		delete m_pRaceFinishedDelegate;
		m_pRaceFinishedDelegate = nullptr;
	}

	if (m_pRaceRestartDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RaceRestart", m_pRaceRestartDelegate);
		delete m_pRaceRestartDelegate;
		m_pRaceRestartDelegate = nullptr;
	}

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	HeatStroke::AudioPlayer::Instance()->StopSoundEffect("Assets/Sounds/engine.wav");
}

void Kartaclysm::StateRacing::PauseGame(const HeatStroke::Event* p_pEvent)
{
	// Get the player who paused the game
	int iPlayer = 0;
	p_pEvent->GetOptionalIntParameter("Player", iPlayer, iPlayer);

	// Create context for pushing to pause state
	HeatStroke::StateMachine::ContextParameters mContext = HeatStroke::StateMachine::ContextParameters();
	mContext["Player"] = std::to_string(iPlayer);
	mContext["Mode"] = m_mContextParams.at("Mode");

	// Push pause state
	m_pStateMachine->Push(STATE_PAUSED, mContext);
}

void Kartaclysm::StateRacing::RacerFinishedRace(const HeatStroke::Event* p_pEvent)
{
	std::string strRacerId = "";
	p_pEvent->GetRequiredStringParameter("racerId", strRacerId);

	float fRacerTime = 0.0f;
	p_pEvent->GetRequiredFloatParameter("racerTime", fRacerTime);

	m_vRaceResults.push_back({strRacerId, fRacerTime});

	if (m_mContextParams.at("Mode") == "Tournament" && 
		m_vRaceResults.size() == m_uiNumRacers - 1)
	{
		std::set<std::string> vFinishedRacerIds;
		for (auto mRaceResult : m_vRaceResults)
		{
			vFinishedRacerIds.insert(mRaceResult.m_strRacerId);
		}

		std::string strRacerIdLeftToFinishRace;
		for (unsigned int i = 0; i < m_uiNumRacers; ++i)
		{
			strRacerIdLeftToFinishRace = "Player" + std::to_string(i);
			if (vFinishedRacerIds.find(strRacerIdLeftToFinishRace) == vFinishedRacerIds.end())
			{
				break;
			}
		}

		m_vRaceResults.push_back({ strRacerIdLeftToFinishRace, fRacerTime += 0.01f });
	}

	if (m_vRaceResults.size() >= m_uiNumRacers)
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RaceFinished");
		HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
	}
}

void Kartaclysm::StateRacing::FinishRace(const HeatStroke::Event* p_pEvent)
{
	std::map<std::string, std::string> mRaceResults = GenerateRaceResults();
	m_pStateMachine->Pop();
	m_pStateMachine->Push(STATE_RACE_COMPLETE_MENU, mRaceResults);
}

std::map<std::string, std::string> Kartaclysm::StateRacing::GenerateRaceResults() const
{
	std::map<std::string, std::string> mRaceResults;

	mRaceResults.insert(std::pair<std::string, std::string>("numRacers", std::to_string(m_uiNumRacers)));

	bool bTournament = (m_mContextParams.at("Mode") == "Tournament");
	for (unsigned int i = 0; i < m_uiNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);
		mRaceResults.insert(std::pair<std::string, std::string>("racerId" + strIndex, m_vRaceResults[i].m_strRacerId));
		mRaceResults.insert(std::pair<std::string, std::string>("racerTime" + strIndex, std::to_string(m_vRaceResults[i].m_fRaceTime)));

		if (bTournament)
		{
			mRaceResults.insert(std::pair<std::string, std::string>("racerPoints" + strIndex, std::to_string(GetTournamentPoints(i))));
		}
	}

	std::string strTrack = static_cast<ComponentTrack*>(m_pGameObjectManager->GetGameObject("Track")->GetComponent("GOC_Track"))->GetTrackName();
	mRaceResults.insert(std::pair<std::string, std::string>("trackName", strTrack));
	mRaceResults.insert(std::pair<std::string, std::string>("screenTitle", "Race Complete!"));

	return mRaceResults;
}

int Kartaclysm::StateRacing::GetTournamentPoints(int p_iPosition) const
{
	switch (p_iPosition)
	{
	case 0: return 10;
	case 1: return 8;
	case 2: return 6;
	case 3: return 4;
	case 4: return 3;
	case 5: return 2;
	case 6: return 1;
	default: return 0;
	}
}

void Kartaclysm::StateRacing::GenerateRandomDriverList()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile("CS483/CS483/Kartaclysm/Data/Racer/drivers.xml");
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	tinyxml2::XMLElement* pRootElement = doc.RootElement();
	tinyxml2::XMLElement* pDriverElement = pRootElement->FirstChildElement("Driver");
	while (pDriverElement != nullptr)
	{
		std::string strDriverDefinitionPath = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(pDriverElement, "definition", strDriverDefinitionPath);
		m_vDrivers.push_back(strDriverDefinitionPath);

		pDriverElement = pDriverElement->NextSiblingElement("Driver");
	}
}

void Kartaclysm::StateRacing::GenerateRandomKartList()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile("CS483/CS483/Kartaclysm/Data/Racer/karts.xml");
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	tinyxml2::XMLElement* pRootElement = doc.RootElement();
	tinyxml2::XMLElement* pKartElement = pRootElement->FirstChildElement("Kart");
	while (pKartElement != nullptr)
	{
		std::string strKartDefinitionPath = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(pKartElement, "definition", strKartDefinitionPath);
		m_vKarts.push_back(strKartDefinitionPath);

		pKartElement = pKartElement->NextSiblingElement("Kart");
	}
}