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
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	// Register listening for pause
	m_pPauseDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&StateRacing::PauseGame, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("Pause", m_pPauseDelegate);

	// Initialize our GameObjectManager
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	// Register component factory methods
	m_pGameObjectManager->RegisterComponentFactory("GOC_3DModel", HeatStroke::Component3DModel::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AmbientLight", HeatStroke::ComponentAmbientLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_DirectionalLight", HeatStroke::ComponentDirectionalLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AbilityConditions", ComponentAbilityConditions::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_SampleAbility", ComponentSampleAbility::CreateComponent);

	// Handle passed context parameters

	// Load the GameObjects from XML.
	LoadLevel("CS483/CS483/Kartaclysm/Data/test_level.xml");

	HeatStroke::GameObject* pKart = m_pGameObjectManager->GetGameObject("Kart");
	pKart->GetTransform().SetScaleXYZ(0.1f, 0.1f, 0.1f);

	// line stuff
	lineDrawer = new HeatStroke::LineDrawer();
	lineDrawer->Init("CS483/CS483/Kartaclysm/Data/lines.vsh", "CS483/CS483/Kartaclysm/Data/lines.fsh");
}

void Kartaclysm::StateRacing::LoadLevel(const std::string& p_strLevelPath)
{
	tinyxml2::XMLDocument mLevelDoc;
	assert(mLevelDoc.LoadFile(p_strLevelPath.c_str()) == tinyxml2::XML_NO_ERROR);

	tinyxml2::XMLElement* pLevelRootElement = mLevelDoc.RootElement();
	assert(pLevelRootElement != nullptr);
	assert(strcmp(pLevelRootElement->Value(), "Level") == 0);

	// Iterate elements in the xml.
	for (tinyxml2::XMLElement* pGameObjectElement = pLevelRootElement->FirstChildElement("GameObject");
		 pGameObjectElement != nullptr;
		 pGameObjectElement = pGameObjectElement->NextSiblingElement("GameObject"))
	{
		m_pGameObjectManager->CreateGameObject(pGameObjectElement);
	}
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
		m_pGameObjectManager->Update(p_fDelta);
	}

	// bad shit
	HeatStroke::GameObject* pKart = m_pGameObjectManager->GetGameObject("Kart");
	const glm::vec3& vKartPosition = pKart->GetTransform().GetTranslation();

	HeatStroke::GameObject* pCamera = m_pGameObjectManager->GetGameObject("Camera");

	glm::vec3 offset = glm::vec3(0.0f, 0.25f, -0.5f);

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

	pCamera->GetTransform().SetTranslation(offset);

	// more bad shit
	float h = -0.2f;
	for (int i = 1; i <= 10; i++)
	{
		lineDrawer->AddLine(glm::vec3(i, h, i), glm::vec3(i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(i, h, i), glm::vec3(-i, h, i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(i, h, -i), glm::vec3(-i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(-i, h, i), glm::vec3(-i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	HeatStroke::SceneCamera* pActiveCamera = HeatStroke::SceneManager::Instance()->GetActivePerspectiveCamera();
	lineDrawer->Render(pActiveCamera->GetProjectionMatrix(), pActiveCamera->GetViewMatrix());

	// FIX ME - move this into data.
	HeatStroke::GameObject* pSprite = m_pGameObjectManager->GetGameObject("SampleSprite");
	pSprite->GetTransform().SetTranslationXYZ(80.0f, 80.0f, 0.0f);
	pSprite->GetTransform().SetScaleXYZ(20.0f, 20.0f, 1.0f);
}

//------------------------------------------------------------------------------
// Method:    PreRender
// 
// Called before rendering occurs.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::PreRender()
{
	// Render even when suspended
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
