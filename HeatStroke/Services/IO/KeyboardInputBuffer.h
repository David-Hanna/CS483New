//====================================================================================
// KeyboardInputBuffer
// Author: David Hanna
//
// A resource handler for keyboard input.
//====================================================================================

#ifndef KEYBOARD_INPUT_BUFFER_H
#define KEYBOARD_INPUT_BUFFER_H

#include <GLFW/glfw3.h>
#include <map>

namespace HeatStroke
{
	class KeyboardInputBuffer
	{
	private:
		// Convenient typedef
		typedef std::map<int, bool> KeyMap;

	public:
		//-------------------------------------------------------------
		// Public methods
		//-------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static KeyboardInputBuffer* Instance();

		// Game Loop Method
		void Update(const float p_fDelta);

		// Returns true if the given key is down. False if not, or
		// if the key is not supported.
		bool IsKeyDown(const int p_iGLFWKeyConstant) const;

	private:
		//-------------------------------------------------------------
		// Private members
		//-------------------------------------------------------------
		// Static singleton instance
		static KeyboardInputBuffer* s_pKeyboardInputBufferInstance;

		// Window with which to query keyboard input.
		GLFWwindow* m_pWindow;

		// Key map
		KeyMap* m_pKeysDown;
		KeyMap* m_pKeysDownLast;
		KeyMap* m_pTemp;

		//-------------------------------------------------------------
		// Private methods
		//-------------------------------------------------------------
		KeyboardInputBuffer(GLFWwindow* p_pWindow);
		KeyboardInputBuffer(const KeyboardInputBuffer&) = delete;
		KeyboardInputBuffer& operator=(const KeyboardInputBuffer&) = delete;
		~KeyboardInputBuffer();

		void Init();
	};
}

#endif // KEYBOARD_INPUT_BUFFER_H