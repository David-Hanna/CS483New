//====================================================================================
// JoystickInputBuffer
// Author: Bradley Cooper
//
// A resource handler for joystick (gamepad) input.
//====================================================================================

#ifndef JOYSTICK_INPUT_BUFFER_H
#define JOYSTICK_INPUT_BUFFER_H

#include <GLFW/glfw3.h>
#include <map>
#include "InputActions.h"
#include "EventManager.h"

namespace HeatStroke
{
	class JoystickInputBuffer
	{
	private:
		// Convenient typedefs and struct
		typedef std::map<int, bool> JoystickButtonMap;
		typedef std::map<int, float> JoystickAxesMap;

		struct Joystick
		{
			bool m_bConnected;
			int	m_iButtonsCount;
			int m_iAxesCount;

			JoystickButtonMap	m_mButtonsDown;
			JoystickAxesMap		m_mAxes;

			Joystick() : 
				m_bConnected(false),
				m_iButtonsCount(0), 
				m_iAxesCount(0), 
				m_mButtonsDown(JoystickButtonMap()),
				m_mAxes(JoystickAxesMap()) {}
		};

		typedef std::map<int, Joystick> JoystickMap;

	public:
		//-------------------------------------------------------------
		// Public methods
		//-------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static JoystickInputBuffer* Instance();

		// Game Loop Method
		void Update(const float p_fDelta);

		// Returns true if the given button is down. False if not, or
		// if the key is not supported.
		bool IsButtonDown(const int p_iGLFWJoystick, const char p_iGLFWJoystickButton) const;

		// Sets parameters to axes tilt if they exceed the dead zone,
		// otherwise set to 0. Positive indicates right and up.
		float GetAxis(const int p_iGLFWJoystick, const int p_iAxis) const;

	private:
		//-------------------------------------------------------------
		// Private members
		//-------------------------------------------------------------
		// Static singleton instance
		static JoystickInputBuffer* s_pJoystickInputBufferInstance;

		// Window with which to query input.
		GLFWwindow* m_pWindow;

		// Joystick maps
		JoystickMap* m_pJoysticks;
		JoystickMap* m_pJoysticksLast;
		JoystickMap* m_pTemp;

		//-------------------------------------------------------------
		// Private methods
		//-------------------------------------------------------------
		JoystickInputBuffer(GLFWwindow* p_pWindow);
		JoystickInputBuffer(const JoystickInputBuffer&) = delete;
		JoystickInputBuffer& operator=(const JoystickInputBuffer&) = delete;
		~JoystickInputBuffer();

		void Init();

		// Helper function
		Joystick* const GetJoystick(const int p_iGLFWJoystick) const;
	};
}

#endif // JOYSTICK_INPUT_BUFFER_H