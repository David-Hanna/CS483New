//------------------------------------------------------------------------
// StateRacing
// Author:	Bradley Cooper
//	
// Gameplay state for racing.
//------------------------------------------------------------------------

#include "StateRacing.h"
#include "KeyboardInputBuffer.h"

#include "CollisionManager.h"

Kartaclysm::StateRacing::StateRacing()
	:
	GameplayState("Racing"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_pRacerFinishedRaceDelegate(nullptr),
	m_pRaceFinishedDelegate(nullptr),
	m_pRaceRestartDelegate(nullptr),
	m_pPauseDelegate(nullptr),
	m_uiLapsNeeded(0),
	m_bCountdown(false)
{
}

Kartaclysm::StateRacing::~StateRacing()
{
}

void Kartaclysm::StateRacing::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_mRaceResults.clear();

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
	m_uiLapsNeeded = pTrackComponent->GetLapsToFinishTrack();

	// Load racers
	unsigned int uiNumHumanRacers = atoi(m_mContextParams.at("PlayerCount").c_str());
	CreateHUDForRacers(uiNumHumanRacers);
	for (unsigned int i = 0; i < uiNumHumanRacers; i++)
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

	if (PlayerInputMapping::Instance()->SetSplitscreenPlayers(uiNumHumanRacers))
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

	HeatStroke::GameObject* pKart = m_pGameObjectManager->CreateGameObject(p_strKartDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pDriver = m_pGameObjectManager->CreateGameObject(p_strDriverDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pCamera = m_pGameObjectManager->CreateGameObject(p_strCameraDefinitionFile, "", pRacer);
	pCamera->GetTransform().SetTranslation(glm::vec3(0.0f, 0.4f, -1.2f));

	ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(pRacer->GetComponent("GOC_Racer"));
	pRacerComponent->SetKart(pKart);
	pRacerComponent->SetDriver(pDriver);

	m_mRaceResults[p_strGuid].m_pRacerComponent = pRacerComponent;

	return pRacer;
}

HeatStroke::GameObject* Kartaclysm::StateRacing::GenerateAIRacer(
	int p_iIndex
)
{
	std::string strRacerDefinitionFile = "CS483/CS483/Kartaclysm/Data/Racer/racer_ai.xml";
	std::string strGuid = "ai_racer" + std::to_string(p_iIndex);
	HeatStroke::GameObject* pRacer = m_pGameObjectManager->CreateGameObject(strRacerDefinitionFile, strGuid);

	HeatStroke::GameObject* pKart = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/kart_speedster.xml", "ai_kart" + std::to_string(p_iIndex), pRacer);
	HeatStroke::GameObject* pDriver = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/driver_ai.xml", "ai_driver" + std::to_string(p_iIndex), pRacer);

	ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(pRacer->GetComponent("GOC_Racer"));
	pRacerComponent->SetKart(pKart);
	pRacerComponent->SetDriver(pDriver);
	pRacerComponent->SetHumanPlayer(false);
	
	ComponentKartController* kartController = static_cast<ComponentKartController*>(pRacer->GetComponent("GOC_KartController"));
	if (kartController != nullptr)
	{
		kartController->SetAI(true);
	}

	m_mRaceResults[strGuid].m_pRacerComponent = pRacerComponent;

	return pRacer;
}

void Kartaclysm::StateRacing::CreateHUDForRacers(unsigned int p_uiNumHumanRacers)
{
	switch (p_uiNumHumanRacers)
	{
	case 1:
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_full.xml", "Player0_HUD");
		break;
	case 2:
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top.xml", "Player0_HUD");
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom.xml", "Player1_HUD");
		break;
	case 3:
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_left.xml", "Player0_HUD");
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_right.xml", "Player1_HUD");
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom_left.xml", "Player2_HUD");
		break;
	case 4:
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_left.xml", "Player0_HUD");
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_top_right.xml", "Player1_HUD");
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom_left.xml", "Player2_HUD");
		m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud_bottom_right.xml", "Player3_HUD");
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

	unsigned int uiHighestPosition = 0;
	for (auto mRaceResult : m_mRaceResults)
	{
		if (mRaceResult.second.m_uiPosition > uiHighestPosition)
		{
			uiHighestPosition = mRaceResult.second.m_uiPosition;
		}
	}

	m_mRaceResults[strRacerId].m_uiPosition = ++uiHighestPosition;
	m_mRaceResults[strRacerId].m_fRaceTime = fRacerTime;

	if (uiHighestPosition == m_mRaceResults.size() - 1 &&
		m_mContextParams.at("Mode") == "Tournament")
	{
		ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(m_pGameObjectManager->GetGameObject(strRacerId)->GetComponent("GOC_Racer"));

		std::string strUnfinishedPlayerId = "";
		for (auto mRaceResult : m_mRaceResults)
		{
			if (pRacerComponent->GetLapTimes().size() != m_uiLapsNeeded)
			{
#ifdef _DEBUG
				assert(strUnfinishedPlayerId == "" && "More than one player waiting to finish");
#endif
				strUnfinishedPlayerId = mRaceResult.first;
			}
		}

		if (strUnfinishedPlayerId != "")
		{
			pRacerComponent = static_cast<ComponentRacer*>(m_pGameObjectManager->GetGameObject(strRacerId)->GetComponent("GOC_Racer"));
			auto pUnfinishedPlayer = &m_mRaceResults[strUnfinishedPlayerId];
			pUnfinishedPlayer->m_uiPosition = ++uiHighestPosition;

			while (pRacerComponent->GetLapTimes().size() < m_uiLapsNeeded)
			{
				pRacerComponent->ManuallyAddLapTime(600.0f);  // 10min
			}
		}
#ifdef _DEBUG
		else
		{
			assert(false && "Could not find player waiting to finish");
		}
#endif
	}

	if (uiHighestPosition == m_mRaceResults.size())
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

	mRaceResults.insert(std::pair<std::string, std::string>("numRacers", std::to_string(m_mRaceResults.size())));
	mRaceResults.insert(std::pair<std::string, std::string>("numLaps", std::to_string(m_uiLapsNeeded)));

	bool bTournament = (m_mContextParams.at("Mode") == "Tournament");
	for (auto mRaceResult : m_mRaceResults)
	{
		std::string strIndex = std::to_string(mRaceResult.second.m_uiPosition - 1);
		mRaceResults.insert(std::pair<std::string, std::string>("racerId" + strIndex, mRaceResult.first));
		mRaceResults.insert(std::pair<std::string, std::string>("racerTime" + strIndex, std::to_string(mRaceResult.second.m_fRaceTime)));

		std::string strDriver = "", strKart = "";
		GetDriverNameAndKartName(mRaceResult.second.m_pRacerComponent, strDriver, strKart);
		mRaceResults.insert(std::pair<std::string, std::string>("racerDriver" + strIndex, strDriver));
		mRaceResults.insert(std::pair<std::string, std::string>("racerKart" + strIndex, strKart));
		mRaceResults.insert(std::pair<std::string, std::string>("racerAI" + strIndex, mRaceResult.second.m_pRacerComponent->IsHumanPlayer() ? "0" : "1"));

		if (bTournament)
		{
			mRaceResults.insert(std::pair<std::string, std::string>("racerPoints" + strIndex, std::to_string(GetTournamentPoints(mRaceResult.second.m_uiPosition - 1))));
		}

		ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(m_pGameObjectManager->GetGameObject(mRaceResult.first)->GetComponent("GOC_Racer"));
		auto vLapTimes = pRacerComponent->GetLapTimes();
		int iLapCount = 0;
		for (auto fLapTime : vLapTimes)
		{
			mRaceResults.insert(std::pair<std::string, std::string>("racer" + strIndex + "Lap" + std::to_string(++iLapCount), std::to_string(fLapTime)));
		}

#ifdef _DEBUG
		assert(iLapCount == m_uiLapsNeeded);
#endif
	}

	std::string strTrack = static_cast<ComponentTrack*>(m_pGameObjectManager->GetGameObject("Track")->GetComponent("GOC_Track"))->GetTrackName();
	mRaceResults.insert(std::pair<std::string, std::string>("trackName", strTrack));
	mRaceResults.insert(std::pair<std::string, std::string>("screenTitle", "Race Complete!"));

	return mRaceResults;
}

void Kartaclysm::StateRacing::GetDriverNameAndKartName(ComponentRacer* p_pRacerComponent, std::string& p_strDriver, std::string& p_strKart) const
{
	auto mDriverTags = p_pRacerComponent->GetDriver()->GetTagList();
	if (mDriverTags.find("Cleopapa") != mDriverTags.end())
		p_strDriver = "Cleopapa";
	else if (mDriverTags.find("Clockmaker") != mDriverTags.end())
		p_strDriver = "Clockmaker";
	else if (mDriverTags.find("Kingpin") != mDriverTags.end())
		p_strDriver = "Kingpin";
	else
	{
		printf("%s\n", p_strDriver);
		assert(false && "Unknown driver name");
	}

	auto mKartTags = p_pRacerComponent->GetKart()->GetTagList();
	if (mKartTags.find("Juggernaut") != mKartTags.end())
		p_strKart = "Juggernaut";
	else if (mKartTags.find("Showoff") != mKartTags.end())
		p_strKart = "Showoff";
	else if (mKartTags.find("Speedster") != mKartTags.end())
		p_strKart = "Speedster";
	else
	{
		printf("%s\n", p_strKart);
		assert(false && "Unknown kart name");
	}
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
