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
	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
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
	m_pGameObjectManager->RegisterComponentFactory("GOC_Camera", HeatStroke::ComponentCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_CameraController", ComponentCameraController::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);

	// Handle passed context parameters

	// Load XML to create GameObjects

	// Temporary code from here down
	// TODO: Remove every trace of this crap
	// Load XML
	tinyxml2::XMLDocument doc;
	doc.LoadFile("CS483/CS483/Kartaclysm/Data/test_level.xml");

	tinyxml2::XMLNode *node = doc.LastChild()->LastChild();
	tinyxml2::XMLNode *node2 = node->PreviousSibling();

	m_pGameObjectManager->CreateGameObject(node2);
	m_pGameObjectManager->CreateGameObject(node);

	// line stuff
	lineDrawer = new HeatStroke::LineDrawer();
	lineDrawer->Init("CS483/CS483/Kartaclysm/lines.vsh", "CS483/CS483/Kartaclysm/lines.fsh");
	// </crap>
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
	// Do not update when suspended
	if (!m_bSuspended)
	{
		m_pGameObjectManager->Update(p_fDelta);
	}

	// Temporary line drawing stuff
	// TODO: Remove every trace of this crap
	HeatStroke::SceneCamera *cam = HeatStroke::SceneManager::Instance()->GetActiveCamera();

	float h = 0.0f;
	for (int i = 1; i <= 10; i++)
	{
		lineDrawer->AddLine(glm::vec3(i, h, i), glm::vec3(i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(i, h, i), glm::vec3(-i, h, i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(i, h, -i), glm::vec3(-i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(-i, h, i), glm::vec3(-i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	glm::vec3 pos = m_pGameObjectManager->GetGameObject("Kart")->GetTransform().GetTranslation();
	lineDrawer->AddLine(pos, pos + glm::vec3(0.0f, 0.2f, 0.0f), HeatStroke::Color4(0.0f, 0.0f, 1.0f, 1.0f));

	lineDrawer->Render(cam->GetProjectionMatrix(), cam->GetViewMatrix());
	// </crap>
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