//----------------------------------------------------------------------------
// KartGame.cpp
// Author: David MacIntosh
//
// Class that handles core game logic.
//----------------------------------------------------------------------------

#include "KartGame.h"

bool Kartaclysm::KartGame::Init()
{
	// Initialize singletons
	HeatStroke::KeyboardInputBuffer::CreateInstance(m_pWindow);

	// Setup State Machine and push first state
	m_pGameStates = new HeatStroke::StateMachine();
	m_pGameStates->SetStateMachineOwner(this);
	m_pGameStates->RegisterState(0, new StateRacing());
	m_pGameStates->Push(0);

	m_pModel = new HeatStroke::Model(std::string("Assets/box/box.obj"));

	return true;
}

void Kartaclysm::KartGame::Update(const float p_fDelta)
{
	HeatStroke::KeyboardInputBuffer::Instance()->Update(p_fDelta);

	// Call Update() on each state in stack, starting from bottom
	m_pGameStates->Update(p_fDelta, true);

	static float angle = 45.0f;
	angle += p_fDelta * 30.0f;
	HeatStroke::Transform mTransform;
	mTransform.RotateXYZ(angle, 1.0f, 0.0f, 0.0f);
	mTransform.RotateXYZ(angle, 0.0f, 1.0f, 0.0f);
	mTransform.RotateXYZ(angle, 0.0f, 0.0f, 1.0f);
	m_pModel->SetTransform(mTransform.GetTransform());
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
	HeatStroke::Camera mCamera;
	m_pModel->Render(&mCamera);
}

void Kartaclysm::KartGame::Shutdown()
{
	delete m_pGameStates;
	m_pGameStates = nullptr;
}