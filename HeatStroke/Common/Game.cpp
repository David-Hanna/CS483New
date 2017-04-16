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

#include "Game.h"

using namespace HeatStroke;

//------------------------------------------------------------------------------
// Method:    Game
// Parameter: void
// Returns:   
// 
// Constructor
//------------------------------------------------------------------------------
Game::Game()
	:
	m_fFrameTime(static_cast<float>(glfwGetTime()))
{
}

//------------------------------------------------------------------------------
// Method:    ~Game
// Parameter: void
// Returns:   
// 
// Destructor.
//------------------------------------------------------------------------------
Game::~Game()
{
}

//------------------------------------------------------------------------------
// Method:    Run
// Returns:   int
// 
// Starts (and runs) the main game loop. Does not return until application
// shuts down.
//------------------------------------------------------------------------------
int Game::Run(const char* p_strAppName, glm::vec4 p_vDefaultColor, int p_iWindowWidth, int p_iWindowHeight)
{
	float t;

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_pWindow = glfwCreateWindow(p_iWindowWidth, p_iWindowHeight, p_strAppName, nullptr, nullptr);

	// Open a window and create its OpenGL context
	if (m_pWindow == nullptr)
	{
		fprintf( stderr, "Failed to open GLFW window\n" );
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(m_pWindow);

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		printf("GlewInit error: %d\n", err);
		return EXIT_FAILURE;
	}
#endif

	// Leave this here! On windows machines in Lab, glewInit is generating some mysterious, but apparently
	// harmless, error - and we need it gone.
	glGetError();

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, 1);

	// Enable vertical sync (on cards that support it)
	glfwSwapInterval(1);

	// Call Init() on game subclass.
	bool bContinue = this->Init();

	while (bContinue && glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(m_pWindow))
	{
		t = static_cast<float>(glfwGetTime());

		// Clear color buffer to default color
		glClearColor(p_vDefaultColor.r, p_vDefaultColor.g, p_vDefaultColor.b, p_vDefaultColor.a);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		float fDelta = (float)t - m_fFrameTime;

		if (fDelta < 1.0f)
		{
			// Update and render game
			this->Update((float)t - m_fFrameTime);
			this->PreRender();
			this->Render();
		}

		// Poll user input events
		glfwPollEvents();

		// Swap buffers
		glfwSwapBuffers(m_pWindow);

		m_fFrameTime = t;
	}

	// Shutdown app
	this->Shutdown();

	glfwDestroyWindow(m_pWindow);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	exit( EXIT_SUCCESS );
}
