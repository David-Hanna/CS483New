//------------------------------------------------------------------------
// StateTrackSelectionMenu
// Author:	David Hanna
//	
// Allows player one to select a track to race on.
// Continues to racing state.
//------------------------------------------------------------------------

#include "StateTrackSelectionMenu.h"

Kartaclysm::StateTrackSelectionMenu::StateTrackSelectionMenu()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
{
}

Kartaclysm::StateTrackSelectionMenu::~StateTrackSelectionMenu()
{
	Exit();
}

void Kartaclysm::StateTrackSelectionMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_pGameObjectManager = new HeatStroke::GameObjectManager();
	m_mContextParameters = p_mContextParameters;
	printf("Entering Track Selection Menu State.\n");
}

void Kartaclysm::StateTrackSelectionMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_ENTER))
		{
			m_mContextParameters.insert(std::pair<std::string, std::string>("TrackDefinitionFile", "CS483/CS483/Kartaclysm/Data/Tracks/up_and_over.xml"));

			m_pStateMachine->Pop();
			m_pStateMachine->Push(STATE_RACING, m_mContextParameters);
		}
	}
}

void Kartaclysm::StateTrackSelectionMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateTrackSelectionMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	printf("Exiting Track Selection Menu state.\n");
}