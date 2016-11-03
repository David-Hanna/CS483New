//------------------------------------------------------------------------
// SceneManager
// Author:	David Hanna
//	
// Responsible for rendering a whole scene. Manages models, cameras,
// and lights.
//------------------------------------------------------------------------

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Model.h"
#include "Sprite.h"
#include "SceneCamera.h"
#include "SceneAmbientLight.h"
#include "SceneDirectionalLight.h"
#include "ScenePointLight.h"

#include <vector>

namespace HeatStroke
{
	class SceneManager
	{
	public:
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static SceneManager* Instance();

		void AddModel(HeatStroke::Model* p_pModel);
		void RemoveModel(HeatStroke::Model* p_pModel);
		void ClearModels();

		void AddSprite(HeatStroke::Sprite* p_pSprite);
		void RemoveSprite(HeatStroke::Sprite* p_pSprite);
		void ClearSprites();

		void AddAmbientLight(SceneAmbientLight* p_pAmbientLight);
		void RemoveAmbientLight(SceneAmbientLight* p_pAmbientLight);
		void ClearAmbientLights();

		void AddDirectionalLight(SceneDirectionalLight* p_pDirectionalLight);
		void RemoveDirectionalLight(SceneDirectionalLight* p_pDirectionalLight);
		void ClearDirectionalLights();

		void AddPointLight(ScenePointLight* p_pPointLight);
		void RemovePointLight(ScenePointLight* p_pPointLight);
		void ClearPointLights();

		void AddCamera(SceneCamera* p_pCamera);
		void SetActiveCamera(SceneCamera* p_pCamera);
		SceneCamera* GetActiveCamera();
		void RemoveCamera(SceneCamera* p_pCamera);
		void ClearCameras();

		void Render();

	private:
		typedef std::vector<Model*>					ModelList;
		typedef std::vector<Sprite*>				SpriteList;
		typedef std::vector<SceneAmbientLight*>		AmbientLightList;
		typedef std::vector<SceneDirectionalLight*> DirectionalLightList;
		typedef std::vector<ScenePointLight*>		PointLightList;
		typedef std::vector<SceneCamera*>			CameraList;


		static SceneManager*	s_pSceneManagerInstance;

		const unsigned int		NUM_AMBIENT_LIGHTS		= 1;
		const unsigned int		NUM_DIRECTIONAL_LIGHTS	= 1;
		const unsigned int		NUM_POINT_LIGHTS		= 0;

		GLFWwindow*				m_pWindow;

		ModelList				m_lModelList;
		SpriteList				m_lSpriteList;
		AmbientLightList		m_lAmbientLightList;
		DirectionalLightList	m_lDirectionalLightList;
		PointLightList			m_lPointLightList;
		CameraList				m_lCameras;
		SceneCamera*			m_pActiveCamera;

	private:
		SceneManager(GLFWwindow* p_pWindow) : m_pActiveCamera(nullptr), m_pWindow(p_pWindow) {}
		virtual ~SceneManager() {}
	};

} // namespace HeatStroke

#endif // SCENEMANAGER_H