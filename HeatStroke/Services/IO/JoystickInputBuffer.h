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

namespace HeatStroke
{
	class JoystickInputBuffer
	{
	private:
		// Convenient typedefs
		typedef std::map<int, bool> JoystickButtonMap;
		typedef std::map<int, float> JoystickAxesMap;

		typedef struct Joystick
		{
			int	m_iButtonsCount;
			int m_iAxesCount;

			JoystickButtonMap*	m_pButtonsDown;
			JoystickAxesMap*	m_pAxes;
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

		// Window with which to query keyboard input.
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

		// Joystick connect/disconnect callback
		// void JoystickCallback(int joy, int event);
	};
}

#endif // JOYSTICK_INPUT_BUFFER_H