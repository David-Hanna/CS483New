//------------------------------------------------------------------------
// StateRacing
// Author:	Bradley Cooper
//	
// Gameplay state for racing.
//------------------------------------------------------------------------

#include "StateRacing.h"

#include "ComponentCamera.h"
#include "Components\ComponentCameraController.h"
#include "Components\ComponentKartController.h"

//------------------------------------------------------------------------------
// Method:    StateRacing
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
Kartaclysm::StateRacing::StateRacing()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
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

	// Initialize our GameObjectManager
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	// Register component factory methods
	m_pGameObjectManager->RegisterComponentFactory("GOC_3DModel", HeatStroke::Component3DModel::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_AmbientLight", HeatStroke::ComponentAmbientLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_DirectionalLight", HeatStroke::ComponentDirectionalLight::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Camera", HeatStroke::ComponentCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);

	// Handle passed context parameters

	// Load the GameObjects from XML.
	LoadLevel("CS483/CS483/Kartaclysm/Data/test_level.xml");

	HeatStroke::GameObject* pKart = m_pGameObjectManager->GetGameObject("Kart");
	pKart->GetTransform().SetScaleXYZ(0.1f, 0.1f, 0.25f);

	// line stuff
	lineDrawer = new HeatStroke::LineDrawer();
	lineDrawer->Init("CS483/CS483/Kartaclysm/Data/lines.vsh", "CS483/CS483/Kartaclysm/Data/lines.fsh");
}

void Kartaclysm::StateRacing::LoadLevel(const std::string& p_strLevelPath)
{
	tinyxml2::XMLDocument mLevelDoc;
	assert(mLevelDoc.LoadFile(p_strLevelPath.c_str()) == tinyxml2::XML_NO_ERROR);

	tinyxml2::XMLNode* pLevelNode = mLevelDoc.RootElement();
	assert(pLevelNode != nullptr);
	assert(strcmp(pLevelNode->Value(), "Level") == 0);

	// Iterate elements in the xml.
	for (tinyxml2::XMLNode* pGameObjectNode = pLevelNode->FirstChild(); pGameObjectNode != nullptr; pGameObjectNode = pGameObjectNode->NextSibling())
	{
		// skip comments
		if (pGameObjectNode->ToComment() != nullptr)
			continue;
		assert(strcmp(pGameObjectNode->Value(), "GameObject") == 0);
		m_pGameObjectManager->CreateGameObject(pGameObjectNode);
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
}

//------------------------------------------------------------------------------
// Method:    Update
// Parameter: const float p_fDelta
// 
// Called each from when this state is active.
//------------------------------------------------------------------------------
void Kartaclysm::StateRacing::Update(const float p_fDelta)
{
	// good shit
	// Do not update when suspended
	if (!m_bSuspended)
	{
		m_pGameObjectManager->Update(p_fDelta);
	}

	// bad shit
	HeatStroke::GameObject* pKart = m_pGameObjectManager->GetGameObject("Kart");
	const glm::vec3& vKartPosition = pKart->GetTransform().GetTranslation();

	HeatStroke::GameObject* pCamera = m_pGameObjectManager->GetGameObject("Camera");

	glm::vec3 offset = glm::vec3(0.0f, 0.5f, -1.0f);

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

	HeatStroke::SceneCamera* pActiveCamera = HeatStroke::SceneManager::Instance()->GetActiveCamera();
	lineDrawer->Render(pActiveCamera->GetProjectionMatrix(), pActiveCamera->GetViewMatrix());
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

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}