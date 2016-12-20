//------------------------------------------------------------------------
// StatePlayerSelectionMenu
// Author:	David Hanna
//	
// Allows player to select their character and kart.
// Allows new players to join the game.
// Continues to the track selection state.
//------------------------------------------------------------------------

#include "StatePlayerSelectionMenu.h"

Kartaclysm::StatePlayerSelectionMenu::StatePlayerSelectionMenu()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
{
}

Kartaclysm::StatePlayerSelectionMenu::~StatePlayerSelectionMenu()
{
	Exit();
}

void Kartaclysm::StatePlayerSelectionMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_pGameObjectManager = new HeatStroke::GameObjectManager();
	printf("Entering Player Selection Menu State.\n");
}

void Kartaclysm::StatePlayerSelectionMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		std::string strKartFile = "CS483/CS483/Kartaclysm/Data/Racer/kart_juggernaut.xml";
		std::string strDriverFile = "CS483/CS483/Kartaclysm/Data/Racer/driver_kingpin.xml";
		std::string strCameraTopFile = "CS483/CS483/Kartaclysm/Data/Camera/camera_top.xml";
		std::string strCameraBottomFile = "CS483/CS483/Kartaclysm/Data/Camera/camera_bottom.xml";

		std::map<std::string, std::string> mContextParameters;
		mContextParameters.insert(std::pair<std::string, std::string>("PlayerCount", "2"));
		mContextParameters.insert(std::pair<std::string, std::string>("Player0_KartDefinitionFile", strKartFile));
		mContextParameters.insert(std::pair<std::string, std::string>("Player0_DriverDefinitionFile", strDriverFile));
		mContextParameters.insert(std::pair<std::string, std::string>("Player0_CameraDefinitionFile", strCameraTopFile));

		mContextParameters.insert(std::pair<std::string, std::string>("Player1_KartDefinitionFile", strKartFile));
		mContextParameters.insert(std::pair<std::string, std::string>("Player1_DriverDefinitionFile", strDriverFile));
		mContextParameters.insert(std::pair<std::string, std::string>("Player1_CameraDefinitionFile", strCameraBottomFile));

		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_ENTER))
		{
			m_pStateMachine->Pop();
			m_pStateMachine->Push(STATE_TRACK_SELECTION_MENU, mContextParameters);
		}
	}
}

void Kartaclysm::StatePlayerSelectionMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StatePlayerSelectionMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	printf("Exiting Player Selection Menu state.\n");
}