//------------------------------------------------------------------------
// StateRacing
// Author:	Bradley Cooper
//	
// Gameplay state for racing.
//------------------------------------------------------------------------

#include "StateRacing.h"

//------------------------------------------------------------------------------
// Method:    StateRacing
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
Kartaclysm::StateRacing::StateRacing()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_pPauseDelegate(nullptr)
{
}

//------------------------------------------------------------------------------
// Method:    ~StateRacing
// Returns:   
// 
// Destructor.
//------------------------------------------------------------------------------
Kartaclysm::StateRacing::~StateRacing()
{
	Exit();
}

//------------------------------------------------------------------------------
// Method:		Enter
// Parameter:	std::map<std::string, std::string> p_mContextParameters - parameters for state
// 
// Called when this state is pushed onto the stack.
// TODO: change context params from std::map<std::string, std::string> to be more like Event params
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	// Register listening for pause
	m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::PauseGame, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);

	m_pRaceFinishedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::FinishRace, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RacerFinishedRace2", m_pRaceFinishedDelegate);

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

	m_pGameObjectManager->RegisterComponentFactory("GOC_AbilityConditions", ComponentAbilityConditions::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AbilityIcon", ComponentAbilityIcon::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_BoostAbility", ComponentBoostAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_WheelieAbility", ComponentWheelieAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_ArmorPlateAbility", ComponentArmorPlateAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_MaintainAbility", ComponentMaintainAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_StrikeAbility", ComponentStrikeAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TurkeyAbility", ComponentTurkeyAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Projectile", ComponentProjectile::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_SelfDestruct", ComponentSelfDestruct::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_SimplePhysics", ComponentSimplePhysics::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Ability", ComponentHudAbility::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_RaceTimer", ComponentHudRaceTimer::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Position", ComponentHudPosition::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_LapCount", ComponentHudLapCount::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Fps", ComponentHudFps::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_HUD_Popup", ComponentHudPopup::CreateComponent);

	m_pGameObjectManager->RegisterComponentFactory("GOC_Track", ComponentTrack::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TrackPiece", ComponentTrackPiece::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Racer", ComponentRacer::CreateComponent);

	// Handle passed context parameters
	m_mContextParams = p_mContextParameters;
	int iCount = atoi(p_mContextParameters.at("PlayerCount").c_str());
	std::vector<HeatStroke::GameObject*> vRacers;
	for (int i = 0; i < iCount; i++)
	{
		std::string strPlayerX = "Player" + std::to_string(i);

		std::string kartFile = p_mContextParameters.at(strPlayerX + "_KartDefinitionFile");
		std::string driverFile = p_mContextParameters.at(strPlayerX + "_DriverDefinitionFile");
		std::string cameraFile = p_mContextParameters.at(strPlayerX + "_CameraDefinitionFile");

		// generate racers
		HeatStroke::GameObject* pRacer = GenerateRacer(kartFile, driverFile, cameraFile, strPlayerX);
		vRacers.push_back(pRacer);
	}

	// Load Lights, and Tracks
	m_pGameObjectManager->CreateGameObject(p_mContextParameters.at("Light"), "AmbientAndDirectionalLight");
	m_pGameObjectManager->CreateGameObject(p_mContextParameters.at("TrackDefinitionFile"), "Track");

	// add racers to track
	// Note: Needs to be done after LoadLevel so the track is loaded
	// LoadLevel also needs to be done after "Player0" GameObject is created for the camera
	HeatStroke::GameObject* pTrack = m_pGameObjectManager->GetGameObject("Track");
	ComponentTrack* pTrackComponent = dynamic_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));
	for (int i = 0; i < iCount; i++)
	{
		pTrackComponent->RegisterRacer(vRacers.at(i));
		vRacers.at(i)->GetTransform().TranslateXYZ(1.0f * i, 0.0f, 0.0f); // TODO: Better positioning
	}
}

HeatStroke::GameObject* Kartaclysm::StateRacing::GenerateRacer
(
	const std::string& p_strKartDefinitionFile, 
	const std::string& p_strDriverDefinitionFile, 
	const std::string& p_strCameraDefinitionFile, 
	const std::string& p_strGuid /*= ""*/
)
{
	//TEMP: only here to allow creation of opponent
	std::string strRacerDefinitionFile = "CS483/CS483/Kartaclysm/Data/Racer/racer.xml";
	HeatStroke::GameObject* pRacer = m_pGameObjectManager->CreateGameObject(strRacerDefinitionFile, p_strGuid);

	if (p_strGuid == "Player0")
	{
		HeatStroke::GameObject* pHUD = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Racer/hud.xml", p_strGuid + "_HUD");
	}
	// TODO: make the HUD work for multiple players.

	HeatStroke::GameObject* pKart = m_pGameObjectManager->CreateGameObject(p_strKartDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pDriver = m_pGameObjectManager->CreateGameObject(p_strDriverDefinitionFile, "", pRacer);
	HeatStroke::GameObject* pCamera = m_pGameObjectManager->CreateGameObject(p_strCameraDefinitionFile, "", pRacer);
	pCamera->GetTransform().SetTranslation(glm::vec3(0.0f, 0.6f, -1.2f));

	ComponentRacer* pRacerComponent = static_cast<ComponentRacer*>(pRacer->GetComponent("GOC_Racer"));
	pRacerComponent->SetKart(pKart);
	pRacerComponent->SetDriver(pDriver);

	return pRacer;
}

//------------------------------------------------------------------------------
// Method:		Suspend
// Parameter:	const int p_iNewState - index of new state being pushed
// 
// Called when this state is pushed down in the stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Suspend(const int p_iNewState)
{
	m_bSuspended = true;

	if (m_pPauseDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
		delete m_pPauseDelegate;
		m_pPauseDelegate = nullptr;
	}
}

//------------------------------------------------------------------------------
// Method:		Unsuspend
// Parameter:	const int p_iPrevState - index of previous state popped
// 
// Called when this state is popped back to top of stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;

	if (m_pPauseDelegate == nullptr)
	{
		m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::PauseGame, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);
	}
}

//------------------------------------------------------------------------------
// Method:    Update
// Parameter: const float p_fDelta
// 
// Called each from when this state is active.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);
	}

	// TODO: add camera as child object of kart
	//			there's a weird bug with children of moving parents at the moment, so once that's sorted out, we can fix this
	/*
	HeatStroke::GameObject* pKart = m_pGameObjectManager->GetGameObject("Player0");
	const glm::vec3& vKartPosition = pKart->GetTransform().GetTranslation();
	//pKart->GetTransform().SetScaleXYZ(0.2f, 0.2f, 0.2f);

	glm::vec3 offset = glm::vec3(0.0f, 0.6f, -1.2f);

	ComponentKartController *controller = (ComponentKartController*)pKart->GetComponent("GOC_KartController");
	if (controller != nullptr)
	{
		offset = offset * controller->GetRotationMinusSwerve();
	}
	else
	{
		offset = offset * pKart->GetTransform().GetRotation();
	}
	offset = offset * glm::vec3(-1.0f, 1.0f, 1.0f);
	offset = offset + vKartPosition;

	*/
	/*
	std::vector<HeatStroke::GameObject*> pCameras = m_pGameObjectManager->GetGameObjectsByTag("Camera");
	std::vector<HeatStroke::GameObject*>::iterator it = pCameras.begin(), end = pCameras.end();
	for (; it != end; it++)
	{
		//(*it)->GetTransform().SetTranslation(offset);
		HeatStroke::GameObject* pCamera = *it;
		const HeatStroke::GameObject* pParent = pCamera->GetParent();
		const glm::vec3& vPosition = pParent->GetTransform().GetTranslation();
		glm::vec3 vOffset = glm::vec3(0.0f, 0.6f, -1.2f);
		ComponentKartController *pController = (ComponentKartController*)pParent->GetComponent("GOC_KartController");
		if (pController != nullptr)
		{
			vOffset = vOffset * pController->GetRotationMinusSwerve();
		}
		else
		{
			vOffset = vOffset * pParent->GetTransform().GetRotation();
		}
		vOffset = vOffset * glm::vec3(1.0f, 1.0f, 1.0f);
		vOffset = vOffset + vPosition;
		pCamera->GetTransform().SetTranslation(vOffset);
	}
	*/
}

//------------------------------------------------------------------------------
// Method:    PreRender
// 
// Called before rendering occurs.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

//------------------------------------------------------------------------------
// Method:    Exit
// 
// Called when this state is popped off the stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Exit()
{
	m_bSuspended = false;

	if (m_pPauseDelegate != nullptr)
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Pause", m_pPauseDelegate);
		delete m_pPauseDelegate;
		m_pPauseDelegate = nullptr;
	}

	HeatStroke::EventManager::Instance()->RemoveListener("RacerFinishedRace2", m_pRaceFinishedDelegate);
	delete m_pRaceFinishedDelegate;
	m_pRaceFinishedDelegate = nullptr;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

//------------------------------------------------------------------------------
// Method:    PauseGame
// Parameter: const HeatStroke::Event* p_pEvent - Event that triggers when a player pauses
// 
// Pause the game by pushing the Pause State.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::PauseGame(const HeatStroke::Event* p_pEvent)
{
	// Get the player who paused the game
	int iPlayer = 0;
	p_pEvent->GetOptionalIntParameter("Player", iPlayer, iPlayer);

	// Create context for pushing to pause state
	HeatStroke::StateMachine::ContextParameters mContext = HeatStroke::StateMachine::ContextParameters();
	mContext["Player"] = std::to_string(iPlayer);

	// Push pause state
	m_pStateMachine->Push(1, mContext);
}

void Kartaclysm::StateRacing::FinishRace(const HeatStroke::Event* p_pEvent)
{
	printf("restarting race\n");
	m_pStateMachine->Pop();
	m_pStateMachine->Push(0, m_mContextParams);
	return;
}
