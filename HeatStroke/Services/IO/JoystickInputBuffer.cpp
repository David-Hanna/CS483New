//====================================================================================
// JoystickInputBuffer
// Author: Bradley Cooper
//
// A resource handler for joystick (gamepad) input.
//====================================================================================

#include "JoystickInputBuffer.h"

#include <cassert>

namespace HeatStroke
{
	// Static singleton instance
	JoystickInputBuffer* JoystickInputBuffer::s_pJoystickInputBufferInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Returns:   void
	// 
	// Creates the singleton instance.
	//------------------------------------------------------------------------------
	void JoystickInputBuffer::CreateInstance(GLFWwindow* p_pWindow)
	{
		assert(s_pJoystickInputBufferInstance == nullptr);
		s_pJoystickInputBufferInstance = new JoystickInputBuffer(p_pWindow);
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void JoystickInputBuffer::DestroyInstance()
	{
		assert(s_pJoystickInputBufferInstance != nullptr);
		delete s_pJoystickInputBufferInstance;
		s_pJoystickInputBufferInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   JoystickInputBuffer::JoystickInputBuffer*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	JoystickInputBuffer* JoystickInputBuffer::Instance()
	{
		assert(s_pJoystickInputBufferInstance != nullptr);
		return s_pJoystickInputBufferInstance;
	}

	//--------------------------------------------------------------------------
	// JoystickInputBuffer::JoystickInputBuffer
	// Parameters: GLFWwindow* p_pWindow - the window to check for key presses.
	//
	// Basic initialization.
	//--------------------------------------------------------------------------
	JoystickInputBuffer::JoystickInputBuffer(GLFWwindow* p_pWindow)
		:
		m_pWindow(p_pWindow),
		m_pJoysticks(new JoystickMap),
		m_pJoysticksLast(new JoystickMap),
		m_pTemp(nullptr)
	{
		Init();
	}

	//--------------------------------------------------------------------------
	// JoystickInputBuffer::~JoystickInputBuffer
	//
	// Clear, delete, and set the maps to nullptr.
	//--------------------------------------------------------------------------
	JoystickInputBuffer::~JoystickInputBuffer()
	{
		for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			m_pJoysticks->at(i).m_mAxes.clear();
			m_pJoysticks->at(i).m_mButtonsDown.clear();
		}
		delete m_pJoysticks;
		m_pJoysticks = nullptr;

		for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			m_pJoysticksLast->at(i).m_mAxes.clear();
			m_pJoysticksLast->at(i).m_mButtonsDown.clear();
		}
		delete m_pJoysticksLast;
		m_pJoysticksLast = nullptr;
	}

	//--------------------------------------------------------------------------
	// JoystickInputBuffer::Init
	//
	// Initializes this JoystickInputBuffer with the map data needed to run.
	//--------------------------------------------------------------------------
	void JoystickInputBuffer::Init()
	{
		// Iterate all joysticks and check to see if they are connected
		for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++)
		{
			(*m_pJoysticks)[i] = Joystick();
			(*m_pJoysticksLast)[i] = Joystick();
			m_pJoysticks->at(i).m_bConnected = (glfwJoystickPresent(i) == GLFW_CONNECTED);
		}
	}

	//--------------------------------------------------------------------------------
	// JoystickInputBuffer::Update
	// Parameter: float fDelta
	//
	// Updates the state of the keys.
	//--------------------------------------------------------------------------------
	void JoystickInputBuffer::Update(const float p_fDelta)
	{
		// Instead of copying all the data from Joysticks to JoysticksLast, we do a pointer swap.
		// First, query all the joystick axes and buttons, and set the values to JoysticksLast.
		for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++)
		{
			if (glfwJoystickPresent(i))
			{
				// If this controller was not connected the previous frame: send reconnect event
				if (!m_pJoysticks->at(i).m_bConnected)
				{
					m_pJoysticks->at(i).m_bConnected = true;
					Event* pEvent = new Event("JoystickCallback");
					pEvent->SetIntParameter("Joystick", i);
					pEvent->SetIntParameter("GLFWEvent", GLFW_CONNECTED);
					EventManager::Instance()->QueueEvent(pEvent);
				}

				const float* pAxes = glfwGetJoystickAxes(i, &m_pJoysticksLast->at(i).m_iAxesCount);
				for (int j = 0; j < m_pJoysticksLast->at(i).m_iAxesCount; j++)
				{
					(*m_pJoysticksLast)[i].m_mAxes[j] = pAxes[j];
				}

				const unsigned char* pButtons = glfwGetJoystickButtons(i, &m_pJoysticksLast->at(i).m_iButtonsCount);
				for (int j = 0; j < m_pJoysticksLast->at(i).m_iButtonsCount; j++)
				{
					(*m_pJoysticksLast)[i].m_mButtonsDown[j] = (pButtons[j] == GLFW_PRESS);
				}
			}
			else if (m_pJoysticks->at(i).m_bConnected)
			{
				// If this controller was connected the previous frame: send disconnect event
				m_pJoysticks->at(i).m_bConnected = false;
				Event* pEvent = new Event("JoystickCallback");
				pEvent->SetIntParameter("Joystick", i);
				pEvent->SetIntParameter("GLFWEvent", GLFW_DISCONNECTED);
				EventManager::Instance()->QueueEvent(pEvent);
			}
		}

		// Then swap the pointers, so now JoysticksLast is pointing to the values from
		// last frame, and Joysticks is pointing to the values just assigned.
		m_pTemp = m_pJoysticksLast;
		m_pJoysticksLast = m_pJoysticks;
		m_pJoysticks = m_pTemp;
	}

	//--------------------------------------------------------------------------------
	// JoystickInputBuffer::IsButtonDownOnce
	// Parameter:	const int p_iGLFWJoystick - GLFW integer for joystick
	//				const int p_iGLFWButtonConstant - the key to check, should be a glfw key constant.
	//
	// Returns true if the given button is down this frame but was not last frame.
	//--------------------------------------------------------------------------------
	bool JoystickInputBuffer::IsButtonDownOnce(const int p_iGLFWJoystick, const char p_iGLFWJoystickButton) const
	{
		// Return false if no joystick or joystick has no buttons
		Joystick* pCurrJoystick = GetJoystick(p_iGLFWJoystick, false);
		if (pCurrJoystick == nullptr || pCurrJoystick->m_iButtonsCount == 0)
		{
			return false;
		}

		// Do the same for joystick from the last frame
		Joystick* pPrevJoystick = GetJoystick(p_iGLFWJoystick, true);
		if (pPrevJoystick == nullptr || pPrevJoystick->m_iButtonsCount == 0)
		{
			return false;
		}

		// Find button pressed value for current and previous frame
		JoystickButtonMap::iterator findCurr = pCurrJoystick->m_mButtonsDown.find(p_iGLFWJoystickButton);
		JoystickButtonMap::iterator findPrev = pPrevJoystick->m_mButtonsDown.find(p_iGLFWJoystickButton);
		if (findCurr != (*m_pJoysticks)[p_iGLFWJoystick].m_mButtonsDown.end() &&
			findPrev != (*m_pJoysticksLast)[p_iGLFWJoystick].m_mButtonsDown.end())
		{
			return (findCurr->second && !findPrev->second);
		}

		return false;
	}

	//--------------------------------------------------------------------------------
	// JoystickInputBuffer::IsButtonDownContinuous
	// Parameter:	const int p_iGLFWJoystick - GLFW integer for joystick
	//				const int p_iGLFWButtonConstant - the key to check, should be a glfw key constant.
	//
	// Returns true if the given button is down.
	//--------------------------------------------------------------------------------
	bool JoystickInputBuffer::IsButtonDownContinuous(const int p_iGLFWJoystick, const char p_iGLFWJoystickButton) const
	{
		// Return false if no joystick or joystick has no buttons
		Joystick* pJoystick = GetJoystick(p_iGLFWJoystick);
		if (pJoystick == nullptr || pJoystick->m_iButtonsCount == 0)
		{
			return false;
		}

		// Return the button result if found. Otherwise, return false.
		JoystickButtonMap::iterator find = pJoystick->m_mButtonsDown.find(p_iGLFWJoystickButton);
		if (find != (*m_pJoysticks)[p_iGLFWJoystick].m_mButtonsDown.end())
		{
			return find->second;
		}

		return false;
	}

	//--------------------------------------------------------------------------------
	// JoystickInputBuffer::GetAxis
	// Parameter:	const int p_iGLFWJoystick - GLFW integer for joystick
	//				const int p_iAxis - Axis number on joystick
	//
	// Sets parameters to axes tilt, or 0 if axis not found.
	// Positive indicates right or up.
	//--------------------------------------------------------------------------------
	float JoystickInputBuffer::GetAxis(const int p_iGLFWJoystick, const int p_iAxis) const
	{
		// Check controller for axis
		Joystick* pJoystick = GetJoystick(p_iGLFWJoystick);
		if (pJoystick == nullptr || pJoystick->m_iAxesCount == 0)
		{
			return 0.0f;
		}

		// See if axis is outside of dead zone
		float fAxis = pJoystick->m_mAxes.at(p_iAxis);
		if (fabsf(fAxis) < XBOX_AXIS_DEAD_ZONE)
		{
			return 0.0f;
		}

		return fAxis;
	}

	//--------------------------------------------------------------------------------
	// JoystickInputBuffer::GetJoystick
	// Parameter:	const int p_iGLFWJoystick - GLFW integer for joystick
	//				const bool p_bPreviousFrame - Get Joystick from previous frame. False by default.
	//
	// Returns reference to joystick in map, or nullptr if not found
	//--------------------------------------------------------------------------------
	JoystickInputBuffer::Joystick* const JoystickInputBuffer::GetJoystick(const int p_iGLFWJoystick, const bool p_bPreviousFrame) const
	{
		if (p_iGLFWJoystick < 0 || p_iGLFWJoystick > GLFW_JOYSTICK_LAST)
		{
			return nullptr;
		}

		// Get iteratore based on current frame Joystick or previous frame Joystick
		JoystickMap::iterator find = (p_bPreviousFrame ? 
			m_pJoysticksLast->find(p_iGLFWJoystick) : 
			m_pJoysticks->find(p_iGLFWJoystick));

		if (find != m_pJoysticks->end() && find != m_pJoysticksLast->end())
		{
			return &find->second;
		}
		return nullptr;
	}
}

