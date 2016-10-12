//----------------------------------------------------------------------------
// KartGame.cpp
// Author: David MacIntosh
//
// Class the handles core game logic.
//----------------------------------------------------------------------------

#include "KartGame.h"

#include "Components\ComponentKartController.h"
#include "KeyboardInputBuffer.h"

namespace Kartaclysm
{
	bool KartGame::Init()
	{
		p_gameObjectManager = new HeatStroke::GameObjectManager();

		// Register component factory methods
		p_gameObjectManager->RegisterComponentFactory("GOC_KartController", ComponentKartController::CreateComponent);

		// Initialize singletons
		HeatStroke::KeyboardInputBuffer::CreateInstance(m_pWindow);

		// Load XML
		tinyxml2::XMLDocument doc;
		doc.LoadFile("CS483/CS483/Kartaclysm/Data/test_level.xml");
		tinyxml2::XMLNode *node = doc.LastChild()->LastChild(); // TODO: this is bad, fix pls

		p_gameObjectManager->CreateGameObject(node);

		return true;
	}

	void KartGame::Update(const float p_fDelta)
	{
		HeatStroke::KeyboardInputBuffer::Instance()->Update(p_fDelta);

		p_gameObjectManager->Update(p_fDelta);
	}

	void KartGame::PreRender()
	{
	}

	void KartGame::Render()
	{
	}

	void KartGame::Shutdown()
	{
		delete p_gameObjectManager;
	}
}