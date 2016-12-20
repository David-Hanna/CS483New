//------------------------------------------------------------------------
// StateRaceCompleteMenu
// Author:	David Hanna
//	
// Shows the results of the race and continues to the Main Menu state.
//------------------------------------------------------------------------

#include "StateRaceCompleteMenu.h"

Kartaclysm::StateRaceCompleteMenu::StateRaceCompleteMenu()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
{
}

Kartaclysm::StateRaceCompleteMenu::~StateRaceCompleteMenu()
{
	Exit();
}

void Kartaclysm::StateRaceCompleteMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_pGameObjectManager = new HeatStroke::GameObjectManager();
	printf("Entering Race Complete Menu State.\n");
}

void Kartaclysm::StateRaceCompleteMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_ENTER))
		{
			m_pStateMachine->Pop();
			m_pStateMachine->Push(STATE_MAIN_MENU, std::map<std::string, std::string>());
		}
	}
}

void Kartaclysm::StateRaceCompleteMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateRaceCompleteMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	printf("Exiting Race Complete Menu state.\n");
}