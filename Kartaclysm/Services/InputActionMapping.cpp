//====================================================================================
// InputActionMapping
// Author: Bradley Cooper
//
// Map inputs to an action for a player.
//====================================================================================

#include "InputActionMapping.h"

#include <cassert>
#include "EventManager.h"

// TO DO, figure out a better way to hardcode or set this value
#define MAX_PLAYERS 8

namespace Kartaclysm
{
	// Static singleton instance
	InputActionMapping* InputActionMapping::s_pInputActionMappingInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Parameter:	const std::string& p_strUserConfigFilePath - file path
	// Returns:   void
	// 
	// Creates the singleton instance.
	//------------------------------------------------------------------------------
	void InputActionMapping::CreateInstance(const std::string& p_strUserConfigFilePath)
	{
		assert(s_pInputActionMappingInstance == nullptr);
		s_pInputActionMappingInstance = new InputActionMapping(p_strUserConfigFilePath);
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void InputActionMapping::DestroyInstance()
	{
		assert(s_pInputActionMappingInstance != nullptr);
		delete s_pInputActionMappingInstance;
		s_pInputActionMappingInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   InputActionMapping::InputActionMapping*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	InputActionMapping* InputActionMapping::Instance()
	{
		assert(s_pInputActionMappingInstance != nullptr);
		return s_pInputActionMappingInstance;
	}

	//--------------------------------------------------------------------------
	// InputActionMapping::InputActionMapping
	// Parameter:	const std::string& p_strUserConfigFilePath - file path
	//
	// Basic initialization.
	//--------------------------------------------------------------------------
	InputActionMapping::InputActionMapping(const std::string& p_strUserConfigFilePath)
		:
		m_pPlayers(new PlayerMap),
		m_iJoysticks(0)
	{
		Init(p_strUserConfigFilePath);
	}

	//--------------------------------------------------------------------------
	// InputActionMapping::~InputActionMapping
	//
	// Clear, delete, and set the map to nullptr.
	//--------------------------------------------------------------------------
	InputActionMapping::~InputActionMapping()
	{
		m_pPlayers->clear();
		delete m_pPlayers;
		m_pPlayers = nullptr;
	}

	//--------------------------------------------------------------------------
	// InputActionMapping::Init
	// Parameter:	const std::string& p_strUserConfigFilePath - file path
	//
	// Initializes this InputActionMapping with the map data needed to run.
	//--------------------------------------------------------------------------
	void InputActionMapping::Init(const std::string& p_strUserConfigFilePath)
	{
		// Set all players to initially have no input
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			(*m_pPlayers)[i] = -1;
		}

		// Set first player to be controlled by keyboard
		(*m_pPlayers)[0] = GLFW_JOYSTICK_LAST + 1;

		// Set other players to be controlled by different joysticks
		// TO DO, create a better method to map player to joystick
		for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++)
		{
			if (glfwJoystickPresent(i))
			{
				(*m_pPlayers)[++m_iJoysticks] = i;
			}
		}

		LoadUserControlBindings(p_strUserConfigFilePath);
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::Update
	// Parameter: float fDelta
	//
	// Gathers user input into event to dispatch.
	//--------------------------------------------------------------------------------
	void InputActionMapping::Update(const float p_fDelta)
	{
		PlayerMap::const_iterator it = m_pPlayers->begin(), end = m_pPlayers->end();
		for (; it != end; it++)
		{
			if (it->second != -1) // keyboard or joystick connected for this player
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerInput");
				pEvent->SetIntParameter("Player", it->first);

				if (it->second == GLFW_JOYSTICK_LAST + 1)
				{
					// Keyboard keys
					pEvent->SetIntParameter("Accelerate", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eAccelerate]));
					pEvent->SetIntParameter("Brake", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eBrake]));
					pEvent->SetIntParameter("Slide", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eSlide]));
					pEvent->SetIntParameter("Driver1", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eDriverAbility1]));
					pEvent->SetIntParameter("Driver2", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eDriverAbility2]));
					pEvent->SetIntParameter("Kart1", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eKartAbility1]));
					pEvent->SetIntParameter("Kart2", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eKartAbility2]));
					pEvent->SetIntParameter("Pause", (int)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[ePause]));

					// For compatibility with joysticks, turn is a float value
					// Right adds +1.0f, Left adds -1.0f
					float fTurn = (float)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eRight]);
					fTurn -= (float)HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pKeyboardMap)[eLeft]);
					pEvent->SetFloatParameter("Turn", fTurn);
				}
				else
				{
					// Joystick buttons
					pEvent->SetIntParameter("Accelerate", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eAccelerate]));
					pEvent->SetIntParameter("Brake", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eBrake]));
					pEvent->SetIntParameter("Slide", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eSlide]));
					pEvent->SetIntParameter("Driver1", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eDriverAbility1]));
					pEvent->SetIntParameter("Driver2", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eDriverAbility2]));
					pEvent->SetIntParameter("Kart1", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eKartAbility1]));
					pEvent->SetIntParameter("Kart2", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eKartAbility2]));
					pEvent->SetIntParameter("Pause", (int)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[ePause]));

					// Joystick axis takes priority over d-pad
					float fTurn = HeatStroke::JoystickInputBuffer::Instance()->GetAxis(it->second, (*m_pJoystickMap)[eJoystick]);
					if (fTurn == 0.0f) // TO DO, test if dead zone needs to be implemented
					{
						fTurn = (float)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eRight]);
						fTurn -= (float)HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(it->second, (*m_pJoystickMap)[eLeft]);
					}
					pEvent->SetFloatParameter("Turn", fTurn);
				}

				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
			}
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::LoadUserControlBindings
	// Parameter:	const std::string& p_strUserConfigFilePath - file path
	//
	// Load user control bindings from XML.
	// If invalid configuration found, keeps controls to default.
	//--------------------------------------------------------------------------------
	void InputActionMapping::LoadUserControlBindings(const std::string& p_strUserConfigFilePath)
	{
		// Set default values and only overwrite if valid control bindings found
		(*m_pKeyboardMap)[eAccelerate] = GLFW_KEY_UP;
		(*m_pKeyboardMap)[eBrake] = GLFW_KEY_DOWN;
		(*m_pKeyboardMap)[eLeft] = GLFW_KEY_LEFT;
		(*m_pKeyboardMap)[eRight] = GLFW_KEY_RIGHT;
		(*m_pKeyboardMap)[eSlide] = GLFW_KEY_SPACE;
		(*m_pKeyboardMap)[eDriverAbility1] = GLFW_KEY_A;
		(*m_pKeyboardMap)[eDriverAbility2] = GLFW_KEY_S;
		(*m_pKeyboardMap)[eKartAbility1] = GLFW_KEY_D;
		(*m_pKeyboardMap)[eKartAbility2] = GLFW_KEY_F;
		(*m_pKeyboardMap)[ePause] = GLFW_KEY_ESCAPE;

		// Input button references are based on XBox controller
		// TO DO, define these constants better
		(*m_pJoystickMap)[eAccelerate] = 0;			// A
		(*m_pJoystickMap)[eBrake] = 1;				// B
		(*m_pJoystickMap)[eLeft] = 9;				// Left d-pad
		(*m_pJoystickMap)[eRight] = 10;				// Right d-pad
		(*m_pJoystickMap)[eSlide] = 2;				// Right bumper
		(*m_pJoystickMap)[eDriverAbility1] = 3;		// X
		(*m_pJoystickMap)[eDriverAbility2] = 4;		// Y
		(*m_pJoystickMap)[eKartAbility1] = 5;		// Left trigger
		(*m_pJoystickMap)[eKartAbility2] = 6;		// Right trigger
		(*m_pJoystickMap)[ePause] = 8;				// Start
		(*m_pJoystickMap)[eJoystick] = 0;			// Left axis

		// Look for XML file and parse into user control bindings
		// TO DO, find a better way to handle early returns, possibly with a soft assert
		tinyxml2::XMLDocument doc;
		if (doc.Parse(p_strUserConfigFilePath.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			return;
		}

		// Make sure the file is formatted correctly and we have the right XML file
		tinyxml2::XMLElement* pElement = doc.FirstChildElement();
		if (strcmp(pElement->Name.c_str(), "ControlBindings") != 0)
		{
			return;
		}

		// First loop is for keyboard mapping, second is for joystick mapping
		for (int i = 0; i < 2; i++)
		{
			// Build a map up, making sure each action is mapped to a unique key
			InputMap inputMap = InputMap();
			bool bError = false;

			// 
			int iLoop = (i == 0 ? 10 : 11);
			std::string strInputName = (i == 0 ? "Keyboard" : "Joystick");

			// Make sure the attribute type is found in the XML document
			tinyxml2::XMLElement* pInputType = (tinyxml2::XMLElement*)HeatStroke::EasyXML::FindChildNode(pElement, strInputName.c_str());
			while (pInputType != nullptr) // NOTE: ONLY USED FOR EARLY 'BREAK' COMMANDS
			{
				for (int j = 0; j < iLoop; j++) // TO DO, hardcoded value and only supports one joystick map
				{
					int iKey = -1;
					std::string strAtrributeName;
					RacerAction eAction;

					// Select attribute to search and map key for insertion
					switch (j)
					{
					case 0:  strAtrributeName = "accelerate";	  eAction = eAccelerate; break;
					case 1:  strAtrributeName = "brake";		  eAction = eBrake; break;
					case 2:  strAtrributeName = "left";			  eAction = eLeft; break;
					case 3:  strAtrributeName = "right";		  eAction = eRight; break;
					case 4:  strAtrributeName = "slide";		  eAction = eSlide; break;
					case 5:  strAtrributeName = "driverAbility1"; eAction = eDriverAbility1; break;
					case 6:  strAtrributeName = "driverAbility2"; eAction = eDriverAbility2; break;
					case 7:  strAtrributeName = "kartAbility1";	  eAction = eKartAbility1; break;
					case 8:  strAtrributeName = "kartAbility2";	  eAction = eKartAbility2; break;
					case 9:  strAtrributeName = "pause";		  eAction = ePause; break;
						// The following case(s) do not apply to Keyboards
					case 10: strAtrributeName = "joystick";		  eAction = eJoystick; break;
					}

					// Search for the attribute and get its value
					tinyxml2::XMLElement* pAttribute = (tinyxml2::XMLElement*)HeatStroke::EasyXML::FindChildNode(pElement, strAtrributeName.c_str());
					HeatStroke::EasyXML::GetOptionalIntAttribute(pAttribute, "value", iKey, -1);

					// Validate that the value was found
					if (iKey == -1)
					{
						bError = true;
						break;
					}

					// Validate that the key is tracked by GLFW if keyboard input
					// TO DO, method to validate joystick input
					if (i == 0 && !HeatStroke::KeyboardInputBuffer::Instance()->IsValidKey(iKey))
					{
						bError = true;
						break;
					}

					// Validate that the value is uniquely mapped
					InputMap::const_iterator it = inputMap.begin(), end = inputMap.end();
					for (; it != end; it++)
					{
						if (it->second == iKey)
						{
							bError = true;
							break;
						}
					}
					if (bError) break;

					// Valid key: add it to the mapping
					inputMap[eAction] = iKey;
				}
				if (bError) break;

				// If mapping is valid, copy it over to the class
				// TO DO, make sure this isn't cut off the stack... not sure as I code it...
				if (i == 0)
				{
					(*m_pKeyboardMap) = std::move(inputMap);
				}
				else
				{
					(*m_pJoystickMap) = std::move(inputMap);
				}
				pInputType = nullptr;
				break;
			}
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::JoystickCallback
	// Parameter:	const int p_iGLFWJoystick - GLFW integer for joystick
	//				const int p_iJoystickEvent - GLFW_CONNECTED or GLFW_DISCONNECTED
	//
	// Adds or removes joystick from player input mapping.
	//--------------------------------------------------------------------------------
	void InputActionMapping::JoystickCallback(const int p_iGLFWJoystick, const int p_iJoystickEvent)
	{
		PlayerMap::iterator it = m_pPlayers->begin(), end = m_pPlayers->end();

		if (p_iJoystickEvent == GLFW_CONNECTED)
		{
			// Add to first player without an input
			for (; it != end; it++)
			{
				if (it->second == -1)
				{
					it->second = p_iGLFWJoystick;
				}
			}
		}
		else if (p_iJoystickEvent == GLFW_DISCONNECTED)
		{
			// Remove from player who was mapped to this joystick
			for (; it != end; it++)
			{
				if (it->second == p_iGLFWJoystick)
				{
					it->second = -1;
				}
			}
		}
	}
}