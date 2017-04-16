//------------------------------------------------------------------------
// StatePlayerSelectionMenu
// Author:	David Hanna
//	
// Allows player to select their character and kart.
// Allows new players to join the game.
// Continues to the track selection state.
//------------------------------------------------------------------------

#include "StatePlayerSelectionMenu.h"

void Kartaclysm::StatePlayerSelectionMenu::Initialize()
{
	m_uiNumPlayers = 0;

	GenerateDriverList();
	GenerateKartList();

	for (int i = 0; i < 4; i++)
	{
		m_mPerPlayerMenuState[i].bJoined = false;
		m_mPerPlayerMenuState[i].bReady = false;
		m_mPerPlayerMenuState[i].bDriverHighlighted = true;

		m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_CLEOPAPA;
		m_mPerPlayerMenuState[i].eSelectedKart = KART_SPEEDSTER;

		m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
		m_mPerPlayerMenuState[i].iDriverAccelerationStat = 0;
		m_mPerPlayerMenuState[i].iDriverHandlingStat = 1;
		m_mPerPlayerMenuState[i].iDriverDurabilityStat = -1;

		m_mPerPlayerMenuState[i].iKartSpeedStat = 3;
		m_mPerPlayerMenuState[i].iKartAccelerationStat = 3;
		m_mPerPlayerMenuState[i].iKartHandlingStat = 3;
		m_mPerPlayerMenuState[i].iKartDurabilityStat = 3;

		m_mPerPlayerMenuState[i].pPressStartToJoin = nullptr;
		m_mPerPlayerMenuState[i].pDriverSelection = nullptr;
		m_mPerPlayerMenuState[i].pKartSelection = nullptr;
		m_mPerPlayerMenuState[i].pReadyButton = nullptr;
		m_mPerPlayerMenuState[i].pDescriptionBoxes = nullptr;
		m_mPerPlayerMenuState[i].pDriverAbilities = nullptr;
		m_mPerPlayerMenuState[i].pKartAbilities = nullptr;
		m_mPerPlayerMenuState[i].pStatDescription = nullptr;
		m_mPerPlayerMenuState[i].pSpeedStatIcon = nullptr;
		m_mPerPlayerMenuState[i].pAccelerationStatIcon = nullptr;
		m_mPerPlayerMenuState[i].pHandlingStatIcon = nullptr;
		m_mPerPlayerMenuState[i].pDurabilityStatIcon = nullptr;
		m_mPerPlayerMenuState[i].pDriverKartDisplayConnector = nullptr;
		m_mPerPlayerMenuState[i].pDriverDisplay = nullptr;
		m_mPerPlayerMenuState[i].pKartDisplay = nullptr;
		m_mPerPlayerMenuState[i].pHighlight = nullptr;
	}

	m_bSuspended = false;

	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_3DModel", HeatStroke::Component3DModel::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_RotateOverTime", HeatStroke::ComponentRotateOverTime::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_menu.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/background.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/divider.xml");

	AddPlayer(0);

	m_mPerPlayerMenuState[1].pPressStartToJoin = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_1/press_start_to_join_1.xml");
	m_mPerPlayerMenuState[2].pPressStartToJoin = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_2/press_start_to_join_2.xml");
	m_mPerPlayerMenuState[3].pPressStartToJoin = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_3/press_start_to_join_3.xml");

	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

Kartaclysm::StatePlayerSelectionMenu::StatePlayerSelectionMenu()
	:
	GameplayState("Player Selection"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_uiNumPlayers(0)
{
}

Kartaclysm::StatePlayerSelectionMenu::~StatePlayerSelectionMenu()
{
}

void Kartaclysm::StatePlayerSelectionMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_mContextParameters = p_mContextParameters;
	Initialize();
}

void Kartaclysm::StatePlayerSelectionMenu::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->Update(p_fDelta);

	bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;

	for (unsigned int i = 0; i < 4; i++)
	{
		std::string strPlayerNum = std::to_string(i);
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(i, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (bConfirm)
		{
			if (!m_mPerPlayerMenuState[i].bJoined)
			{
				AddPlayer(i);
			}
			else
			{
				SetReadyStatus(i, !m_mPerPlayerMenuState[i].bReady);
			}
		}
		else if (bCancel)
		{
			if (m_mPerPlayerMenuState[i].bJoined)
			{
				RemovePlayer(i);

				// Pop if no player is left joined. Otherwise, reset all players to not ready.
				bool bPlayerStillJoined = false;
				for (unsigned int j = 0; j < 4; ++j)
				{
					if (m_mPerPlayerMenuState[j].bJoined)
					{
						bPlayerStillJoined = true;
						SetReadyStatus(j, false);
					}
				}

				if (!bPlayerStillJoined)
				{
					m_pStateMachine->Pop();
					return;
				}
			}
		}

		else if (m_mPerPlayerMenuState[i].bJoined)
		{
			if (bUp && !m_mPerPlayerMenuState[i].bDriverHighlighted)
			{
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pHighlight);
				m_mPerPlayerMenuState[i].pHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/highlight_driver_selection_" + strPlayerNum + ".xml");
				m_mPerPlayerMenuState[i].bDriverHighlighted = true;
			}

			else if (bDown && m_mPerPlayerMenuState[i].bDriverHighlighted)
			{
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pHighlight);
				m_mPerPlayerMenuState[i].pHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/highlight_kart_selection_" + strPlayerNum + ".xml");
				m_mPerPlayerMenuState[i].bDriverHighlighted = false;
			}

			if (bLeft && !m_mPerPlayerMenuState[i].bReady)
			{
				// save the y rotation to apply it to the new kart/driver
				glm::quat qCurrentRotation = m_mPerPlayerMenuState[i].pDriverDisplay->GetTransform().GetRotation();

				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pSpeedStatIcon);
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pAccelerationStatIcon);
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pHandlingStatIcon);
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDurabilityStatIcon);

				if (m_mPerPlayerMenuState[i].bDriverHighlighted)
				{
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDriverSelection);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDriverDisplay);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDriverAbilities);

					// TODO - we need to change this to be more dynamic/data-driven
					switch (m_mPerPlayerMenuState[i].eSelectedDriver)
					{
					case DRIVER_CLEOPAPA:
						m_mPerPlayerMenuState[i].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_kingpin_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_kingpin_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_kingpin_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_KINGPIN;
						m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
						m_mPerPlayerMenuState[i].iDriverAccelerationStat = 1;
						m_mPerPlayerMenuState[i].iDriverHandlingStat = -1;
						m_mPerPlayerMenuState[i].iDriverDurabilityStat = 0;
						break;

					case DRIVER_CLOCKMAKER:
						m_mPerPlayerMenuState[i].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_cleopapa_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_cleopapa_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_cleopapa_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_CLEOPAPA;
						m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
						m_mPerPlayerMenuState[i].iDriverAccelerationStat = 0;
						m_mPerPlayerMenuState[i].iDriverHandlingStat = 1;
						m_mPerPlayerMenuState[i].iDriverDurabilityStat = -1;
						break;

					case DRIVER_KINGPIN:
						m_mPerPlayerMenuState[i].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_clockmaker_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_clockmaker_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_clockmaker_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_CLOCKMAKER;
						m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
						m_mPerPlayerMenuState[i].iDriverAccelerationStat = -1;
						m_mPerPlayerMenuState[i].iDriverHandlingStat = 1;
						m_mPerPlayerMenuState[i].iDriverDurabilityStat = 0;
						break;
					}

					m_mPerPlayerMenuState[i].pDriverDisplay->SetParent(m_mPerPlayerMenuState[i].pDriverKartDisplayConnector);
				}
				else
				{
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pKartSelection);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pKartDisplay);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pKartAbilities);

					switch (m_mPerPlayerMenuState[i].eSelectedKart)
					{
					// TODO - we need to change this to be more dynamic/data-driven
					case KART_JUGGERNAUT:
						m_mPerPlayerMenuState[i].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_speedster_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_speedster_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_speedster_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedKart = KART_SPEEDSTER;
						m_mPerPlayerMenuState[i].iKartSpeedStat = 3;
						m_mPerPlayerMenuState[i].iKartAccelerationStat = 3;
						m_mPerPlayerMenuState[i].iKartHandlingStat = 3;
						m_mPerPlayerMenuState[i].iKartDurabilityStat = 3;
						break;

					case KART_SHOWOFF:
						m_mPerPlayerMenuState[i].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_juggernaut_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_juggernaut_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_juggernaut_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedKart = KART_JUGGERNAUT;
						m_mPerPlayerMenuState[i].iKartSpeedStat = 5;
						m_mPerPlayerMenuState[i].iKartAccelerationStat = 1;
						m_mPerPlayerMenuState[i].iKartHandlingStat = 1;
						m_mPerPlayerMenuState[i].iKartDurabilityStat = 5;
						break;

					case KART_SPEEDSTER:
						m_mPerPlayerMenuState[i].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_showoff_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_showoff_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_showoff_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedKart = KART_SHOWOFF;
						m_mPerPlayerMenuState[i].iKartSpeedStat = 2;
						m_mPerPlayerMenuState[i].iKartAccelerationStat = 5;
						m_mPerPlayerMenuState[i].iKartHandlingStat = 3;
						m_mPerPlayerMenuState[i].iKartDurabilityStat = 2;
						break;
					}

					m_mPerPlayerMenuState[i].pKartDisplay->SetParent(m_mPerPlayerMenuState[i].pDriverKartDisplayConnector);
				}

				m_mPerPlayerMenuState[i].pSpeedStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/speed_" + std::to_string(m_mPerPlayerMenuState[i].iKartSpeedStat + m_mPerPlayerMenuState[i].iDriverSpeedStat) + ".xml");
				m_mPerPlayerMenuState[i].pAccelerationStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/acceleration_" + std::to_string(m_mPerPlayerMenuState[i].iKartAccelerationStat + m_mPerPlayerMenuState[i].iDriverAccelerationStat) + ".xml");
				m_mPerPlayerMenuState[i].pHandlingStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/handling_" + std::to_string(m_mPerPlayerMenuState[i].iKartHandlingStat + m_mPerPlayerMenuState[i].iDriverHandlingStat) + ".xml");
				m_mPerPlayerMenuState[i].pDurabilityStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/durability_" + std::to_string(m_mPerPlayerMenuState[i].iKartDurabilityStat + m_mPerPlayerMenuState[i].iDriverDurabilityStat) + ".xml");
			}

			else if (bRight && !m_mPerPlayerMenuState[i].bReady)
			{
				// save the y rotation to apply it to the new kart/driver
				glm::quat qCurrentRotation = m_mPerPlayerMenuState[i].pDriverDisplay->GetTransform().GetRotation();

				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pSpeedStatIcon);
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pAccelerationStatIcon);
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pHandlingStatIcon);
				m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDurabilityStatIcon);

				if (m_mPerPlayerMenuState[i].bDriverHighlighted)
				{
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDriverSelection);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDriverDisplay);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pDriverAbilities);

					// TODO - we need to change this to be more dynamic/data-driven
					switch (m_mPerPlayerMenuState[i].eSelectedDriver)
					{
					case DRIVER_CLEOPAPA:
						m_mPerPlayerMenuState[i].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_clockmaker_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_clockmaker_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_clockmaker_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_CLOCKMAKER;
						m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
						m_mPerPlayerMenuState[i].iDriverAccelerationStat = -1;
						m_mPerPlayerMenuState[i].iDriverHandlingStat = 1;
						m_mPerPlayerMenuState[i].iDriverDurabilityStat = 0;
						break;

					case DRIVER_CLOCKMAKER:
						m_mPerPlayerMenuState[i].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_kingpin_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_kingpin_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_kingpin_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_KINGPIN;
						m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
						m_mPerPlayerMenuState[i].iDriverAccelerationStat = 1;
						m_mPerPlayerMenuState[i].iDriverHandlingStat = -1;
						m_mPerPlayerMenuState[i].iDriverDurabilityStat = 0;
						break;

					case DRIVER_KINGPIN:
						m_mPerPlayerMenuState[i].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_cleopapa_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_cleopapa_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_cleopapa_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedDriver = DRIVER_CLEOPAPA;
						m_mPerPlayerMenuState[i].iDriverSpeedStat = 0;
						m_mPerPlayerMenuState[i].iDriverAccelerationStat = 0;
						m_mPerPlayerMenuState[i].iDriverHandlingStat = 1;
						m_mPerPlayerMenuState[i].iDriverDurabilityStat = -1;
						break;
					}

					m_mPerPlayerMenuState[i].pDriverDisplay->SetParent(m_mPerPlayerMenuState[i].pDriverKartDisplayConnector);
				}
				else
				{
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pKartSelection);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pKartDisplay);
					m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[i].pKartAbilities);

					// TODO - we need to change this to be more dynamic/data-driven
					switch (m_mPerPlayerMenuState[i].eSelectedKart)
					{
					case KART_JUGGERNAUT:
						m_mPerPlayerMenuState[i].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_showoff_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_showoff_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_showoff_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedKart = KART_SHOWOFF;
						m_mPerPlayerMenuState[i].iKartSpeedStat = 2;
						m_mPerPlayerMenuState[i].iKartAccelerationStat = 5;
						m_mPerPlayerMenuState[i].iKartHandlingStat = 3;
						m_mPerPlayerMenuState[i].iKartDurabilityStat = 2;
						break;

					case KART_SHOWOFF:
						m_mPerPlayerMenuState[i].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_speedster_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_speedster_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_speedster_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedKart = KART_SPEEDSTER;
						m_mPerPlayerMenuState[i].iKartSpeedStat = 3;
						m_mPerPlayerMenuState[i].iKartAccelerationStat = 3;
						m_mPerPlayerMenuState[i].iKartHandlingStat = 3;
						m_mPerPlayerMenuState[i].iKartDurabilityStat = 3;
						break;

					case KART_SPEEDSTER:
						m_mPerPlayerMenuState[i].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_juggernaut_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_juggernaut_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_juggernaut_" + strPlayerNum + ".xml");
						m_mPerPlayerMenuState[i].eSelectedKart = KART_JUGGERNAUT;
						m_mPerPlayerMenuState[i].iKartSpeedStat = 5;
						m_mPerPlayerMenuState[i].iKartAccelerationStat = 1;
						m_mPerPlayerMenuState[i].iKartHandlingStat = 1;
						m_mPerPlayerMenuState[i].iKartDurabilityStat = 5;
						break;
					}

					m_mPerPlayerMenuState[i].pKartDisplay->SetParent(m_mPerPlayerMenuState[i].pDriverKartDisplayConnector);
				}

				m_mPerPlayerMenuState[i].pSpeedStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/speed_" + std::to_string(m_mPerPlayerMenuState[i].iKartSpeedStat + m_mPerPlayerMenuState[i].iDriverSpeedStat) + ".xml");
				m_mPerPlayerMenuState[i].pAccelerationStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/acceleration_" + std::to_string(m_mPerPlayerMenuState[i].iKartAccelerationStat + m_mPerPlayerMenuState[i].iDriverAccelerationStat) + ".xml");
				m_mPerPlayerMenuState[i].pHandlingStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/handling_" + std::to_string(m_mPerPlayerMenuState[i].iKartHandlingStat + m_mPerPlayerMenuState[i].iDriverHandlingStat) + ".xml");
				m_mPerPlayerMenuState[i].pDurabilityStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/durability_" + std::to_string(m_mPerPlayerMenuState[i].iKartDurabilityStat + m_mPerPlayerMenuState[i].iDriverDurabilityStat) + ".xml");
			}
		}
	}

	// If at least one player is not ready, don't advance. Otherwise, advance.
	for (auto mPlayer : m_mPerPlayerMenuState)
	{
		if (mPlayer.bJoined && !mPlayer.bReady) return;
	}
	GoToTrackSelectionState();
}

void Kartaclysm::StatePlayerSelectionMenu::PreRender()
{
	if (m_bSuspended) return;
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StatePlayerSelectionMenu::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StatePlayerSelectionMenu::GenerateDriverList()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile("CS483/CS483/Kartaclysm/Data/Racer/drivers.xml");
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	tinyxml2::XMLElement* pRootElement = doc.RootElement();
	tinyxml2::XMLElement* pDriverElement = pRootElement->FirstChildElement("Driver");
	while (pDriverElement != nullptr)
	{
		std::string strDriverDefinitionPath = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(pDriverElement, "definition", strDriverDefinitionPath);
		m_vDrivers.push_back(strDriverDefinitionPath);

		pDriverElement = pDriverElement->NextSiblingElement("Driver");
	}
}

void Kartaclysm::StatePlayerSelectionMenu::GenerateKartList()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile("CS483/CS483/Kartaclysm/Data/Racer/karts.xml");
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	tinyxml2::XMLElement* pRootElement = doc.RootElement();
	tinyxml2::XMLElement* pKartElement = pRootElement->FirstChildElement("Kart");
	while (pKartElement != nullptr)
	{
		std::string strKartDefinitionPath = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(pKartElement, "definition", strKartDefinitionPath);
		m_vKarts.push_back(strKartDefinitionPath);

		pKartElement = pKartElement->NextSiblingElement("Kart");
	}
}

void Kartaclysm::StatePlayerSelectionMenu::AddPlayer(const unsigned int p_uiPlayerNum)
{
	assert(!m_mPerPlayerMenuState[p_uiPlayerNum].bJoined);
	m_mPerPlayerMenuState[p_uiPlayerNum].bJoined = true;
	m_mPerPlayerMenuState[p_uiPlayerNum].bReady = false;
	std::string strPlayerNum = std::to_string(p_uiPlayerNum);

	std::string strSpeed = std::to_string(m_mPerPlayerMenuState[p_uiPlayerNum].iKartSpeedStat + m_mPerPlayerMenuState[p_uiPlayerNum].iDriverSpeedStat);
	std::string strAcceleration = std::to_string(m_mPerPlayerMenuState[p_uiPlayerNum].iKartAccelerationStat + m_mPerPlayerMenuState[p_uiPlayerNum].iDriverAccelerationStat);
	std::string strHandling = std::to_string(m_mPerPlayerMenuState[p_uiPlayerNum].iKartHandlingStat + m_mPerPlayerMenuState[p_uiPlayerNum].iDriverHandlingStat);
	std::string strDurability = std::to_string(m_mPerPlayerMenuState[p_uiPlayerNum].iKartDurabilityStat + m_mPerPlayerMenuState[p_uiPlayerNum].iDriverDurabilityStat);

	if (m_mPerPlayerMenuState[p_uiPlayerNum].pPressStartToJoin != nullptr)
	{
		m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pPressStartToJoin);
		m_mPerPlayerMenuState[p_uiPlayerNum].pPressStartToJoin = nullptr;
	}

	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_selection_cleopapa_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pKartSelection = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_selection_speedster_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pReadyButton = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/not_ready_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pDescriptionBoxes = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/description_boxes_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_abilities_cleopapa_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pKartAbilities = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_abilities_speedster_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pStatDescription = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/description.xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pSpeedStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/speed_" + strSpeed + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pAccelerationStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/acceleration_" + strAcceleration + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pHandlingStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/handling_" + strHandling + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pDurabilityStatIcon = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/stats/durability_" + strDurability + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverKartDisplayConnector = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_kart_display_connector_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/driver_display_cleopapa_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pKartDisplay = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/kart_display_speedster_" + strPlayerNum + ".xml");
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverKartDisplayConnector->AddChild(m_mPerPlayerMenuState[p_uiPlayerNum].pDriverDisplay);
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverKartDisplayConnector->AddChild(m_mPerPlayerMenuState[p_uiPlayerNum].pKartDisplay);
	m_mPerPlayerMenuState[p_uiPlayerNum].pHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/highlight_driver_selection_" + strPlayerNum + ".xml");

	m_uiNumPlayers++;
}

void Kartaclysm::StatePlayerSelectionMenu::RemovePlayer(const unsigned int p_uiPlayerNum)
{
	assert(m_mPerPlayerMenuState[p_uiPlayerNum].bJoined);
	m_mPerPlayerMenuState[p_uiPlayerNum].bJoined = false;
	m_mPerPlayerMenuState[p_uiPlayerNum].bReady = false;
	std::string strPlayerNum = std::to_string(p_uiPlayerNum);

	if (m_mPerPlayerMenuState[p_uiPlayerNum].pPressStartToJoin == nullptr)
	{
		m_mPerPlayerMenuState[p_uiPlayerNum].pPressStartToJoin = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/press_start_to_join_" + strPlayerNum + ".xml");
	}

	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pDriverSelection);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pKartSelection);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pReadyButton);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pDescriptionBoxes);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pDriverAbilities);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pKartAbilities);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pStatDescription);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pSpeedStatIcon);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pAccelerationStatIcon);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pHandlingStatIcon);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pDurabilityStatIcon);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pDriverKartDisplayConnector);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pDriverDisplay);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pKartDisplay);
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pHighlight);

	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverSelection = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pKartSelection = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pReadyButton = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pDescriptionBoxes = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverAbilities = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pKartAbilities = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pStatDescription = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pSpeedStatIcon = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pAccelerationStatIcon = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pHandlingStatIcon = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pDurabilityStatIcon = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverKartDisplayConnector = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pDriverDisplay = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pKartDisplay = nullptr;
	m_mPerPlayerMenuState[p_uiPlayerNum].pHighlight = nullptr;

	m_uiNumPlayers--;
}

void Kartaclysm::StatePlayerSelectionMenu::SetReadyStatus(const unsigned int p_uiPlayerNum, const bool p_bReady)
{
	assert(m_mPerPlayerMenuState[p_uiPlayerNum].bJoined);
	assert(m_mPerPlayerMenuState[p_uiPlayerNum].pReadyButton != nullptr);
	if (m_mPerPlayerMenuState[p_uiPlayerNum].bReady == p_bReady) return;

	std::string strPlayerNum = std::to_string(p_uiPlayerNum);
	std::string strReadyStatus = p_bReady ? "ready_" : "not_ready_";

	m_mPerPlayerMenuState[p_uiPlayerNum].bReady = p_bReady;
	m_pGameObjectManager->DestroyGameObject(m_mPerPlayerMenuState[p_uiPlayerNum].pReadyButton);
	m_mPerPlayerMenuState[p_uiPlayerNum].pReadyButton = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/PlayerSelectionMenu/player_" + strPlayerNum + "/" + strReadyStatus + strPlayerNum + ".xml");
}

void Kartaclysm::StatePlayerSelectionMenu::GoToTrackSelectionState()
{
	PlayerInputMapping::Instance()->SetSplitscreenPlayers(m_uiNumPlayers);

	m_mContextParameters.insert(std::pair<std::string, std::string>("NumHumanRacers", std::to_string(m_uiNumPlayers)));
	unsigned int uiJoined = 0;

	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!m_mPerPlayerMenuState[i].bReady) continue;

		std::string strPlayerNum = std::to_string(uiJoined++);
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_StartPosition", strPlayerNum));

		// TODO - we need to change this to be more dynamic/data-driven
		switch (m_mPerPlayerMenuState[i].eSelectedDriver)
		{
		case DRIVER_CLEOPAPA:
			m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_DriverDefinitionFile", "CS483/CS483/Kartaclysm/Data/Racer/driver_cleopapa.xml"));
			break;

		case DRIVER_CLOCKMAKER:
			m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_DriverDefinitionFile", "CS483/CS483/Kartaclysm/Data/Racer/driver_clockmaker.xml"));
			break;

		case DRIVER_KINGPIN:
			m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_DriverDefinitionFile", "CS483/CS483/Kartaclysm/Data/Racer/driver_kingpin.xml"));
			break;
		}

		// TODO - we need to change this to be more dynamic/data-driven
		switch (m_mPerPlayerMenuState[i].eSelectedKart)
		{
		case KART_JUGGERNAUT:
			m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_KartDefinitionFile", "CS483/CS483/Kartaclysm/Data/Racer/kart_juggernaut.xml"));
			break;

		case KART_SHOWOFF:
			m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_KartDefinitionFile", "CS483/CS483/Kartaclysm/Data/Racer/kart_showoff.xml"));
			break;

		case KART_SPEEDSTER:
			m_mContextParameters.insert(std::pair<std::string, std::string>("Player" + strPlayerNum + "_KartDefinitionFile", "CS483/CS483/Kartaclysm/Data/Racer/kart_speedster.xml"));
			break;
		}
	}

#ifdef _DEBUG
	assert(uiJoined == m_uiNumPlayers);
#endif

	// TEMP - using hardcoded value of 1 for now
	int iNumAIRacers = 1;
	m_mContextParameters.insert(std::pair<std::string, std::string>("NumAIRacers", std::to_string(iNumAIRacers)));

	for (int i = 0; i < iNumAIRacers; ++i)
	{
		std::string strAIRacer = "AI_racer" + std::to_string(i);

		std::string strDriverDefinitionFile = GetRandomDriver();
		std::string strKartDefinitionFile = GetRandomKart();
		
		m_mContextParameters.insert(std::pair<std::string, std::string>(strAIRacer + "_DriverDefinitionFile", strDriverDefinitionFile));
		m_mContextParameters.insert(std::pair<std::string, std::string>(strAIRacer + "_KartDefinitionFile", strKartDefinitionFile));
		m_mContextParameters.insert(std::pair<std::string, std::string>(strAIRacer + "_StartPosition", std::to_string(i + m_uiNumPlayers)));
	}

	switch (m_uiNumPlayers)
	{
	case 1:
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player0_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_full.xml"));
		break;

	case 2:
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player0_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_top.xml"));
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player1_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_bottom.xml"));
		break;

	case 3:
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player0_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_top_left.xml"));
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player1_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_top_right.xml"));
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player2_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_bottom_left.xml"));
		break;

	case 4:
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player0_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_top_left.xml"));
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player1_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_top_right.xml"));
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player2_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_bottom_left.xml"));
		m_mContextParameters.insert(std::pair<std::string, std::string>("Player3_CameraDefinitionFile", "CS483/CS483/Kartaclysm/Data/Camera/camera_bottom_right.xml"));
		break;
	}

	if (m_mContextParameters.find("TrackDefinitionFile") == m_mContextParameters.end())
	{
		// TODO: Destroy all graphics
		m_pGameObjectManager->DestroyAllGameObjects();
		m_pStateMachine->Push(STATE_TRACK_SELECTION_MENU, m_mContextParameters);
	}
	else
	{
		m_pStateMachine->Pop();
		m_pStateMachine->Push(STATE_RACING, m_mContextParameters);
	}
}

std::string Kartaclysm::StatePlayerSelectionMenu::GetRandomDriver() const
{
	auto range = std::uniform_int_distribution<int>(0, m_vDrivers.size() - 1);
	return m_vDrivers[range(HeatStroke::Common::GetRNGesus())];
}

std::string Kartaclysm::StatePlayerSelectionMenu::GetRandomKart() const
{
	auto range = std::uniform_int_distribution<int>(0, m_vKarts.size() - 1);
	return m_vKarts[range(HeatStroke::Common::GetRNGesus())];
}
