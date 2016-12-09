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
			(*m_pPlayers)[i] = GetFirstAvailableInput();
			if ((*m_pPlayers)[i] != -1)
			{
				m_iPlayersConnected++;
			}
		}
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
		if (p_iNumPlayers > m_iPlayersRacing)
		{
			// Map additional players to first available inputs
			for (; m_iPlayersRacing < p_iNumPlayers; m_iPlayersRacing++)
			{
				// Not the most efficient, but good enough
				(*m_pPlayers)[m_iPlayersRacing] = GetFirstAvailableInput();
				if ((*m_pPlayers)[m_iPlayersRacing] != -1)
				{
					m_iPlayersConnected++;
				}
			}
		}
		else if (p_iNumPlayers < m_iPlayersRacing)
		{
			// Delete the additional mappings that are no longer needed
			for (; m_iPlayersRacing > p_iNumPlayers; m_iPlayersRacing--)
			{
				if ((*m_pPlayers)[m_iPlayersRacing] != -1)
				{
					m_iPlayersConnected--;
				}
				m_pPlayers->erase(m_pPlayers->at(m_iPlayersRacing));
			}
		}

		return m_iPlayersConnected == m_iPlayersRacing;
	}

	//--------------------------------------------------------------------------------
	// PlayerInputMapping::ManuallyMapPlayer
	// Parameter:	const int p_iPlayer - player to map
	//				const int p_iGLFWJoystick - Joystick number, GLFW_JOYSTICK_LAST + 1 for keyboard, or -1 to disable
	// Returns: bool
	//
	// Manually map player to input. Returns if successful (input not in use by anyone else).
	// Not that it matters, but returns true if p_iGLFWJoystick == -1.
	// Note: Make sure you pass a player number less than what is set by SetSplitscreenPlayers().
	//--------------------------------------------------------------------------------
	bool PlayerInputMapping::ManuallyMapPlayer(const int p_iPlayer, const int p_iGLFWJoystick)
	{
		assert(p_iPlayer <= m_iPlayersRacing);

		if (p_iGLFWJoystick == -1) // disable
		{
			(*m_pPlayers)[p_iPlayer] = -1;
			return true;
		}

		// Check if desired input is in use
		PlayerMap::iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
		bool bFound = false;
		for (; it != end; it++)
		{
			if (it->second == p_iGLFWJoystick)
			{
				bFound = true;
				break;
			}
		}

		if (!bFound) // input not in use: assign it
		{
			(*m_pPlayers)[p_iPlayer] = it->second;
			return true;
		}

		return false;
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
			if (i == GLFW_JOYSTICK_LAST + 1 ||
				i == GLFW_JOYSTICK_LAST + 2 || 
				i == GLFW_JOYSTICK_LAST + 3 || 
				i == GLFW_JOYSTICK_LAST + 4 ||
				glfwJoystickPresent(i))
			{
				// See if input is in use
				bool bFound = false;
				PlayerMap::iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
				for (; it != end; it++)
				{
					if (it->second == i)
					{
						bFound = true;
						break;
					}
				}

				// If not in use, return it
				if (!bFound)
				{
					return i;
				}
			}
		}

		// No available inputs
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
			bool bFound = false;
			int iKeyboardInUseBy = -1;
			PlayerMap::iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
			for (; it != end; it++)
			{
				if (it->second == -1)
				{
					it->second = iGLFWJoystick;
					m_iPlayersConnected++;
					bFound = true;
					break;
				}
				else if (it->second == GLFW_JOYSTICK_LAST + 1)
				{
					iKeyboardInUseBy = it->first;
				}
			}

			// If race mode is enabled, send out player reconnect event
			if (bFound && m_bRaceMode)
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerReconnect");
				pEvent->SetIntParameter("Player", it->first);
				pEvent->SetIntParameter("LeftToConnect", m_iPlayersRacing - m_iPlayersConnected);
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
			}

			// If not in race mode, push a player off keyboard and onto joystick
			if (iKeyboardInUseBy != -1 && !m_bRaceMode)
			{
				(*m_pPlayers)[iKeyboardInUseBy] = iGLFWJoystick;
			}
		}
		else if (iJoystickEvent == GLFW_DISCONNECTED)
		{
			// Remove from player who was mapped to this joystick
			bool bFound = false, bKeyboardInUse = false;
			PlayerMap::iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
			for (; it != end; it++)
			{
				if (it->second == iGLFWJoystick)
				{
					// TO DO, for now, hardcoded that you get assigned another input automatically
					// When implemented in game, this should be handled through the event below
					int iTemp = it->first;
					it->second = GetFirstAvailableInput();
					if ((*m_pPlayers)[iTemp] == -1)
					{
						m_iPlayersConnected--;
					}
					bFound = true;
				}
				else if (it->second == GLFW_JOYSTICK_LAST + 1)
				{
					bKeyboardInUse = true;
				}
			}

			// If race mode is enabled, send out player disconnect event
			if (bFound && m_bRaceMode)
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerDisconnect");
				pEvent->SetIntParameter("Player", it->first);
				pEvent->SetIntParameter("KeyboardInUse", (int)bKeyboardInUse); // can they map to keyboard?
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
			}


		}
	}
}