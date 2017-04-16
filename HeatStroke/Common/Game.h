//------------------------------------------------------------------------
// Game
//
// Created:	2012/12/06
// Author:	Carel Boers
//	
// This class serves as a base Game class. Derive from this class and
// override the pure virtuals to setup the Init/Update/Render hooks for
// the main game logic.
//------------------------------------------------------------------------

#ifndef GAME_H
#define GAME_H

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU

#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "Common.h"

namespace HeatStroke
{
	class Game
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		Game();
		virtual ~Game();

		int Run(const char* p_strAppName = "HeatStroke::Game",
				glm::vec4 p_vDefaultColor = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f),
				int p_iWindowWidth = 1280,
				int p_iWindowHeight = 720);

	protected:
		//------------------------------------------------------------------------------
		// Protected members.
		//------------------------------------------------------------------------------
		GLFWwindow* m_pWindow;

		//------------------------------------------------------------------------------
		// Protected methods.
		//------------------------------------------------------------------------------
		virtual bool Init() = 0;
		virtual void Update(const float p_fDelta) = 0;
		virtual void PreRender() = 0;
		virtual void Render() = 0;
		virtual void Shutdown() = 0;

	private:
		//------------------------------------------------------------------------------
		// Private members.
		//------------------------------------------------------------------------------
		float m_fFrameTime;
	};

} // namespace HeatStroke

#endif // GAME_H
