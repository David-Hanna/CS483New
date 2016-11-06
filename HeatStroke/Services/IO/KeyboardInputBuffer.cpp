//====================================================================================
// KeyboardInputBuffer
// Author: David Hanna
//
// A resource handler for keyboard input.
//====================================================================================

#include "KeyboardInputBuffer.h"

#include <cassert>

namespace HeatStroke
{
	// Static singleton instance
	KeyboardInputBuffer* KeyboardInputBuffer::s_pKeyboardInputBufferInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Returns:   void
	// 
	// Creates the singleton instance.
	//------------------------------------------------------------------------------
	void KeyboardInputBuffer::CreateInstance(GLFWwindow* p_pWindow)
	{
		assert(s_pKeyboardInputBufferInstance == nullptr);
		s_pKeyboardInputBufferInstance = new KeyboardInputBuffer(p_pWindow);
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void KeyboardInputBuffer::DestroyInstance()
	{
		assert(s_pKeyboardInputBufferInstance != nullptr);
		delete s_pKeyboardInputBufferInstance;
		s_pKeyboardInputBufferInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   KeyboardInputBuffer::KeyboardInputBuffer*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	KeyboardInputBuffer* KeyboardInputBuffer::Instance()
	{
		assert(s_pKeyboardInputBufferInstance != nullptr);
		return s_pKeyboardInputBufferInstance;
	}

	//--------------------------------------------------------------------------
	// KeyboardInputBuffer::KeyboardInputBuffer
	// Parameters: GLFWwindow* p_pWindow - the window to check for key presses.
	//
	// Basic initialization.
	//--------------------------------------------------------------------------
	KeyboardInputBuffer::KeyboardInputBuffer(GLFWwindow* p_pWindow)
		:
		m_pWindow(p_pWindow),
		m_pKeysDown(new KeyMap),
		m_pKeysDownLast(new KeyMap),
		m_pTemp(nullptr)
	{
		Init();
	}

	//--------------------------------------------------------------------------
	// KeyBoardInputBuffer::~KeyboardInputBuffer
	//
	// Clear, delete, and set the maps to nullptr.
	//--------------------------------------------------------------------------
	KeyboardInputBuffer::~KeyboardInputBuffer()
	{
		m_pKeysDown->clear();
		delete m_pKeysDown;
		m_pKeysDown = nullptr;

		m_pKeysDownLast->clear();
		delete m_pKeysDownLast;
		m_pKeysDownLast = nullptr;
	}

	//--------------------------------------------------------------------------
	// KeyboardInputBuffer::Init
	//
	// Initializes this KeyboardInputBuffer with the map data needed to run.
	//--------------------------------------------------------------------------
	void KeyboardInputBuffer::Init()
	{
		//----------------------------------------------------------------------
		// Only insert the keys defined by GLFW.
		// We could do a loop... but many keyboard codes are unused and would
		// be a waste of memory. Also, it is a convenient reference to see
		// what keys are available to query here.
		//----------------------------------------------------------------------
		(*m_pKeysDown)[GLFW_KEY_UNKNOWN] = false;
		(*m_pKeysDown)[GLFW_KEY_SPACE] = false;
		(*m_pKeysDown)[GLFW_KEY_APOSTROPHE] = false;
		(*m_pKeysDown)[GLFW_KEY_COMMA] = false;
		(*m_pKeysDown)[GLFW_KEY_MINUS] = false;
		(*m_pKeysDown)[GLFW_KEY_PERIOD] = false;
		(*m_pKeysDown)[GLFW_KEY_SLASH] = false;
		(*m_pKeysDown)[GLFW_KEY_0] = false;
		(*m_pKeysDown)[GLFW_KEY_1] = false;
		(*m_pKeysDown)[GLFW_KEY_2] = false;
		(*m_pKeysDown)[GLFW_KEY_3] = false;
		(*m_pKeysDown)[GLFW_KEY_4] = false;
		(*m_pKeysDown)[GLFW_KEY_5] = false;
		(*m_pKeysDown)[GLFW_KEY_6] = false;
		(*m_pKeysDown)[GLFW_KEY_7] = false;
		(*m_pKeysDown)[GLFW_KEY_8] = false;
		(*m_pKeysDown)[GLFW_KEY_9] = false;
		(*m_pKeysDown)[GLFW_KEY_SEMICOLON] = false;
		(*m_pKeysDown)[GLFW_KEY_EQUAL] = false;
		(*m_pKeysDown)[GLFW_KEY_A] = false;
		(*m_pKeysDown)[GLFW_KEY_B] = false;
		(*m_pKeysDown)[GLFW_KEY_C] = false;
		(*m_pKeysDown)[GLFW_KEY_D] = false;
		(*m_pKeysDown)[GLFW_KEY_E] = false;
		(*m_pKeysDown)[GLFW_KEY_F] = false;
		(*m_pKeysDown)[GLFW_KEY_G] = false;
		(*m_pKeysDown)[GLFW_KEY_H] = false;
		(*m_pKeysDown)[GLFW_KEY_I] = false;
		(*m_pKeysDown)[GLFW_KEY_J] = false;
		(*m_pKeysDown)[GLFW_KEY_K] = false;
		(*m_pKeysDown)[GLFW_KEY_L] = false;
		(*m_pKeysDown)[GLFW_KEY_M] = false;
		(*m_pKeysDown)[GLFW_KEY_N] = false;
		(*m_pKeysDown)[GLFW_KEY_O] = false;
		(*m_pKeysDown)[GLFW_KEY_P] = false;
		(*m_pKeysDown)[GLFW_KEY_Q] = false;
		(*m_pKeysDown)[GLFW_KEY_R] = false;
		(*m_pKeysDown)[GLFW_KEY_S] = false;
		(*m_pKeysDown)[GLFW_KEY_T] = false;
		(*m_pKeysDown)[GLFW_KEY_U] = false;
		(*m_pKeysDown)[GLFW_KEY_V] = false;
		(*m_pKeysDown)[GLFW_KEY_W] = false;
		(*m_pKeysDown)[GLFW_KEY_X] = false;
		(*m_pKeysDown)[GLFW_KEY_Y] = false;
		(*m_pKeysDown)[GLFW_KEY_Z] = false;
		(*m_pKeysDown)[GLFW_KEY_LEFT_BRACKET] = false;
		(*m_pKeysDown)[GLFW_KEY_BACKSLASH] = false;
		(*m_pKeysDown)[GLFW_KEY_RIGHT_BRACKET] = false;
		(*m_pKeysDown)[GLFW_KEY_GRAVE_ACCENT] = false;
		(*m_pKeysDown)[GLFW_KEY_WORLD_1] = false;
		(*m_pKeysDown)[GLFW_KEY_WORLD_2] = false;
		(*m_pKeysDown)[GLFW_KEY_ESCAPE] = false;
		(*m_pKeysDown)[GLFW_KEY_ENTER] = false;
		(*m_pKeysDown)[GLFW_KEY_TAB] = false;
		(*m_pKeysDown)[GLFW_KEY_BACKSPACE] = false;
		(*m_pKeysDown)[GLFW_KEY_INSERT] = false;
		(*m_pKeysDown)[GLFW_KEY_DELETE] = false;
		(*m_pKeysDown)[GLFW_KEY_RIGHT] = false;
		(*m_pKeysDown)[GLFW_KEY_LEFT] = false;
		(*m_pKeysDown)[GLFW_KEY_DOWN] = false;
		(*m_pKeysDown)[GLFW_KEY_UP] = false;
		(*m_pKeysDown)[GLFW_KEY_PAGE_UP] = false;
		(*m_pKeysDown)[GLFW_KEY_PAGE_DOWN] = false;
		(*m_pKeysDown)[GLFW_KEY_HOME] = false;
		(*m_pKeysDown)[GLFW_KEY_END] = false;
		(*m_pKeysDown)[GLFW_KEY_CAPS_LOCK] = false;
		(*m_pKeysDown)[GLFW_KEY_SCROLL_LOCK] = false;
		(*m_pKeysDown)[GLFW_KEY_NUM_LOCK] = false;
		(*m_pKeysDown)[GLFW_KEY_PRINT_SCREEN] = false;
		(*m_pKeysDown)[GLFW_KEY_PAUSE] = false;
		(*m_pKeysDown)[GLFW_KEY_F1] = false;
		(*m_pKeysDown)[GLFW_KEY_F2] = false;
		(*m_pKeysDown)[GLFW_KEY_F3] = false;
		(*m_pKeysDown)[GLFW_KEY_F4] = false;
		(*m_pKeysDown)[GLFW_KEY_F5] = false;
		(*m_pKeysDown)[GLFW_KEY_F6] = false;
		(*m_pKeysDown)[GLFW_KEY_F7] = false;
		(*m_pKeysDown)[GLFW_KEY_F8] = false;
		(*m_pKeysDown)[GLFW_KEY_F9] = false;
		(*m_pKeysDown)[GLFW_KEY_F10] = false;
		(*m_pKeysDown)[GLFW_KEY_F11] = false;
		(*m_pKeysDown)[GLFW_KEY_F12] = false;
		(*m_pKeysDown)[GLFW_KEY_F13] = false;
		(*m_pKeysDown)[GLFW_KEY_F14] = false;
		(*m_pKeysDown)[GLFW_KEY_F15] = false;
		(*m_pKeysDown)[GLFW_KEY_F16] = false;
		(*m_pKeysDown)[GLFW_KEY_F17] = false;
		(*m_pKeysDown)[GLFW_KEY_F18] = false;
		(*m_pKeysDown)[GLFW_KEY_F19] = false;
		(*m_pKeysDown)[GLFW_KEY_F20] = false;
		(*m_pKeysDown)[GLFW_KEY_F21] = false;
		(*m_pKeysDown)[GLFW_KEY_F22] = false;
		(*m_pKeysDown)[GLFW_KEY_F23] = false;
		(*m_pKeysDown)[GLFW_KEY_F24] = false;
		(*m_pKeysDown)[GLFW_KEY_F25] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_0] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_1] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_2] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_3] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_4] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_5] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_6] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_7] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_8] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_9] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_DECIMAL] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_DIVIDE] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_MULTIPLY] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_SUBTRACT] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_ADD] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_ENTER] = false;
		(*m_pKeysDown)[GLFW_KEY_KP_EQUAL] = false;
		(*m_pKeysDown)[GLFW_KEY_LEFT_SHIFT] = false;
		(*m_pKeysDown)[GLFW_KEY_LEFT_CONTROL] = false;
		(*m_pKeysDown)[GLFW_KEY_LEFT_ALT] = false;
		(*m_pKeysDown)[GLFW_KEY_LEFT_SUPER] = false;
		(*m_pKeysDown)[GLFW_KEY_RIGHT_SHIFT] = false;
		(*m_pKeysDown)[GLFW_KEY_RIGHT_CONTROL] = false;
		(*m_pKeysDown)[GLFW_KEY_RIGHT_ALT] = false;
		(*m_pKeysDown)[GLFW_KEY_RIGHT_SUPER] = false;
		(*m_pKeysDown)[GLFW_KEY_MENU] = false;
	}

	//--------------------------------------------------------------------------------
	// KeyboardInputBuffer::Update
	// Parameter: float fDelta
	//
	// Updates the state of the keys.
	//--------------------------------------------------------------------------------
	void KeyboardInputBuffer::Update(const float p_fDelta)
	{
		// Instead of copying all the data from KeysDown to KeysDownLast,
		// we do a pointer swap.
		// First, query all the keys and set the values to KeysDownLast.
		KeyMap::iterator it = m_pKeysDown->begin(), end = m_pKeysDown->end();
		for (; it != end; it++)
		{
			(*m_pKeysDownLast)[it->first] = (glfwGetKey(m_pWindow, it->first) == GLFW_PRESS);
		}
		// Then swap the pointers, so now KeysDownLast is pointing to the values from
		// last frame, and KeysDown is pointing to the values just assigned.
		m_pTemp = m_pKeysDownLast;
		m_pKeysDownLast = m_pKeysDown;
		m_pKeysDown = m_pTemp;
	}

	//--------------------------------------------------------------------------------
	// KeyboardInputBuffer::IsValidKey
	// Parameter: const int p_iGLFWKeyConstant - the key to check, should be a glfw key constant.
	//
	// Returns true if the given key is tracked by GLFW.
	//--------------------------------------------------------------------------------
	bool KeyboardInputBuffer::IsValidKey(const int p_iGLFWKeyConstant) const
	{
		return m_pKeysDown->find(p_iGLFWKeyConstant) != m_pKeysDown->end();
	}

	//--------------------------------------------------------------------------------
	// KeyboardInputBuffer::IsKeyDownOnce
	// Parameter: const int p_iGLFWKeyConstant - the key to check, should be a glfw key constant.
	//
	// Returns true if the given key is down this frame but was
	// not last frame. False if not, or if the key is not supported.
	//--------------------------------------------------------------------------------
	bool KeyboardInputBuffer::IsKeyDownOnce(const int p_iGLFWKeyConstant) const
	{
		KeyMap::iterator findCurr = m_pKeysDown->find(p_iGLFWKeyConstant);
		KeyMap::iterator findPrev = m_pKeysDownLast->find(p_iGLFWKeyConstant);
		if (findCurr != m_pKeysDown->end() &&
			findPrev != m_pKeysDownLast->end())
		{
			return (findCurr->second && !findPrev->second);
		}

		return false;
	}

	//--------------------------------------------------------------------------------
	// KeyboardInputBuffer::IsKeyDownContinuous
	// Parameter: const int p_iGLFWKeyConstant - the key to check, should be a glfw key constant.
	//
	// Returns true if the given key is down. False if not, or
	// if the key is not supported.
	//--------------------------------------------------------------------------------
	bool KeyboardInputBuffer::IsKeyDownContinuous(const int p_iGLFWKeyConstant) const
	{
		KeyMap::iterator find = m_pKeysDown->find(p_iGLFWKeyConstant);
		if (find != m_pKeysDown->end())
		{
			return find->second;
		}

		// TO DO, this should be a soft assert. (can keep going after assertion)
		assert(false && "Unsupported key requested.");
		return false;
	}
}