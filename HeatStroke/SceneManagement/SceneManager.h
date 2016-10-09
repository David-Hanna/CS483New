//------------------------------------------------------------------------
// SceneManager
//
// Created:	2012/12/23
// Author:	Carel Boers
//	
// A simple scene manager to manage our models and camera.
//------------------------------------------------------------------------

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Model.h"
#include "Sprite.h"
#include "SceneCamera.h"
#include "ScenePointLight.h"

#include <vector>

namespace HeatStroke
{
	class SceneManager
	{
		// Typedef for convenience
		typedef std::vector<HeatStroke::Model*> ModelList;
		typedef std::vector<HeatStroke::Sprite*> SpriteList;
		typedef std::vector<ScenePointLight*> LightList;
		typedef std::vector<SceneCamera*> CameraList;

	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static SceneManager* Instance();

		void AddModel(HeatStroke::Model* p_pModel);
		void RemoveModel(HeatStroke::Model* p_pModel);
		void Clear();

		void AddSprite(HeatStroke::Sprite* p_pSprite);
		void RemoveSprite(HeatStroke::Sprite* p_pSprite);
		void ClearSprites();

		void AddLight(ScenePointLight* p_pLight);
		void RemoveLight(ScenePointLight* p_pLight);
		void ClearLights();

		void AddCamera(SceneCamera* p_pCamera);
		void SetActiveCamera(SceneCamera* p_pCamera);
		SceneCamera* GetActiveCamera();
		void RemoveCamera(SceneCamera* p_pCamera);
		void ClearCameras();

		void Update(const float p_fDelta);
		void Render();

	private:
		//------------------------------------------------------------------------------
		// Private methods.
		//------------------------------------------------------------------------------
		
		// Constructor/Destructor are private because we're a Singleton
		SceneManager(GLFWwindow* p_pWindow);
		virtual ~SceneManager();

	private:
		//------------------------------------------------------------------------------
		// Private members.
		//------------------------------------------------------------------------------

		// Static singleton instance
		static SceneManager* s_pSceneManagerInstance;

		// A list of models to render
		ModelList m_lModelList;

		// A list of sprites to render
		SpriteList m_lSpriteList;

		// A list of lights in the scene
		LightList m_lLightList;

		// A list of cameras from which to view the current scene.
		CameraList m_lCameras;

		// The currently active camera, which is a member of m_lCameras.
		SceneCamera* m_pActiveCamera;

		// Boolean for keeping track of changes in the C key.
		bool m_bCKeyHeld;

		// The window this scene is rendering to.
		GLFWwindow* m_pWindow;
	};

} // namespace HeatStroke

#endif // SCENEMANAGER_H