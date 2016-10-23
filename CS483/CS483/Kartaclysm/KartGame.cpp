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
	HeatStroke::ParsingServiceLocator::CreateInstance(new HeatStroke::DebugParsingWrapper(new HeatStroke::StoredParsingService()));

	// Setup State Machine and push first state
	m_pGameStates = new HeatStroke::StateMachine();
	m_pGameStates->SetStateMachineOwner(this);
	m_pGameStates->RegisterState(0, new StateRacing());
	m_pGameStates->Push(0);

	return true;
}

void Kartaclysm::KartGame::Update(const float p_fDelta)
{
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
}

void Kartaclysm::KartGame::Shutdown()
{
	HeatStroke::KeyboardInputBuffer::DestroyInstance();
	HeatStroke::ParsingServiceLocator::DestroyInstance();

	delete m_pGameStates;
	m_pGameStates = nullptr;
}