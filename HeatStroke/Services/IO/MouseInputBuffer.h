//=====================================================================================
// MouseInputBuffer
// Author: David Hanna
//
// Provides mouse position differential each frame.
//=====================================================================================

#ifndef MOUSE_INPUT_BUFFER_H
#define MOUSE_INPUT_BUFFER_H

#include <GLFW/glfw3.h>
#include <assert.h>

namespace HeatStroke
{
	class MouseInputBuffer
	{
	public:
		//----------------------------------------------------------------------------
		// Public Methods
		//----------------------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static MouseInputBuffer* Instance();

		// Game Loop Methods
		void Update(const float p_fDelta);

		bool IsLeftMouseButtonPressed() const;
		void GetMouseDifferential(double& p_fMouseXOut, double& p_fMouseYOut) const;

	private:
		//----------------------------------------------------------------------------
		// Private Variables
		//----------------------------------------------------------------------------
		// Static singleton instance
		static MouseInputBuffer* s_pMouseInputBufferInstance;

		// Window with which to query keyboard input.
		GLFWwindow* m_pWindow;

		// Mouse variables.
		double m_fPreviousMouseX;
		double m_fDifferentialMouseX;

		double m_fPreviousMouseY;
		double m_fDifferentialMouseY;

		//----------------------------------------------------------------------------
		// Private Methods
		//----------------------------------------------------------------------------
		MouseInputBuffer(GLFWwindow* p_pWindow);
		MouseInputBuffer(const MouseInputBuffer&) = delete;
		MouseInputBuffer& operator=(const MouseInputBuffer&) = delete;
		~MouseInputBuffer() {}
	};
}

#endif // MOUSE_INPUT_BUFFER_H