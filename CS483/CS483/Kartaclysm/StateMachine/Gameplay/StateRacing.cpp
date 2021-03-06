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
	m_uiNumAIRacers(0),
	m_uiNumHumanRacers(0),
	m_uiNumRacers(0),
	m_uiLapsNeeded(0),
	m_bRaceEndCountdown(false),
	m_fTimeRemaining(-1.0f),
	m_fMaxTimeUntilDNF(20.0f)
{
}

Kartaclysm::StateRacing::~StateRacing()
{
}

void Kartaclysm::StateRacing::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
#ifdef _DEBUG
	assert(m_pStateMachine->size() == (p_mContextParameters.at("Mode") == "Tournament" ? 2 : 1));
#endif

	m_bSuspended = false;
	m_fTimeRemaining = -1.0f;
	m_vRaceResults.clear();
	m_sUnfinishedRacers.clear();

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
	m_pGameObjectManager->RegisterComponentFactory("GOC_RotateOverTime", HeatStroke::ComponentRotateOverTime::CreateComponent);
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
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_RaceResults", ComponentHudRaceResults::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_Track", ComponentTrack::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TrackPiece", ComponentTrackPiece::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AIDriver", ComponentAIDriver::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Racer", ComponentRacer::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_Effect", ComponentEffect::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_ObjectSpawn", ComponentObjectSpawn::CreateComponent);
	
	// Store passed context parameters and begin race
	m_mContextParams = p_mContextParameters;
	SendRaceInfoEvent();
	BeginRace();

	HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/engine.flac", true);

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
	m_bRaceEndCountdown = false;
	m_fTimeRemaining = -1.0f;
	m_vRaceResults.clear();
	m_sUnfinishedRacers.clear();

	// Destroy game objects in case we are restarting the race
	m_pGameObjectManager->DestroyAllGameObjects();

	// Load Lights and Tracks
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Lights/light.xml", "AmbientAndDirectionalLight");
	HeatStroke::GameObject* pTrack = m_pGameObjectManager->CreateGameObject(m_mContextParams.at("TrackDefinitionFile"), "Track");
	ComponentTrack* pTrackComponent = static_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));
	m_uiLapsNeeded = pTrackComponent->GetLapsToFinishTrack();

	// Load racers
	m_uiNumHumanRacers = atoi(m_mContextParams.at("NumHumanRacers").c_str());
	for (unsigned int i = 0; i < m_uiNumHumanRacers; i++)
	{
		std::string strPlayerX = "Player" + std::to_string(i);

		std::string strKartFile = m_mContextParams.at(strPlayerX + "_KartDefinitionFile");
		std::string strDriverFile = m_mContextParams.at(strPlayerX + "_DriverDefinitionFile");
		std::string strCameraFile = m_mContextParams.at(strPlayerX + "_CameraDefinitionFile");
		int iStartPosition = atoi(m_mContextParams.at(strPlayerX + "_StartPosition").c_str());

		// generate racers
		HeatStroke::GameObject* pRacer = GenerateRacer(strKartFile, strDriverFile, strCameraFile, strPlayerX);
		pTrackComponent->RegisterRacer(pRacer);
		pRacer->GetTransform().Translate(glm::vec3((iStartPosition % 2 == 0 ? -0.5f : 0.5f), 0.0f, -0.5f * iStartPosition));
	}

	m_uiNumAIRacers = atoi(m_mContextParams.at("NumAIRacers").c_str());
	for (unsigned int i = 0; i < m_uiNumAIRacers; ++i)
	{
		std::string strAIRacer = "AI_racer" + std::to_string(i);

		std::string strKartFile = m_mContextParams.at(strAIRacer + "_KartDefinitionFile");
		std::string strDriverFile = m_mContextParams.at(strAIRacer + "_DriverDefinitionFile");
		int iStartPosition = atoi(m_mContextParams.at(strAIRacer + "_StartPosition").c_str());

		HeatStroke::GameObject* pRacer = GenerateAIRacer(strKartFile, strDriverFile, strAIRacer);
		pTrackComponent->RegisterAIRacer(pRacer);
		pRacer->GetTransform().Translate(glm::vec3((iStartPosition % 2 == 0 ? -0.5f : 0.5f), 0.0f, -0.5f * iStartPosition));
	}

	m_uiNumRacers = m_uiNumHumanRacers + m_uiNumAIRacers;

	// Set inital position sprites on racer HUDs
	pTrackComponent->TriggerRaceStandingsUpdateEvent();

	if (PlayerInputMapping::Instance()->SetSplitscreenPlayers(m_uiNumHumanRacers))
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

	m_pStateMachine->Push(GameplayState::STATE_COUNTDOWN, m_mContextParams);
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

	m_sUnfinishedRacers.insert(pRacer->GetGUID());
	return pRacer;
}

HeatStroke::GameObject* Kartaclysm::StateRacing::GenerateAIRacer(
	const std::string& p_strKartDefinitionFile,
	const std::string& p_strDriverDefinitionFile,
	const std::string& p_strGuid /*= ""*/
)
{
	std::string strRacerDefinitionFile = "CS483/CS483/Kartaclysm/Data/Racer/racer_ai.xml";
	HeatStroke::GameObject* pRacer = m_pGameObjectManager->CreateGameObject(strRacerDefinitionFile, p_strGuid);

	HeatStroke::GameObject* pDriver = m_pGameObjectManager->CreateGameObject(p_strDriverDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pKart = m_pGameObjectManager->CreateGameObject(p_strKartDefinitionFile, "", pRacer);

	ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(pRacer->GetComponent("GOC_Racer"));
	pRacerComponent->SetKart(pKart);
	pRacerComponent->SetDriver(pDriver);
	pRacerComponent->SetHumanPlayer(false);
	
	ComponentKartController* kartController = static_cast<ComponentKartController*>(pRacer->GetComponent("GOC_KartController"));
	if (kartController != nullptr)
	{
		kartController->SetAI(true);
	}

	m_sUnfinishedRacers.insert(pRacer->GetGUID());
	return pRacer;
}

void Kartaclysm::StateRacing::CreateHUDForRacer(const std::string& p_strGuid)
{
	switch (m_uiNumHumanRacers)
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
	HeatStroke::AudioPlayer::Instance()->StopSoundEffect("Assets/Sounds/engine.flac");
}

void Kartaclysm::StateRacing::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);
	HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/engine.flac", true);
}

void Kartaclysm::StateRacing::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->Update(p_fDelta);

	if (m_bRaceEndCountdown)
	{
		m_fTimeRemaining -= p_fDelta;
		if (m_fTimeRemaining <= 0.0f)
		{
			HeatStroke::Event* pEvent = new HeatStroke::Event("RaceFinished");
			HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
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

	HeatStroke::AudioPlayer::Instance()->StopSoundEffect("Assets/Sounds/engine.flac");
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

	int iPosition = m_vRaceResults.size() + 1;

	m_vRaceResults.push_back({strRacerId, fRacerTime, iPosition});
	m_sUnfinishedRacers.erase(strRacerId);

	if (m_vRaceResults.size() == m_uiNumRacers - 1)
	{
		m_bRaceEndCountdown = true;
		m_fTimeRemaining = m_fMaxTimeUntilDNF;
	}
	else if (m_vRaceResults.size() >= m_uiNumRacers)
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

std::map<std::string, std::string> Kartaclysm::StateRacing::GenerateRaceResults()
{
	std::map<std::string, std::string> mRaceResults;
	mRaceResults.insert(std::pair<std::string, std::string>("numLaps", std::to_string(m_uiLapsNeeded)));
	mRaceResults.insert(std::pair<std::string, std::string>("numRacers", std::to_string(m_uiNumRacers)));
	mRaceResults.insert(std::pair<std::string, std::string>("numHumans", std::to_string(m_uiNumHumanRacers)));
	mRaceResults.insert(std::pair<std::string, std::string>("numAI", std::to_string(m_uiNumAIRacers)));
	
	float fDNFTime = m_vRaceResults.back().m_fRaceTime + m_fMaxTimeUntilDNF;
	for (std::string strRacerId : m_sUnfinishedRacers)
	{
		m_vRaceResults.push_back({ strRacerId, fDNFTime, -1 });
	}

	bool bTournament = (m_mContextParams.at("Mode") == "Tournament");
	for (unsigned int i = 0; i < m_uiNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);
		RacerResults results = m_vRaceResults[i];
		int iPosition = results.m_fRaceTime < fDNFTime ? i : -1;
		ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(m_pGameObjectManager->GetGameObject(results.m_strRacerId)->GetComponent("GOC_Racer"));

		mRaceResults.insert(std::pair<std::string, std::string>("racerId" + strIndex, results.m_strRacerId));
		mRaceResults.insert(std::pair<std::string, std::string>("racerTime" + strIndex, std::to_string(results.m_fRaceTime)));
		mRaceResults.insert(std::pair<std::string, std::string>("racerPosition" + strIndex, results.m_iPosition == -1 ? "dnf" : std::to_string(results.m_iPosition)));
		mRaceResults.insert(std::pair<std::string, std::string>("racerIsHuman" + strIndex, pRacerComponent->IsHumanPlayer() ? "1" : "0"));

		std::string strDriver = "", strKart = "";
		GetDriverAndKartNames(pRacerComponent, strDriver, strKart);
		mRaceResults.insert(std::pair<std::string, std::string>("racerDriver" + strIndex, strDriver));
		mRaceResults.insert(std::pair<std::string, std::string>("racerKart" + strIndex, strKart));

		if (bTournament)
		{
			mRaceResults.insert(std::pair<std::string, std::string>("racerPoints" + strIndex, std::to_string(GetTournamentPoints(i))));
		}

		auto vLapTimes = pRacerComponent->GetLapTimes();
#ifdef _DEBUG
		if (vLapTimes.size() < m_uiLapsNeeded)
		{
			assert(iPosition == -1); // must be DNF
		}
		else
		{
			assert(vLapTimes.size() == m_uiLapsNeeded); // cannot be greater
		}
#endif

		for (unsigned int i = 0; i < m_uiLapsNeeded; ++i)
		{
			float fLapTime = 600.0f; // 10 min default for database
			if (i < vLapTimes.size())
			{
				fLapTime = vLapTimes[i];
			}
			mRaceResults.insert(std::pair<std::string, std::string>("racer" + strIndex + "Lap" + std::to_string(i+1), std::to_string(fLapTime)));
		}
	}

	std::string strTrack = static_cast<ComponentTrack*>(m_pGameObjectManager->GetGameObject("Track")->GetComponent("GOC_Track"))->GetTrackName();
	mRaceResults.insert(std::pair<std::string, std::string>("trackName", strTrack));
	mRaceResults.insert(std::pair<std::string, std::string>("screenTitle", "Race Complete!"));

	return mRaceResults;
}

void Kartaclysm::StateRacing::GetDriverAndKartNames(ComponentRacer* p_pRacerComponent, std::string& p_strDriver, std::string& p_strKart) const
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
		printf("%s\n", p_strDriver.c_str());
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
		printf("%s\n", p_strKart.c_str());
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
