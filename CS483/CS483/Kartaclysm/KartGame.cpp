//----------------------------------------------------------------------------
// KartGame.cpp
// Author: David MacIntosh
//
// Class that handles core game logic.
//----------------------------------------------------------------------------

#include "KartGame.h"
#include "EventManager.h"

#include "ComponentKartController.h"
#include "ComponentCameraController.h"
#include "ComponentPerspectiveCamera.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "KeyboardInputBuffer.h"

bool Kartaclysm::KartGame::Init()
{
	// Initialize singletons
	HeatStroke::EventManager::CreateInstance();
	HeatStroke::SceneManager::CreateInstance(m_pWindow);
	HeatStroke::CollisionManager::CreateInstance();
	HeatStroke::KeyboardInputBuffer::CreateInstance(m_pWindow);
	HeatStroke::JoystickInputBuffer::CreateInstance(m_pWindow);
	InputActionMapping::CreateInstance("CS483/CS483/Kartaclysm/Data/UserConfig/ControlBindings.xml");
	PlayerInputMapping::CreateInstance();
	if (!PlayerInputMapping::Instance()->SetSplitscreenPlayers(2))
	{
		assert(false && "Failed to set number of players.");
	}

	// Setup State Machine and push first state
	m_pGameStates = new HeatStroke::StateMachine();
	m_pGameStates->SetStateMachineOwner(this);
	
	m_pGameStates->RegisterState(GameplayState::STATE_MAIN_MENU, new StateMainMenu());
	m_pGameStates->RegisterState(GameplayState::STATE_PLAYER_SELECTION_MENU, new StatePlayerSelectionMenu());
	m_pGameStates->RegisterState(GameplayState::STATE_TRACK_SELECTION_MENU, new StateTrackSelectionMenu());
	m_pGameStates->RegisterState(GameplayState::STATE_RACING, new StateRacing());
	m_pGameStates->RegisterState(GameplayState::STATE_PAUSED, new StatePaused());
	m_pGameStates->RegisterState(GameplayState::STATE_RACE_COMPLETE_MENU, new StateRaceCompleteMenu());

	m_pGameStates->Push(GameplayState::STATE_MAIN_MENU, std::map<std::string, std::string>());

	return true;
}

void Kartaclysm::KartGame::Update(const float p_fDelta)
{
	HeatStroke::EventManager::Instance()->Update(p_fDelta);
	HeatStroke::CollisionManager::Instance()->Update(p_fDelta);
	HeatStroke::KeyboardInputBuffer::Instance()->Update(p_fDelta);
	HeatStroke::JoystickInputBuffer::Instance()->Update(p_fDelta);
	PlayerInputMapping::Instance()->Update(p_fDelta);

	// Call Update() on each state in stack, starting from bottom
	m_pGameStates->Update(p_fDelta, false);
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
	HeatStroke::SceneManager::Instance()->Render();
}

void Kartaclysm::KartGame::Shutdown()
{
	delete m_pGameStates;
	m_pGameStates = nullptr;

	PlayerInputMapping::DestroyInstance();
	InputActionMapping::DestroyInstance();
	HeatStroke::JoystickInputBuffer::DestroyInstance();
	HeatStroke::KeyboardInputBuffer::DestroyInstance();
	HeatStroke::SceneManager::DestroyInstance();
	HeatStroke::EventManager::DestroyInstance();
	HeatStroke::CollisionManager::DestroyInstance();
}
