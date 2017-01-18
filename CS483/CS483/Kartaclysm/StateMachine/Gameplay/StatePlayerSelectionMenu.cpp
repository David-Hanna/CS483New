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
	GameplayState("Player Selection"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
{
}

Kartaclysm::StatePlayerSelectionMenu::~StatePlayerSelectionMenu()
{
}

void Kartaclysm::StatePlayerSelectionMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	m_pGameObjectManager = new HeatStroke::GameObjectManager();
	
	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_3DModel", HeatStroke::Component3DModel::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/divider.xml");

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/driver_selection_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/kart_selection_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/ready_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/description_boxes_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/driver_abilities_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/kart_abilities_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/stats_1.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/kart_display_1.xml");

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/press_start_to_join_2.xml", "Press_Start_2");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/press_start_to_join_3.xml", "Press_Start_3");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/press_start_to_join_4.xml", "Press_Start_4");
}

void Kartaclysm::StatePlayerSelectionMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (bConfirm)
		{
			std::string strKartFile = "CS483/CS483/Kartaclysm/Data/Racer/kart_juggernaut.xml";
			std::string strDriverFile = "CS483/CS483/Kartaclysm/Data/Racer/driver_clockmaker.xml";
			std::string strCameraTopFile = "CS483/CS483/Kartaclysm/Data/Camera/camera_top.xml";
			std::string strCameraBottomFile = "CS483/CS483/Kartaclysm/Data/Camera/camera_bottom.xml";

			// TODO: Maybe at some point we decouple and set this value by Event instead?
			int iPlayerCount = 2;
			std::map<std::string, std::string> mContextParameters;
			mContextParameters.insert(std::pair<std::string, std::string>("PlayerCount", std::to_string(iPlayerCount)));
			mContextParameters.insert(std::pair<std::string, std::string>("Player0_KartDefinitionFile", strKartFile));
			mContextParameters.insert(std::pair<std::string, std::string>("Player0_DriverDefinitionFile", strDriverFile));
			mContextParameters.insert(std::pair<std::string, std::string>("Player0_CameraDefinitionFile", strCameraTopFile));

			mContextParameters.insert(std::pair<std::string, std::string>("Player1_KartDefinitionFile", strKartFile));
			mContextParameters.insert(std::pair<std::string, std::string>("Player1_DriverDefinitionFile", strDriverFile));
			mContextParameters.insert(std::pair<std::string, std::string>("Player1_CameraDefinitionFile", strCameraBottomFile));

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
}