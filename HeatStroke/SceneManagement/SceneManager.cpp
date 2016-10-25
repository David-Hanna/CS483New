//------------------------------------------------------------------------
// SceneManager
//
// Created:	2012/12/23
// Author:	Carel Boers
//	
// A simple scene manager to manage our models and camera.
//------------------------------------------------------------------------

#include "SceneManager.h"

#include <algorithm>

using namespace HeatStroke;

// Static singleton instance
SceneManager* SceneManager::s_pSceneManagerInstance = nullptr;

//------------------------------------------------------------------------------
// Method:    CreateInstance
// Returns:   void
// 
// Creates the singletone instance.
//------------------------------------------------------------------------------
void SceneManager::CreateInstance(GLFWwindow* p_pWindow)
{
	assert(s_pSceneManagerInstance == nullptr);
	s_pSceneManagerInstance = new SceneManager(p_pWindow);
}

//------------------------------------------------------------------------------
// Method:    DestroyInstance
// Returns:   void
// 
// Destroys the singleton instance.
//------------------------------------------------------------------------------
void SceneManager::DestroyInstance()
{
	assert(s_pSceneManagerInstance != nullptr);
	delete s_pSceneManagerInstance;
	s_pSceneManagerInstance = nullptr;
}

//------------------------------------------------------------------------------
// Method:    Instance
// Returns:   SceneManager::SceneManager*
// 
// Access to singleton instance.
//------------------------------------------------------------------------------
SceneManager* SceneManager::Instance()
{
	assert(s_pSceneManagerInstance != nullptr);
	return s_pSceneManagerInstance;
}

//------------------------------------------------------------------------------
// Method:    SceneManager
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
SceneManager::SceneManager(GLFWwindow* p_pWindow)
:
m_pActiveCamera(nullptr),
m_bCKeyHeld(false),
m_pWindow(p_pWindow)
{
}

//------------------------------------------------------------------------------
// Method:    ~SceneManager
// Returns:   
// 
// Destructor.
//------------------------------------------------------------------------------
SceneManager::~SceneManager()
{
}

//------------------------------------------------------------------------------
// Method:    AddModel
// Parameter: HeatStroke::Model * p_pModel
// Returns:   void
// 
// Adds a model the scene manager.
//------------------------------------------------------------------------------
void SceneManager::AddModel(HeatStroke::Model* p_pModel)
{
	m_lModelList.push_back(p_pModel);
}

//------------------------------------------------------------------------------
// Method:    RemoveModel
// Parameter: HeatStroke::Model * p_pModel
// Returns:   void
// 
// Removes a model from the scene manager.
//------------------------------------------------------------------------------
void SceneManager::RemoveModel(HeatStroke::Model* p_pModel)
{
	ModelList::iterator it = std::find(m_lModelList.begin(), m_lModelList.end(), p_pModel);
	if (it != m_lModelList.end())
	{
		m_lModelList.erase(it);
	}	
}

//------------------------------------------------------------------------------
// Method:    Clear
// Returns:   void
// 
// Clears the list of models in the scene manager.
//------------------------------------------------------------------------------
void SceneManager::Clear()
{
	m_lModelList.clear();
}

//------------------------------------------------------------------------------
// Method:    AddSprite
// Parameter: HeatStroke::Sprite * p_pSprite
// Returns:   void
// 
// Adds the given sprite to the scene manager.
//------------------------------------------------------------------------------
void SceneManager::AddSprite(HeatStroke::Sprite* p_pSprite)
{
	m_lSpriteList.push_back(p_pSprite);
}

//------------------------------------------------------------------------------
// Method:    RemoveSprite
// Parameter: HeatStroke::Sprite * p_pSprite
// Returns:   void
// 
// Removes the given sprite from the scene manager.
//------------------------------------------------------------------------------
void SceneManager::RemoveSprite(HeatStroke::Sprite* p_pSprite)
{
	SpriteList::iterator it = std::find(m_lSpriteList.begin(), m_lSpriteList.end(), p_pSprite);
	if (it != m_lSpriteList.end())
	{
		m_lSpriteList.erase(it);
	}
}

//------------------------------------------------------------------------------
// Method:    ClearSprites
// Returns:   void
// 
// Clears the list of sprites in the scene manager.
//------------------------------------------------------------------------------
void SceneManager::ClearSprites()
{
	m_lSpriteList.clear();
}

//------------------------------------------------------------------------------
// Method:	AddLight
// Parameter: ComponentLight* p_pLight
// Returns: void
//
// Adds the given light to the current scene.
//------------------------------------------------------------------------------
void SceneManager::AddLight(ScenePointLight* p_pLight)
{
	m_lLightList.push_back(p_pLight);
}

//------------------------------------------------------------------------------
// Method:	RemoveLight
// Parameter: ComponentLight* p_pLight
// Returns: void
//
// Removes the given light from the current scene.
//------------------------------------------------------------------------------
void SceneManager::RemoveLight(ScenePointLight* p_pLight)
{
	LightList::iterator it = std::find(m_lLightList.begin(), m_lLightList.end(), p_pLight);
	if (it != m_lLightList.end())
	{
		m_lLightList.erase(it);
	}
}

//------------------------------------------------------------------------------
// Method:	ClearLights
// Returns: void
//
// Clears all the lights from the current scene.
//------------------------------------------------------------------------------
void SceneManager::ClearLights()
{
	m_lLightList.clear();
}

//------------------------------------------------------------------------------
// Method: AddCamera
// Parameter: ComponentCamera* p_pCamera
// Returns: void
//
// Adds the given camera to the scene. Only sets it to the active camera
// if there was previously no other camera.
//------------------------------------------------------------------------------
void SceneManager::AddCamera(SceneCamera* p_pCamera)
{
	m_lCameras.push_back(p_pCamera);

	if (m_pActiveCamera == nullptr)
	{
		m_pActiveCamera = p_pCamera;
	}
}

//-----------------------------------------------------------------------------
// Method: SetActiveCamera
// Parameter: ComponentCamera* p_pCamera
// Returns: void
//
// Sets the active camera to the given camera. If it was not already in
// the list of cameras held by the scene, it adds it.
//-----------------------------------------------------------------------------
void SceneManager::SetActiveCamera(SceneCamera* p_pCamera)
{
	CameraList::iterator it = std::find(m_lCameras.begin(), m_lCameras.end(), p_pCamera);

	if (it == m_lCameras.end())
	{
		m_lCameras.push_back(p_pCamera);
	}

	m_pActiveCamera = p_pCamera;
}

//------------------------------------------------------------------------------
// Method: GetActiveCamera
// Returns: ComponentCamera*
//
// Returns a pointer to the currently active camera.
//------------------------------------------------------------------------------
SceneCamera* SceneManager::GetActiveCamera()
{
	return m_pActiveCamera;
}

//------------------------------------------------------------------------------
// Method: RemoveCamera
// Parameter: ComponentCamera* p_pCamera
// Returns: void
//
// Removes the given camera from the scene. If it was the active camera,
// another camera is set to active if the list is not empty.
//------------------------------------------------------------------------------
void SceneManager::RemoveCamera(SceneCamera* p_pCamera)
{
	CameraList::iterator it = std::find(m_lCameras.begin(), m_lCameras.end(), p_pCamera);

	if (it != m_lCameras.end())
	{
		m_lCameras.erase(it);

		if (p_pCamera == m_pActiveCamera)
		{
			if (m_lCameras.empty())
			{
				m_pActiveCamera = nullptr;
			}
			else
			{
				m_pActiveCamera = m_lCameras[0];
			}
		}
	}
}

//------------------------------------------------------------------------------
// Method: ClearCameras
// Returns: void
//
// Clears the list of cameras in the Scene, including the active camera.
//------------------------------------------------------------------------------
void SceneManager::ClearCameras()
{
	m_pActiveCamera = nullptr;
	m_lCameras.clear();
}

//------------------------------------------------------------------------------
// Method:		Update
// Returns:		void
//
// Checks that the player pressed the 'C' key and switches between
// the two cameras if it is pressed.
//------------------------------------------------------------------------------
void SceneManager::Update(const float p_fDelta)
{
	// To switch the camera requires a currently active camera and at least two total cameras.
	if (m_pActiveCamera != nullptr && m_lCameras.size() > 1)
	{
		// Switch Cameras when the player pressed the C key.
		if (!m_bCKeyHeld && glfwGetKey(m_pWindow, GLFW_KEY_C) == GLFW_PRESS)
		{
			CameraList::const_iterator it = std::find(m_lCameras.begin(), m_lCameras.end(), m_pActiveCamera);
			// If we're at the last item in the list, go back to the beginning.
			if (it == m_lCameras.end() - 1)
			{
				m_pActiveCamera = m_lCameras[0];
			}
			// Otherwise go to the next camera.
			else
			{
				m_pActiveCamera = *(it + 1);
			}

			m_bCKeyHeld = true;
		}
		else if (m_bCKeyHeld && glfwGetKey(m_pWindow, GLFW_KEY_C) != GLFW_PRESS)
		{
			m_bCKeyHeld = false;
		}
	}
}

//------------------------------------------------------------------------------
// Method:    Render
// Returns:   void
// 
// Iterates the list of models, applies the camera params to the shader and 
// renders the model.
//------------------------------------------------------------------------------
void SceneManager::Render()
{
	// Can't render without a camera
	if (m_pActiveCamera == nullptr)
	{
		return;
	}

	// Iterate over the list of models and render them
	ModelList::iterator it = m_lModelList.begin(), end = m_lModelList.end();
	for (; it != end; ++it)
	{
		HeatStroke::Model* pModel = *it;
		
		pModel->Render(m_pActiveCamera);
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Render the sprite list with an ortho camera. 
	// Rendering sprites currently out of order.
	// TODO: We should really add the camera separately rather than hard code it.
	SceneCamera mSpriteCamera;
	mSpriteCamera.SetProjectionMatrix(glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1000.0f));

	SpriteList::iterator sIt = m_lSpriteList.begin(), sEnd = m_lSpriteList.end();
	for (; sIt != sEnd; ++sIt)
	{
		HeatStroke::Sprite* pSprite = static_cast<HeatStroke::Sprite*>(*sIt);
		pSprite->Render(&mSpriteCamera);
	}
}