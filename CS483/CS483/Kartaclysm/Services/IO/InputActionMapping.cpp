//====================================================================================
// InputActionMapping
// Author: Bradley Cooper
//
// Map inputs to an action for a player.
//====================================================================================

#include "InputActionMapping.h"

#include <cassert>
#include "EventManager.h"

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
		m_pInputMap(new InputMap()),
		m_strUserConfigFilePath(p_strUserConfigFilePath)
	{
		Init();
	}

	//--------------------------------------------------------------------------
	// InputActionMapping::~InputActionMapping
	//
	// Clear, delete, and set the map to nullptr.
	//--------------------------------------------------------------------------
	InputActionMapping::~InputActionMapping()
	{
		m_pInputMap->clear();
		delete m_pInputMap;
		m_pInputMap = nullptr;
	}

	//--------------------------------------------------------------------------
	// InputActionMapping::Init
	//
	// Initializes this InputActionMapping with the map data needed to run.
	//--------------------------------------------------------------------------
	void InputActionMapping::Init()
	{
		(*m_pInputMap)[Input::eKeyboard] = ActionMap();
		(*m_pInputMap)[Input::eJoystick] = ActionMap();

		LoadUserControlBindings();
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::SendInputEventForPlayer
	// Parameter:	const int p_iPlayer - player number to put in event
	//				const int p_iGLFWJoystick - Joystick number, or GLFW_JOYSTICK_LAST + 1 for keyboard
	//
	// Gathers user input into event for dispatch.
	//--------------------------------------------------------------------------------
	void InputActionMapping::SendInputEventForPlayer(const int p_iPlayer, const int p_iGLFWJoystick)
	{
		// The event for controller disconnect or reconnect is queued (not triggered immediately)
		// meaning the PlayerInputMapping may be delayed some frames before updating its mapping
		// As such, we cannot use an assert() here, and will have to handle it quietly
		if (p_iGLFWJoystick != GLFW_JOYSTICK_LAST + 1 && !glfwJoystickPresent(p_iGLFWJoystick))
		{
			return;
		}

		HeatStroke::Event* pEvent = new HeatStroke::Event("PlayerInput_" + p_iPlayer);

		if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 1)
		{
			// Keyboard keys
			pEvent->SetIntParameter("Accelerate", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eAccelerate])));
			pEvent->SetIntParameter("Brake", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eBrake])));
			pEvent->SetIntParameter("Slide", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eSlide])));
			pEvent->SetIntParameter("Driver1", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eDriverAbility1])));
			pEvent->SetIntParameter("Driver2", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eDriverAbility2])));
			pEvent->SetIntParameter("Kart1", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eKartAbility1])));
			pEvent->SetIntParameter("Kart2", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eKartAbility2])));
			pEvent->SetIntParameter("Pause", static_cast<int>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::ePause])));

			// For compatibility with joysticks, turn is a float value
			// Right adds +1.0f, Left adds -1.0f
			float fTurn = static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eRight]));
			fTurn -= static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown((*m_pInputMap)[Input::eKeyboard][Racer::eLeft]));
			pEvent->SetFloatParameter("Turn", fTurn);
		}
		else
		{
			// Joystick buttons
			pEvent->SetIntParameter("Accelerate", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eAccelerate])));
			pEvent->SetIntParameter("Brake", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eBrake])));
			pEvent->SetIntParameter("Driver1", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eDriverAbility1])));
			pEvent->SetIntParameter("Driver2", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eDriverAbility2])));
			pEvent->SetIntParameter("Kart1", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eKartAbility1])));
			pEvent->SetIntParameter("Kart2", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eKartAbility2])));
			pEvent->SetIntParameter("Pause", static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::ePause])));

			// Analog stick takes priority over d-pad for turning
			float fTurn = HeatStroke::JoystickInputBuffer::Instance()->GetAxis(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eAnalogStick]);
			if (fTurn == 0.0f)
			{
				fTurn = static_cast<float>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eRight]));
				fTurn -= static_cast<float>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eLeft]));
			}
			pEvent->SetFloatParameter("Turn", fTurn);

			// Sliding may be controlled by button OR by RT/LT axis
			int iSlide = static_cast<int>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDown(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eSlide]));
			if (iSlide == 0)
			{
				float fSlide = HeatStroke::JoystickInputBuffer::Instance()->GetAxis(p_iGLFWJoystick, XBOX_AXIS_TRIGGERS);
				iSlide = (fSlide == 0.0f ? 0 : 1);
			}
			pEvent->SetIntParameter("Slide", iSlide);
		}

		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::ResetUserControlBindings
	// Parameter:	const bool p_bResetKeyboard - reset keyboard controls
	//				const bool p_bResetJoystick - reset joystick controls
	//
	// Resets keyboard and/or joystick controls to default.
	//--------------------------------------------------------------------------------
	void InputActionMapping::ResetUserControlBindings(const bool p_bResetKeyboard, const bool p_bResetJoystick)
	{
		if (p_bResetKeyboard)
		{
			(*m_pInputMap)[Input::eKeyboard][Racer::eAccelerate] =		GLFW_KEY_UP;
			(*m_pInputMap)[Input::eKeyboard][Racer::eBrake] =			GLFW_KEY_DOWN;
			(*m_pInputMap)[Input::eKeyboard][Racer::eLeft] =			GLFW_KEY_LEFT;
			(*m_pInputMap)[Input::eKeyboard][Racer::eRight] =			GLFW_KEY_RIGHT;
			(*m_pInputMap)[Input::eKeyboard][Racer::eSlide] =			GLFW_KEY_SPACE;
			(*m_pInputMap)[Input::eKeyboard][Racer::eDriverAbility1] =	GLFW_KEY_A;
			(*m_pInputMap)[Input::eKeyboard][Racer::eDriverAbility2] =	GLFW_KEY_S;
			(*m_pInputMap)[Input::eKeyboard][Racer::eKartAbility1] =	GLFW_KEY_D;
			(*m_pInputMap)[Input::eKeyboard][Racer::eKartAbility2] =	GLFW_KEY_F;
			(*m_pInputMap)[Input::eKeyboard][Racer::ePause] =			GLFW_KEY_ESCAPE;
		}

		if (p_bResetJoystick)
		{
			(*m_pInputMap)[Input::eJoystick][Racer::eAccelerate] =		XBOX_A;
			(*m_pInputMap)[Input::eJoystick][Racer::eBrake] =			XBOX_B;
			(*m_pInputMap)[Input::eJoystick][Racer::eLeft] =			XBOX_LEFT;
			(*m_pInputMap)[Input::eJoystick][Racer::eRight] =			XBOX_RIGHT;
			(*m_pInputMap)[Input::eJoystick][Racer::eSlide] =			XBOX_L3;
			(*m_pInputMap)[Input::eJoystick][Racer::eDriverAbility1] =	XBOX_X;
			(*m_pInputMap)[Input::eJoystick][Racer::eDriverAbility2] =	XBOX_Y;
			(*m_pInputMap)[Input::eJoystick][Racer::eKartAbility1] =	XBOX_LB;
			(*m_pInputMap)[Input::eJoystick][Racer::eKartAbility2] =	XBOX_RB;
			(*m_pInputMap)[Input::eJoystick][Racer::ePause] =			XBOX_START;
			(*m_pInputMap)[Input::eJoystick][Racer::eAnalogStick] =		XBOX_AXIS_LEFT_X;
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::LoadUserControlBindings
	//
	// Load user control bindings from XML.
	// If invalid configuration found, resets controls to default for that input.
	// Returns true if the file was found.
	//--------------------------------------------------------------------------------
	void InputActionMapping::LoadUserControlBindings()
	{
		// Make sure we can load the XML file
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(m_strUserConfigFilePath.c_str()) != tinyxml2::XML_NO_ERROR)
		{
#ifdef _DEBUG
			assert(false && "Failure to load ControlBindings.xml");
#endif
			ResetUserControlBindings();
			return;
		}

		// Make sure the file is formatted correctly and we have the right XML file
		tinyxml2::XMLElement* pElement = doc.RootElement();
		if (strcmp(pElement->Name(), "ControlBindings") != 0)
		{
#ifdef _DEBUG
			assert(false && "ControlBindings.xml not formatted correctly");
#endif
			ResetUserControlBindings();
			return;
		}

		// Make sure the mappings have been created for the different input types (keyboard, joystick)
		InputMap::const_iterator it = m_pInputMap->begin(), end = m_pInputMap->end();
		if (it == end)
		{
			ResetUserControlBindings();
			return;
		}

		// Loop each input type to build the mapping
		for (; it != end; it++)
		{
			// Build a map up, making sure each action is mapped to a unique key
			ActionMap mActionMap = ActionMap();
			bool bError = false;

			// Find title of XML element to read, and the number of buttons corresponding to that input
			std::string strInputName;
			int iActionsToMap;
			switch (it->first)
			{
				case Input::eKeyboard: strInputName = "Keyboard"; iActionsToMap = 10; break;
				case Input::eJoystick: strInputName = "Joystick"; iActionsToMap = 11; break;
			}

			// Make sure the attribute type is found in the XML document
			tinyxml2::XMLElement* pInputType = static_cast<tinyxml2::XMLElement*>(HeatStroke::EasyXML::FindChildNode(pElement, strInputName.c_str()));
			if (pInputType == nullptr)
			{
#ifdef _DEBUG
				assert(false && "ControlBindings.xml does not contain input type");
#endif
				ResetUserControlBindings(it->first == Input::eKeyboard, it->first == Input::eJoystick);
				continue;
			}

			// Loop however many inputs need to be mapped for this input type
			for (int j = 0; j < iActionsToMap; j++)
			{
				// Select attribute to search, and the map key for insertion
				std::string strAtrributeName;
				Racer::Action eAction;
				switch (j)
				{
				case 0:  strAtrributeName = "accelerate";			eAction = Racer::eAccelerate; break;
				case 1:  strAtrributeName = "brake";				eAction = Racer::eBrake; break;
				case 2:  strAtrributeName = "left";					eAction = Racer::eLeft; break;
				case 3:  strAtrributeName = "right";				eAction = Racer::eRight; break;
				case 4:  strAtrributeName = "slide";				eAction = Racer::eSlide; break;
				case 5:  strAtrributeName = "driverAbility1";		eAction = Racer::eDriverAbility1; break;
				case 6:  strAtrributeName = "driverAbility2";		eAction = Racer::eDriverAbility2; break;
				case 7:  strAtrributeName = "kartAbility1";			eAction = Racer::eKartAbility1; break;
				case 8:  strAtrributeName = "kartAbility2";			eAction = Racer::eKartAbility2; break;
				case 9:  strAtrributeName = "pause";				eAction = Racer::ePause; break;
					// The following case(s) only apply to Joysticks
				case 10: strAtrributeName = "analogStick";			eAction = Racer::eAnalogStick; break;
				}

				// Search for the attribute and get its value
				int iButton = -1;
				tinyxml2::XMLElement* pAttribute = static_cast<tinyxml2::XMLElement*>(HeatStroke::EasyXML::FindChildNode(pInputType, strAtrributeName.c_str()));
				HeatStroke::EasyXML::GetOptionalIntAttribute(pAttribute, "value", iButton, -1);

				// Validate that the value was found
				if (iButton == -1)
				{
					bError = true;
					break;
				}

				// Validate that the key is tracked by GLFW if keyboard input
				// TO DO, method to validate joystick input
				if (it->first == Input::eKeyboard && !HeatStroke::KeyboardInputBuffer::Instance()->IsValidKey(iButton))
				{
					bError = true;
					break;
				}

				// Validate that the value is uniquely mapped (skipping joystick analog sticks)
				if (j < 10)
				{
					ActionMap::const_iterator it2 = mActionMap.begin(), end2 = mActionMap.end();
					for (; it2 != end2; it2++)
					{
						if (it2->second == iButton)
						{
							bError = true;
							break;
						}
					}
					if (bError) break;
				}

				// Valid key: add it to the mapping
				mActionMap[eAction] = iButton;
			}

			if (bError)
			{
#ifdef _DEBUG
				printf("ControlBindings.xml error: restoring defaults");
#endif
				ResetUserControlBindings(it->first == Input::eKeyboard, it->first == Input::eJoystick);
			}
			else
			{
				(*m_pInputMap)[it->first] = std::move(mActionMap);
			}
		}
	}
}