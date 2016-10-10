//----------------------------------------------------------------------------
// KartGame.cpp
// Author: David MacIntosh
//
// Class the handles core game logic.
//----------------------------------------------------------------------------

#include "KartGame.h"

bool KartGame::Init()
{
	gameObjectManager = new HeatStroke::GameObjectManager();

	tinyxml2::XMLDocument doc;
	doc.LoadFile("CS483/CS483/Kartaclysm/Data/kart.xml");
	tinyxml2::XMLNode *node = doc.LastChild(); // THIS IS SHIT, FIX

	gameObjectManager->CreateGameObject(node);
	
	return true;
}

void KartGame::Update(const float p_fDelta)
{
}

void KartGame::PreRender()
{
}

void KartGame::Render()
{
}

void KartGame::Shutdown()
{
	delete gameObjectManager;
}