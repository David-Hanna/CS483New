//----------------------------------------------------------------------------
// KartGame.cpp
// Author: David MacIntosh
//
// Class that handles core game logic.
//----------------------------------------------------------------------------

#include "KartGame.h"

#include "Components\ComponentKartController.h"
#include "Components\ComponentCameraController.h"
#include "ComponentCamera.h"
#include "SceneManager.h"
#include "KeyboardInputBuffer.h"

bool Kartaclysm::KartGame::Init()
{
	// Initialize singletons
	HeatStroke::SceneManager::CreateInstance(m_pWindow);
	HeatStroke::KeyboardInputBuffer::CreateInstance(m_pWindow);

	// Setup State Machine and push first state
	m_pGameStates = new HeatStroke::StateMachine();
	m_pGameStates->SetStateMachineOwner(this);
	m_pGameStates->RegisterState(0, new StateRacing());
	m_pGameStates->Push(0);

	// Load XML
	tinyxml2::XMLDocument doc;
	doc.LoadFile("CS483/CS483/Kartaclysm/Data/test_level.xml");

	// TODO: this is bad, so bad, fix pls
	tinyxml2::XMLNode *node = doc.LastChild()->LastChild();
	tinyxml2::XMLNode *node2 = node->PreviousSibling();

	//p_gameObjectManager->CreateGameObject(node2);
	//p_gameObjectManager->CreateGameObject(node);

	// line stuff
	lineDrawer = new HeatStroke::LineDrawer();
	lineDrawer->Init("CS483/CS483/Kartaclysm/lines.vsh", "CS483/CS483/Kartaclysm/lines.fsh");

	return true;
}

void Kartaclysm::KartGame::Update(const float p_fDelta)
{
	HeatStroke::SceneManager::Instance()->Update(p_fDelta);
	HeatStroke::KeyboardInputBuffer::Instance()->Update(p_fDelta);

	// Call Update() on each state in stack, starting from bottom
	m_pGameStates->Update(p_fDelta, true);
}

void Kartaclysm::KartGame::PreRender()
{
	// Call PreRender() on each state in stack, starting from bottom
	auto it = m_pGameStates->begin(), end = m_pGameStates->end();
	for (; it != end; it++)
	{
		static_cast<GameplayState*>(it->second)->PreRender();
	}
}

void Kartaclysm::KartGame::Render()
{
	// good shit
	HeatStroke::SceneManager::Instance()->Render();

	// bad shit
	HeatStroke::SceneCamera *cam = HeatStroke::SceneManager::Instance()->GetActiveCamera();

	float h = 0.0f;
	for (int i = 1; i <= 10; i++)
	{
		lineDrawer->AddLine(glm::vec3(i, h, i), glm::vec3(i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(i, h, i), glm::vec3(-i, h, i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(i, h, -i), glm::vec3(-i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
		lineDrawer->AddLine(glm::vec3(-i, h, i), glm::vec3(-i, h, -i), HeatStroke::Color4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	//glm::vec3 pos = p_gameObjectManager->GetGameObject("Kart")->GetTransform().GetTranslation();
	//lineDrawer->AddLine(pos, pos + glm::vec3(0.0f, 0.2f, 0.0f), HeatStroke::Color4(0.0f, 0.0f, 1.0f, 1.0f));

	//lineDrawer->Render(cam->GetProjectionMatrix(), cam->GetViewMatrix());
}

void Kartaclysm::KartGame::Shutdown()
{
	delete m_pGameStates;
	m_pGameStates = nullptr;
}