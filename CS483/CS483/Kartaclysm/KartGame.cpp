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

	// TEMP: get kart and driver files
	// this will eventually happen in a separate state before StateRacing
	std::string strKartFile = "CS483/CS483/Kartaclysm/Data/Racer/kart_speedster.xml";
	std::string strDriverFile = "CS483/CS483/Kartaclysm/Data/Racer/driver_kingpin.xml";

	std::map<std::string, std::string> mContextParams;
	mContextParams.insert(std::pair<std::string, std::string>("PlayerCount", "2"));
	mContextParams.insert(std::pair<std::string, std::string>("Player0_KartDefinitionFile", strKartFile));
	mContextParams.insert(std::pair<std::string, std::string>("Player0_DriverDefinitionFile", strDriverFile));
	mContextParams.insert(std::pair<std::string, std::string>("Player1_KartDefinitionFile", strKartFile));
	mContextParams.insert(std::pair<std::string, std::string>("Player1_DriverDefinitionFile", strDriverFile));

	mContextParams.insert(std::pair<std::string, std::string>("Camera", "CS483/CS483/Kartaclysm/Data/Camera/camera_full.xml"));
	mContextParams.insert(std::pair<std::string, std::string>("Light", "CS483/CS483/Kartaclysm/Data/Lights/light.xml"));
	mContextParams.insert(std::pair<std::string, std::string>("Track", "CS483/CS483/Kartaclysm/Data/Tracks/up_and_over.xml"));

	// Setup State Machine and push first state
	m_pGameStates = new HeatStroke::StateMachine();
	m_pGameStates->SetStateMachineOwner(this);
	m_pGameStates->RegisterState(0, new StateRacing());
	m_pGameStates->RegisterState(1, new StatePaused());
	m_pGameStates->RegisterState(2, new StateMainMenu());
	m_pGameStates->Push(2, mContextParams);

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
