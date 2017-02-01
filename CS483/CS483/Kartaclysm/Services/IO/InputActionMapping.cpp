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
		(*m_pInputMap)[Input::eKeyboard1] = ActionMap();
		(*m_pInputMap)[Input::eKeyboard2] = ActionMap();
		(*m_pInputMap)[Input::eKeyboard3] = ActionMap();
		(*m_pInputMap)[Input::eKeyboard4] = ActionMap();
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
		if ((p_iGLFWJoystick < GLFW_JOYSTICK_LAST && !glfwJoystickPresent(p_iGLFWJoystick)) || p_iGLFWJoystick > GLFW_JOYSTICK_LAST + 4)
		{
			return;
		}

		// Helpful variables to avoid repetition
		std::string strPlayerIdentifier = "Player" + std::to_string(p_iPlayer);
		HeatStroke::EventManager* pEventManager = HeatStroke::EventManager::Instance();

		if (p_iGLFWJoystick > GLFW_JOYSTICK_LAST) // Keyboard
		{
			// Helpful variables to avoid repetition
			HeatStroke::KeyboardInputBuffer* pBuffer = HeatStroke::KeyboardInputBuffer::Instance();

			ActionMap mActionMap;
			switch (p_iGLFWJoystick)
			{
			case GLFW_JOYSTICK_LAST + 1: mActionMap = (*m_pInputMap)[Input::eKeyboard1]; break;
			case GLFW_JOYSTICK_LAST + 2: mActionMap = (*m_pInputMap)[Input::eKeyboard2]; break;
			case GLFW_JOYSTICK_LAST + 3: mActionMap = (*m_pInputMap)[Input::eKeyboard3]; break;
			case GLFW_JOYSTICK_LAST + 4: mActionMap = (*m_pInputMap)[Input::eKeyboard4]; break;
			default: assert(false && "Not enough keyboards!!!");
			}

			// Iterate ability and pause actions
			if (pBuffer->IsKeyDownOnce(mActionMap[Racer::eDriverAbility1]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_DriverAbility1"));
			}
			if (pBuffer->IsKeyDownOnce(mActionMap[Racer::eDriverAbility2]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_DriverAbility2"));
			}
			if (pBuffer->IsKeyDownOnce(mActionMap[Racer::eKartAbility1]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_KartAbility1"));
			}
			if (pBuffer->IsKeyDownOnce(mActionMap[Racer::eKartAbility2]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_KartAbility2"));
			}
			if (pBuffer->IsKeyDownOnce(mActionMap[Racer::ePause]))
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event("Pause");
				pEvent->SetIntParameter("Player", p_iPlayer);
				pEventManager->TriggerEvent(pEvent);
			}
		}
		else // Not keyboard (joystick)
		{
			// Helpful variables to avoid repetition
			HeatStroke::JoystickInputBuffer* pBuffer = HeatStroke::JoystickInputBuffer::Instance();
			ActionMap mActionMap = (*m_pInputMap)[Input::eJoystick];

			// Iterate ability and pause actions
			if (pBuffer->IsButtonDownOnce(p_iGLFWJoystick, mActionMap[Racer::eDriverAbility1]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_DriverAbility1"));
			}
			if (pBuffer->IsButtonDownOnce(p_iGLFWJoystick, mActionMap[Racer::eDriverAbility2]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_DriverAbility2"));
			}
			if (pBuffer->IsButtonDownOnce(p_iGLFWJoystick, mActionMap[Racer::eKartAbility1]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_KartAbility1"));
			}
			if (pBuffer->IsButtonDownOnce(p_iGLFWJoystick, mActionMap[Racer::eKartAbility2]))
			{
				pEventManager->TriggerEvent(new HeatStroke::Event(strPlayerIdentifier + "_KartAbility2"));
			}
			if (pBuffer->IsButtonDownOnce(p_iGLFWJoystick, mActionMap[Racer::ePause]))
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event("Pause");
				pEvent->SetIntParameter("Player", p_iPlayer);
				pEventManager->TriggerEvent(pEvent);
			}
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::GetButton
	// Parameter:	const int p_iGLFWJoystick - Joystick number, or GLFW_JOYSTICK_LAST + 1 for keyboard
	//				const Racer::Action p_eAction - Action to query for the input
	//
	// Queries for an individual button press for the related action.
	//--------------------------------------------------------------------------------
	bool InputActionMapping::GetButton(const int p_iGLFWJoystick, const Racer::Action p_eAction)
	{
		if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 1)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard1][p_eAction]);
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 2)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard2][p_eAction]);
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 3)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard3][p_eAction]);
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 4)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard4][p_eAction]);
		}
		else
		{
			if (p_eAction == Racer::eSlide)
			{
				// Sliding is controlled by button or by the bumpers
				bool bSlide = HeatStroke::JoystickInputBuffer::Instance()->IsButtonDownContinuous(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eSlide]);
				return (bSlide == true ? true : static_cast<float>(HeatStroke::JoystickInputBuffer::Instance()->GetAxis(p_iGLFWJoystick, XBOX_AXIS_TRIGGERS)) != 0.0f);
			}
			else
			{
				return HeatStroke::JoystickInputBuffer::Instance()->IsButtonDownContinuous(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][p_eAction]);
			}
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::GetButtonOnce
	// Parameter:	const int p_iGLFWJoystick - Joystick number, or GLFW_JOYSTICK_LAST + 1 for keyboard
	//				const Racer::Action p_eAction - Action to query for the input
	//
	// Queries for an individual button press for the related action only once.
	//--------------------------------------------------------------------------------
	bool InputActionMapping::GetButtonOnce(const int p_iGLFWJoystick, const Racer::Action p_eAction)
	{
		if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 1)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce((*m_pInputMap)[Input::eKeyboard1][p_eAction]);
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 2)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce((*m_pInputMap)[Input::eKeyboard2][p_eAction]);
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 3)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce((*m_pInputMap)[Input::eKeyboard3][p_eAction]);
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 4)
		{
			return HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce((*m_pInputMap)[Input::eKeyboard4][p_eAction]);
		}
		else
		{
			if (p_eAction == Racer::eSlide)
			{
				// Sliding is controlled by button or by the bumpers
				bool bSlide = HeatStroke::JoystickInputBuffer::Instance()->IsButtonDownOnce(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eSlide]);
				return (bSlide == true ? true : static_cast<float>(HeatStroke::JoystickInputBuffer::Instance()->GetAxis(p_iGLFWJoystick, XBOX_AXIS_TRIGGERS)) != 0.0f);
			}
			else
			{
				return HeatStroke::JoystickInputBuffer::Instance()->IsButtonDownOnce(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][p_eAction]);
			}
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::GetTurning
	// Parameter:	const int p_iGLFWJoystick - Joystick number, or GLFW_JOYSTICK_LAST + 1 for keyboard
	//
	// Queries for the individual turning value of the input.
	//--------------------------------------------------------------------------------
	float InputActionMapping::GetTurning(const int p_iGLFWJoystick)
	{
		float fTurn = 0.0f;
		if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 1)
		{
			fTurn = static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard1][Racer::eRight]));
			fTurn -= static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard1][Racer::eLeft]));
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 2)
		{
			fTurn = static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard2][Racer::eRight]));
			fTurn -= static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard2][Racer::eLeft]));
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 3)
		{
			fTurn = static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard3][Racer::eRight]));
			fTurn -= static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard3][Racer::eLeft]));
		}
		else if (p_iGLFWJoystick == GLFW_JOYSTICK_LAST + 4)
		{
			fTurn = static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard4][Racer::eRight]));
			fTurn -= static_cast<float>(HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownContinuous((*m_pInputMap)[Input::eKeyboard4][Racer::eLeft]));
		}
		else
		{
			fTurn = HeatStroke::JoystickInputBuffer::Instance()->GetAxis(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eAnalogStick]);
			if (fTurn == 0.0f)
			{
				fTurn = static_cast<float>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDownContinuous(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eRight]));
				fTurn -= static_cast<float>(HeatStroke::JoystickInputBuffer::Instance()->IsButtonDownContinuous(p_iGLFWJoystick, (*m_pInputMap)[Input::eJoystick][Racer::eLeft]));
			}
		}
		return fTurn;
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
			(*m_pInputMap)[Input::eKeyboard1][Racer::eAccelerate] =		GLFW_KEY_UP;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eBrake] =			GLFW_KEY_DOWN;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eLeft] =			GLFW_KEY_LEFT;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eRight] =			GLFW_KEY_RIGHT;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eSlide] =			GLFW_KEY_SPACE;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eDriverAbility1] =	GLFW_KEY_A;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eDriverAbility2] =	GLFW_KEY_S;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eKartAbility1] =	GLFW_KEY_D;
			(*m_pInputMap)[Input::eKeyboard1][Racer::eKartAbility2] =	GLFW_KEY_F;
			(*m_pInputMap)[Input::eKeyboard1][Racer::ePause] =			GLFW_KEY_ENTER;

			(*m_pInputMap)[Input::eKeyboard2][Racer::eAccelerate] = GLFW_KEY_UP;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eBrake] = GLFW_KEY_DOWN;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eLeft] = GLFW_KEY_LEFT;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eRight] = GLFW_KEY_RIGHT;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eSlide] = GLFW_KEY_SPACE;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eDriverAbility1] = GLFW_KEY_A;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eDriverAbility2] = GLFW_KEY_S;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eKartAbility1] = GLFW_KEY_D;
			(*m_pInputMap)[Input::eKeyboard2][Racer::eKartAbility2] = GLFW_KEY_F;
			(*m_pInputMap)[Input::eKeyboard2][Racer::ePause] = GLFW_KEY_ENTER;

			(*m_pInputMap)[Input::eKeyboard3][Racer::eAccelerate] = GLFW_KEY_UP;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eBrake] = GLFW_KEY_DOWN;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eLeft] = GLFW_KEY_LEFT;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eRight] = GLFW_KEY_RIGHT;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eSlide] = GLFW_KEY_SPACE;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eDriverAbility1] = GLFW_KEY_A;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eDriverAbility2] = GLFW_KEY_S;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eKartAbility1] = GLFW_KEY_D;
			(*m_pInputMap)[Input::eKeyboard3][Racer::eKartAbility2] = GLFW_KEY_F;
			(*m_pInputMap)[Input::eKeyboard3][Racer::ePause] = GLFW_KEY_ENTER;

			(*m_pInputMap)[Input::eKeyboard4][Racer::eAccelerate] = GLFW_KEY_UP;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eBrake] = GLFW_KEY_DOWN;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eLeft] = GLFW_KEY_LEFT;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eRight] = GLFW_KEY_RIGHT;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eSlide] = GLFW_KEY_SPACE;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eDriverAbility1] = GLFW_KEY_A;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eDriverAbility2] = GLFW_KEY_S;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eKartAbility1] = GLFW_KEY_D;
			(*m_pInputMap)[Input::eKeyboard4][Racer::eKartAbility2] = GLFW_KEY_F;
			(*m_pInputMap)[Input::eKeyboard4][Racer::ePause] = GLFW_KEY_ENTER;
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
				case Input::eKeyboard1: strInputName = "Keyboard1"; iActionsToMap = 10; break;
				case Input::eKeyboard2: strInputName = "Keyboard2"; iActionsToMap = 10; break;
				case Input::eKeyboard3: strInputName = "Keyboard3"; iActionsToMap = 10; break;
				case Input::eKeyboard4: strInputName = "Keyboard4"; iActionsToMap = 10; break;
				case Input::eJoystick: strInputName = "Joystick"; iActionsToMap = 11; break;
			}

			// Make sure the attribute type is found in the XML document
			tinyxml2::XMLElement* pInputType = static_cast<tinyxml2::XMLElement*>(HeatStroke::EasyXML::FindChildNode(pElement, strInputName.c_str()));
			if (pInputType == nullptr)
			{
#ifdef _DEBUG
				assert(false && "ControlBindings.xml does not contain input type");
#endif
				ResetUserControlBindings(it->first == Input::eKeyboard1, it->first == Input::eJoystick);
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
				if ((it->first == Input::eKeyboard1 ||
					it->first == Input::eKeyboard2 ||
					it->first == Input::eKeyboard3 ||
					it->first == Input::eKeyboard4)
					&& !HeatStroke::KeyboardInputBuffer::Instance()->IsValidKey(iButton))
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
				ResetUserControlBindings(it->first == Input::eKeyboard1, it->first == Input::eJoystick);
			}
			else
			{
				(*m_pInputMap)[it->first] = std::move(mActionMap);
			}
		}
	}

	//--------------------------------------------------------------------------------
	// InputActionMapping::GetButtonMapping
	//
	// Return mapped GLFW_KEY or XBOX constant, or -1 if not found.
	//--------------------------------------------------------------------------------
	int InputActionMapping::GetButtonMapping(Input::Type eType, Racer::Action eAction)
	{
		auto find = m_pInputMap->find(eType);
		if (find != m_pInputMap->end())
		{
			auto find2 = find->second.find(eAction);
			if (find2 != find->second.end())
			{
				return find2->second;
			}
		}
		return -1;
	}
}