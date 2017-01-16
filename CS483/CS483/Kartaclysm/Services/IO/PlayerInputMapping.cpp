//====================================================================================
// PlayerInputMapping
// Author: Bradley Cooper
//
// Map inputs to an action for a player.
//====================================================================================

#include "PlayerInputMapping.h"

#include <cassert>
#include "EventManager.h"

namespace Kartaclysm
{
	// Static singleton instance
	PlayerInputMapping* PlayerInputMapping::s_pPlayerInputMappingInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Returns:   void
	// 
	// Creates the singleton instance.
	//------------------------------------------------------------------------------
	void PlayerInputMapping::CreateInstance()
	{
		assert(s_pPlayerInputMappingInstance == nullptr);
		s_pPlayerInputMappingInstance = new PlayerInputMapping();
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void PlayerInputMapping::DestroyInstance()
	{
		assert(s_pPlayerInputMappingInstance != nullptr);
		delete s_pPlayerInputMappingInstance;
		s_pPlayerInputMappingInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   PlayerInputMapping::PlayerInputMapping*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	PlayerInputMapping* PlayerInputMapping::Instance()
	{
		assert(s_pPlayerInputMappingInstance != nullptr);
		return s_pPlayerInputMappingInstance;
	}

	//--------------------------------------------------------------------------
	// PlayerInputMapping::PlayerInputMapping
	//
	// Basic initialization.
	//--------------------------------------------------------------------------
	PlayerInputMapping::PlayerInputMapping()
		:
		m_pPlayers(new PlayerMap),
		m_iPlayersRacing(1),
		m_iPlayersConnected(0),
		m_bRaceMode(false)
	{
		m_pJoystickDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&PlayerInputMapping::JoystickCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("JoystickCallback", m_pJoystickDelegate);

		Init();
	}

	//--------------------------------------------------------------------------
	// PlayerInputMapping::~PlayerInputMapping
	//
	// Clear, delete, and set the map to nullptr.
	//--------------------------------------------------------------------------
	PlayerInputMapping::~PlayerInputMapping()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("JoystickCallback", m_pJoystickDelegate);
		delete m_pJoystickDelegate;
		m_pJoystickDelegate = nullptr;

		m_pPlayers->clear();
		delete m_pPlayers;
		m_pPlayers = nullptr;
	}

	//--------------------------------------------------------------------------
	// PlayerInputMapping::Init
	//
	// Initializes this PlayerInputMapping with the map data needed to run.
	//--------------------------------------------------------------------------
	void PlayerInputMapping::Init()
	{
		// Try to set an initial input for each player
		for (int i = 0; i < m_iPlayersRacing; i++)
		{
			// Not the most efficient, but good enough
			AssignInput(i, GetFirstAvailableInput());
		}
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::AssignInput
	//
	// Assign input to player. Returns if mapping was successful.
	//--------------------------------------------------------------------------------
	bool PlayerInputMapping::AssignInput(const int p_iPlayer, const int p_iGLFWJoystick)
	{
		if (p_iPlayer < 0 || p_iPlayer > m_iPlayersConnected)
		{
			assert(false && "Invalid player number");
			return false;
		}

		int iPlayerUsingInput = PlayerUsingInput(p_iGLFWJoystick);
		if (iPlayerUsingInput == -2)
		{
			assert(false && "Invalid input");
			return false;
		}
		else if (p_iGLFWJoystick != -1 && iPlayerUsingInput != -1 && iPlayerUsingInput != p_iPlayer)
		{
			return false; // input in use by another player
		}

		auto find = m_pPlayers->find(p_iPlayer);
		if (p_iGLFWJoystick != -1 && (find == m_pPlayers->end() || find->second == -1))
		{
			m_iPlayersConnected++;
		}
		else if (p_iGLFWJoystick == -1 && find->second != -1)
		{
			m_iPlayersConnected--;
		}

		(*m_pPlayers)[p_iPlayer] = p_iGLFWJoystick;
		SendInputAssignmentEvent(p_iPlayer);
		return true;
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::PlayerUsingInput
	//
	// Returns number of player using input, -1 if not in use, or -2 if invalid.
	// If -1 is passed, returns first player without an input.
	//--------------------------------------------------------------------------------
	int PlayerInputMapping::PlayerUsingInput(const int p_iGLFWJoystick)
	{
		if (p_iGLFWJoystick != GLFW_JOYSTICK_LAST + 1 &&
			p_iGLFWJoystick != GLFW_JOYSTICK_LAST + 2 &&
			p_iGLFWJoystick != GLFW_JOYSTICK_LAST + 3 &&
			p_iGLFWJoystick != GLFW_JOYSTICK_LAST + 4 &&
			!glfwJoystickPresent(p_iGLFWJoystick))
		{
			return -2;
		}

		PlayerMap::iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
		for (; it != end; it++)
		{
			if (it->second == p_iGLFWJoystick)
			{
				return it->first;
			}
		}

		return -1;
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::SendInputAssignmentEvent
	//
	// Queue event listing button mappings for player
	//--------------------------------------------------------------------------------
	void PlayerInputMapping::SendInputAssignmentEvent(const int p_iPlayer)
	{
		int iJoystick = m_pPlayers->at(p_iPlayer);
		Input::Type eType;
		switch (iJoystick)
		{
			case -1: return;
			case GLFW_JOYSTICK_LAST + 1: eType = Input::eKeyboard1; break;
			case GLFW_JOYSTICK_LAST + 2: eType = Input::eKeyboard2; break;
			case GLFW_JOYSTICK_LAST + 3: eType = Input::eKeyboard3; break;
			case GLFW_JOYSTICK_LAST + 4: eType = Input::eKeyboard4; break;
			default: eType = Input::eJoystick; break;
		}

		HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerInputMap");
		pEvent->SetIntParameter("Player", p_iPlayer);

		for (int i = 0; i < 10; i++)
		{
			Racer::Action eAction;
			std::string strAction;
			switch (i)
			{
				case 0:  strAction = "Accelerate";			eAction = Racer::eAccelerate; break;
				case 1:  strAction = "Brake";				eAction = Racer::eBrake; break;
				case 2:  strAction = "Left";				eAction = Racer::eLeft; break;
				case 3:  strAction = "Right";				eAction = Racer::eRight; break;
				case 4:  strAction = "Slide";				eAction = Racer::eSlide; break;
				case 5:  strAction = "DriverAbility1";		eAction = Racer::eDriverAbility1; break;
				case 6:  strAction = "DriverAbility2";		eAction = Racer::eDriverAbility2; break;
				case 7:  strAction = "KartAbility1";		eAction = Racer::eKartAbility1; break;
				case 8:  strAction = "KartAbility2";		eAction = Racer::eKartAbility2; break;
				case 9:  strAction = "Pause";				eAction = Racer::ePause; break;
			}

			int iButton = InputActionMapping::Instance()->GetButtonMapping(eType, eAction);
			std::string strMapping = GetButtonString(eType, iButton);
			pEvent->SetStringParameter(strAction, strMapping);
		}

		HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::GetButtonString
	//
	// Get string representation for a button, or "" if unavailable
	//--------------------------------------------------------------------------------
	std::string PlayerInputMapping::GetButtonString(Input::Type eType, const int p_iButton) const
	{
		// TODO: Data drive these strings using a map
		if (eType == Input::eJoystick)
		{
			switch (p_iButton)
			{
				case XBOX_A: return "A";
				case XBOX_B: return "B";
				case XBOX_X: return "X";
				case XBOX_Y: return "Y";
				case XBOX_LB: return "LB";
				case XBOX_RB: return "RB";
				case XBOX_BACK: return "Back";
				case XBOX_START: return "Start";
				case XBOX_L3: return "L3";
				case XBOX_R3: return "R3";
				case XBOX_UP: return "Up";
				case XBOX_RIGHT: return "Right";
				case XBOX_DOWN: return "Down";
				case XBOX_LEFT: return "Left";
				default: return "";
			}
		}
		else
		{
			// TODO: More support for keyboard strings
			if (p_iButton <= 127) // converts to ASCII
			{
				return std::string(1, static_cast<char>(p_iButton));
			}
		}
		return "";
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::QueryPlayerMovement
	// Parameter:	const int p_iPlayerNum - number of players in race
	//				int& p_iAccelerate - Gets value of accelerate bool
	//				int& p_iBrake - Gets value of brake bool
	//				int& p_iSlide - Gets value of slide bool
	//				float& p_fTurn - Gets value of turning float
	// Returns: 
	//
	// Queries the movement properties of a single racer.
	//--------------------------------------------------------------------------------
	void PlayerInputMapping::QueryPlayerMovement(
		const int p_iPlayerNum,
		int& p_iAccelerate,
		int& p_iBrake,
		int& p_iSlide,
		float& p_fTurn) const
	{
		int iJoystick = m_pPlayers->at(p_iPlayerNum);
		p_iAccelerate = static_cast<int>(InputActionMapping::Instance()->GetButton(iJoystick, Racer::eAccelerate));
		p_iBrake = static_cast<int>(InputActionMapping::Instance()->GetButton(iJoystick, Racer::eBrake));
		p_iSlide = static_cast<int>(InputActionMapping::Instance()->GetButton(iJoystick, Racer::eSlide));
		p_fTurn = InputActionMapping::Instance()->GetTurning(iJoystick);
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::SetSplitscreenPlayers
	// Parameter:	const int p_iNumPlayers - number of players in race
	// Returns: bool
	//
	// Sets number of players to track when race mode is enabled.
	// Also changes current mapping of players to accomodate the new number.
	// Returns if there are enough inputs connected for every player to be mapped.
	//--------------------------------------------------------------------------------
	bool PlayerInputMapping::SetSplitscreenPlayers(const int p_iNumPlayers)
	{
		m_iPlayersRacing = p_iNumPlayers;
		int iAssigned = m_iPlayersConnected;

		int iLimit = (p_iNumPlayers > m_iPlayersConnected ? p_iNumPlayers : m_iPlayersConnected);
		for (int i = 0; i < iLimit; i++)
		{
			if (i < iAssigned)
			{
				SendInputAssignmentEvent(i);
			}
			else if (i > p_iNumPlayers)
			{
				AssignInput(i, -1);
			}
			else if (!AssignInput(i, GetFirstAvailableInput()))
			{
				m_iPlayersRacing--;
			}
		}

		return m_iPlayersConnected == m_iPlayersRacing;
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::GetFirstAvailableInput
	// Returns: int
	//
	// Returns Joystick number, or GLFW_JOYSTICK_LAST + 1 for keyboard, or -1 if none available
	//--------------------------------------------------------------------------------
	int PlayerInputMapping::GetFirstAvailableInput()
	{
		for (int i = 0; i <= GLFW_JOYSTICK_LAST + 4; i++)
		{
			if (PlayerUsingInput(i) == -1)
			{
				return i;
			}
		}

		return -1;
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::Update
	// Parameter: float fDelta
	//
	// Iterate through players and call method for their input types
	//--------------------------------------------------------------------------------
	void PlayerInputMapping::Update(const float p_fDelta)
	{
		PlayerMap::const_iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
		for (; it != end; it++)
		{
			if (it->second != -1) // keyboard or joystick connected for this player
			{
				InputActionMapping::Instance()->SendInputEventForPlayer(it->first, it->second);
			}
			else
			{
				// If a player is not connected, you should not be calling this during raceMode
				assert(m_bRaceMode);
			}
		}
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::JoystickCallback
	// Parameter:	const HeatStroke::Event* p_pEvent - "JoystickCallback" event
	//
	// Adds or removes joystick from player input mapping.
	//--------------------------------------------------------------------------------
	void PlayerInputMapping::JoystickCallback(const HeatStroke::Event* p_pEvent)
	{
		// Get required information from event
		int iGLFWJoystick, iJoystickEvent;
		p_pEvent->GetRequiredIntParameter("Joystick", iGLFWJoystick);
		p_pEvent->GetRequiredIntParameter("GLFWEvent", iJoystickEvent);

		if (iJoystickEvent == GLFW_CONNECTED)
		{
			// Add to first player without an input
			int iPlayerWithoutInput = PlayerUsingInput(-1);
			if (iPlayerWithoutInput != -1)
			{
				if (!m_bRaceMode)
				{
					AssignInput(iPlayerWithoutInput, iGLFWJoystick);
				}
				else
				{
					HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerReconnect");
					pEvent->SetIntParameter("Player", iPlayerWithoutInput);
					pEvent->SetIntParameter("LeftToConnect", m_iPlayersRacing - m_iPlayersConnected);
					HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
				}
			}
			else if (!m_bRaceMode)
			{
				// If not in race mode, push a player off keyboard and onto joystick
				int iKeyboardPlayer = PlayerUsingInput(GLFW_JOYSTICK_LAST + 1);
				if (iKeyboardPlayer >= 0 && !m_bRaceMode)
				{
					AssignInput(iKeyboardPlayer, iGLFWJoystick);
				}
			}
		}
		else if (iJoystickEvent == GLFW_DISCONNECTED)
		{
			// Remove from player who was mapped to this joystick
			int iPlayerUsingInput = PlayerUsingInput(iGLFWJoystick);
			if (iPlayerUsingInput != -1)
			{
				if (!m_bRaceMode)
				{
					AssignInput(iPlayerUsingInput, GetFirstAvailableInput());
				}
				else
				{
					HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerDisconnect");
					pEvent->SetIntParameter("Player", iPlayerUsingInput);
					pEvent->SetIntParameter("KeyboardInUse", (PlayerUsingInput(GLFW_JOYSTICK_LAST + 1) == -1 ? 0 : 1)); // can they map to keyboard?
					HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
				}
			}
		}
	}
}