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
	HeatStroke::EventManager::CreateInstance();
	HeatStroke::KeyboardInputBuffer::CreateInstance(m_pWindow);
	HeatStroke::JoystickInputBuffer::CreateInstance(m_pWindow);
	InputActionMapping::CreateInstance("CS483/Kartaclysm/Data/UserConfig/ControlBindings.xml");
	PlayerInputMapping::CreateInstance();

	// Setup State Machine and push first state
	m_pGameStates = new HeatStroke::StateMachine();
	m_pGameStates->SetStateMachineOwner(this);
	m_pGameStates->RegisterState(0, new StateRacing());
	m_pGameStates->Push(0);

	return true;
}

void Kartaclysm::KartGame::Update(const float p_fDelta)
{
	HeatStroke::EventManager::Instance()->Update(p_fDelta);
	HeatStroke::KeyboardInputBuffer::Instance()->Update(p_fDelta);
	HeatStroke::JoystickInputBuffer::Instance()->Update(p_fDelta);
	PlayerInputMapping::Instance()->Update(p_fDelta);

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
}

void Kartaclysm::KartGame::Shutdown()
{
	delete m_pGameStates;
	m_pGameStates = nullptr;

	PlayerInputMapping::DestroyInstance();
	InputActionMapping::DestroyInstance();
	HeatStroke::JoystickInputBuffer::DestroyInstance();
	HeatStroke::KeyboardInputBuffer::DestroyInstance();
	HeatStroke::EventManager::DestroyInstance();
}