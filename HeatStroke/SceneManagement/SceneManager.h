//------------------------------------------------------------------------
// SceneManager
// Author:	David Hanna
//	
// Responsible for rendering a whole scene. Manages models, cameras,
// and lights.
//------------------------------------------------------------------------

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H


#include "Sprite.h"
#include "SceneOrthographicCamera.h"

#include "Model.h"
#include "ScenePerspectiveCamera.h"

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

		void AddPerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera);
		void SetActivePerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera);
		ScenePerspectiveCamera* GetActivePerspectiveCamera();
		void RemovePerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera);
		void ClearPerspectiveCameras();

		void AddSprite(HeatStroke::Sprite* p_pSprite);
		void RemoveSprite(HeatStroke::Sprite* p_pSprite);
		void ClearSprites();

		void AddOrthographicCamera(SceneOrthographicCamera* p_pOrthographicCamera);
		void SetActiveOrthographicCamera(SceneOrthographicCamera* p_pOrthographicCamera);
		SceneOrthographicCamera* GetActiveOrthographicCamera();
		void RemoveOrthographicCamera(SceneOrthographicCamera* p_pOrthographicCamera);
		void ClearOrthographicCameras();

		void AddAmbientLight(SceneAmbientLight* p_pAmbientLight);
		void RemoveAmbientLight(SceneAmbientLight* p_pAmbientLight);
		void ClearAmbientLights();

		void AddDirectionalLight(SceneDirectionalLight* p_pDirectionalLight);
		void RemoveDirectionalLight(SceneDirectionalLight* p_pDirectionalLight);
		void ClearDirectionalLights();

		void AddPointLight(ScenePointLight* p_pPointLight);
		void RemovePointLight(ScenePointLight* p_pPointLight);
		void ClearPointLights();

		void Render();

	private:
		typedef std::vector<Model*>						ModelList;
		typedef std::vector<ScenePerspectiveCamera*>	PerspectiveCameraList;

		typedef std::vector<Sprite*>					SpriteList;
		typedef std::vector<SceneOrthographicCamera*>	OrthographicCameraList;
		
		typedef std::vector<SceneAmbientLight*>			AmbientLightList;
		typedef std::vector<SceneDirectionalLight*>		DirectionalLightList;
		typedef std::vector<ScenePointLight*>			PointLightList;

		static SceneManager*	s_pSceneManagerInstance;

		const unsigned int		NUM_AMBIENT_LIGHTS		= 1;
		const unsigned int		NUM_DIRECTIONAL_LIGHTS	= 1;
		const unsigned int		NUM_POINT_LIGHTS		= 0;

		GLFWwindow*				m_pWindow;

		ModelList				m_lModelList;
		PerspectiveCameraList	m_lPerspectiveCameras;
		ScenePerspectiveCamera*	m_pActivePerspectiveCamera;

		SpriteList				m_lSpriteList;
		OrthographicCameraList  m_lOrthographicCameras;
		SceneOrthographicCamera* m_pActiveOrthographicCamera;
		
		AmbientLightList		m_lAmbientLightList;
		DirectionalLightList	m_lDirectionalLightList;
		PointLightList			m_lPointLightList;

	private:
		SceneManager(GLFWwindow* p_pWindow) : 
			m_pActivePerspectiveCamera(nullptr), 
			m_pActiveOrthographicCamera(nullptr),
			m_pWindow(p_pWindow) 
		{}

		virtual ~SceneManager() {}

		void RenderModels();
		void RenderModel(Model* p_pModel);
		void SetModelLights(Model* p_pModel);
		void SetMeshLights(Mesh* p_pMesh);
		void SetMeshAmbientLight(Mesh* p_pMesh);
		void SetMeshDirectionalLight(Mesh* p_pMesh);

		void RenderSprites();
		void RenderSprite(Sprite* p_pSprite);
	};

}

#endif