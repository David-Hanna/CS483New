//=====================================================================================
// MouseInputBuffer
// Author: David Hanna
//
// Provides mouse position differential each frame.
//=====================================================================================

#include "MouseInputBuffer.h"

namespace HeatStroke
{
	// Static singleton instance
	MouseInputBuffer* MouseInputBuffer::s_pMouseInputBufferInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Returns:   void
	// 
	// Creates the singletone instance.
	//------------------------------------------------------------------------------
	void MouseInputBuffer::CreateInstance(GLFWwindow* p_pWindow)
	{
		assert(s_pMouseInputBufferInstance == nullptr);
		s_pMouseInputBufferInstance = new MouseInputBuffer(p_pWindow);
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void MouseInputBuffer::DestroyInstance()
	{
		assert(s_pMouseInputBufferInstance != nullptr);
		delete s_pMouseInputBufferInstance;
		s_pMouseInputBufferInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   MouseInputBuffer::MouseInputBuffer*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	MouseInputBuffer* MouseInputBuffer::Instance()
	{
		assert(s_pMouseInputBufferInstance != nullptr);
		return s_pMouseInputBufferInstance;
	}

	//--------------------------------------------------------------------------
	// MouseInputBuffer::MouseInputBuffer
	// Parameters: GLFWwindow* p_pWindow - the window to check for key presses.
	//
	// Basic initialization.
	//--------------------------------------------------------------------------
	MouseInputBuffer::MouseInputBuffer(GLFWwindow* p_pWindow)
		:
		m_pWindow(p_pWindow)
	{
		glfwGetCursorPos(m_pWindow, &m_fPreviousMouseX, &m_fPreviousMouseY);
	}

	//--------------------------------------------------------------------------
	// MouseInputBuffer::Update
	//
	// Updates the mouse differential.
	//--------------------------------------------------------------------------
	void MouseInputBuffer::Update(const float p_fDelta)
	{
		double fCurrentMouseX, fCurrentMouseY;
		glfwGetCursorPos(m_pWindow, &fCurrentMouseX, &fCurrentMouseY);

		m_fDifferentialMouseX = fCurrentMouseX - m_fPreviousMouseX;
		m_fDifferentialMouseY = m_fPreviousMouseY - fCurrentMouseY;

		m_fPreviousMouseX = fCurrentMouseX;
		m_fPreviousMouseY = fCurrentMouseY;
	}

	bool MouseInputBuffer::IsLeftMouseButtonPressed() const
	{
		return GLFW_PRESS == glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_LEFT);
	}

	//--------------------------------------------------------------------------
	// MouseInputBuffer::GetMouseDifferential
	//
	// Returns the mouse differentials in the output parameters.
	//--------------------------------------------------------------------------
	void MouseInputBuffer::GetMouseDifferential(double& p_fMouseXOut, double& p_fMouseYOut) const
	{
		p_fMouseXOut = m_fDifferentialMouseX;
		p_fMouseYOut = m_fDifferentialMouseY;
	}
}