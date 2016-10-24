//----------------------------------------------------------------------------
// KartGame.cpp
// Author: David MacIntosh
//
// Class the handles core game logic.
//----------------------------------------------------------------------------

#include "KartGame.h"

#include "Components\ComponentKartController.h"
#include "Components\ComponentCameraController.h"
#include "ComponentCamera.h"
#include "SceneManager.h"
#include "KeyboardInputBuffer.h"

namespace Kartaclysm
{
	bool KartGame::Init()
	{
		p_gameObjectManager = new HeatStroke::GameObjectManager();

		// Register component factory methods
		p_gameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);
		p_gameObjectManager->RegisterComponentFactory("GOC_CameraController", ComponentCameraController::CreateComponent);
		p_gameObjectManager->RegisterComponentFactory("GOC_Camera", HeatStroke::ComponentCamera::CreateComponent);

		// Initialize singletons
		HeatStroke::SceneManager::CreateInstance(m_pWindow);
		HeatStroke::KeyboardInputBuffer::CreateInstance(m_pWindow);

		// Load XML
		tinyxml2::XMLDocument doc;
		doc.LoadFile("CS483/CS483/Kartaclysm/Data/test_level.xml");

		// TODO: this is bad, so bad, fix pls
		tinyxml2::XMLNode *node = doc.LastChild()->LastChild();
		tinyxml2::XMLNode *node2 = node->PreviousSibling();

		p_gameObjectManager->CreateGameObject(node2);
		p_gameObjectManager->CreateGameObject(node);

		// line stuff
		lineDrawer = new HeatStroke::LineDrawer();
		lineDrawer->Init("CS483/CS483/Kartaclysm/lines.vsh", "CS483/CS483/Kartaclysm/lines.fsh");

		return true;
	}

	void KartGame::Update(const float p_fDelta)
	{
		HeatStroke::SceneManager::Instance()->Update(p_fDelta);
		HeatStroke::KeyboardInputBuffer::Instance()->Update(p_fDelta);

		p_gameObjectManager->Update(p_fDelta);
	}

	void KartGame::PreRender()
	{
	}

	void KartGame::Render()
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

		glm::vec3 pos = p_gameObjectManager->GetGameObject("Kart")->GetTransform().GetTranslation();
		lineDrawer->AddLine(pos, pos + glm::vec3(0.0f, 0.2f, 0.0f), HeatStroke::Color4(0.0f, 0.0f, 1.0f, 1.0f));

		lineDrawer->Render(cam->GetProjectionMatrix(), cam->GetViewMatrix());
	}

	void KartGame::Shutdown()
	{
		delete p_gameObjectManager;
	}
}