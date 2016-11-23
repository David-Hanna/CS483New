//------------------------------------------------------------------------
// SceneManager
// Author:	David Hanna
//	
// Responsible for rendering a whole scene. Manages models, cameras,
// and lights.
//------------------------------------------------------------------------

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H


#include "ScenePerspectiveCamera.h"
#include "SceneOrthographicCamera.h"
#include "Model.h"
#include "Sprite.h"
#include "TextBox.h"
#include "SceneAmbientLight.h"
#include "SceneDirectionalLight.h"
#include "ScenePointLight.h"

#include <vector>

namespace HeatStroke
{
	class SceneManager
	{
	public:
		enum SceneViewportSelection
		{
			SVS_FULL = 0,
			SVS_TOP,
			SVS_BOTTOM,
			SVS_TOP_LEFT,
			SVS_TOP_RIGHT,
			SVS_BOTTOM_LEFT,
			SVS_BOTTOM_RIGHT,
			SVS_LENGTH,
			SVS_INVALID
		};

		static SceneViewportSelection ParseViewportSelection(const std::string& p_strViewportSelection);

	public:
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static SceneManager* Instance();

		GLFWwindow* GetWindow()	{ return m_pWindow; }

		void AddModel(HeatStroke::Model* p_pModel);
		void RemoveModel(HeatStroke::Model* p_pModel);
		void ClearModels();

		void AddPerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera, SceneViewportSelection p_eViewportSelection);
		void RemovePerspectiveCamera(SceneViewportSelection p_eViewportSelection);
		void ClearPerspectiveCameras();

		void AddSprite(HeatStroke::Sprite* p_pSprite);
		void RemoveSprite(HeatStroke::Sprite* p_pSprite);
		void ClearSprites();

		void AddTextBox(HeatStroke::TextBox* p_pTextBox);
		void RemoveTextBox(HeatStroke::TextBox* p_pTextBox);
		void ClearTextBoxes();

		void AddOrthographicCamera(SceneOrthographicCamera* p_pOrthographicCamera, SceneViewportSelection p_eViewportSelection);
		void RemoveOrthographicCamera(SceneViewportSelection p_eViewportSelection);
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
		typedef std::vector<Model*>					ModelList;
		typedef std::vector<Sprite*>				SpriteList;
		typedef std::vector<TextBox*>				TextBoxList;
		
		typedef std::vector<SceneAmbientLight*>			AmbientLightList;
		typedef std::vector<SceneDirectionalLight*>		DirectionalLightList;
		typedef std::vector<ScenePointLight*>			PointLightList;

		static SceneManager*	s_pSceneManagerInstance;

		GLFWwindow*				m_pWindow;

		ModelList				m_lModelList;
		ScenePerspectiveCamera*	m_lPerspectiveCameras[SVS_LENGTH];

		SpriteList				m_lSpriteList;
		TextBoxList				m_lTextBoxList;
		SceneOrthographicCamera* m_lOrthographicCameras[SVS_LENGTH];
		
		AmbientLightList		m_lAmbientLightList;
		DirectionalLightList	m_lDirectionalLightList;
		PointLightList			m_lPointLightList;

	private:
		SceneManager(GLFWwindow* p_pWindow);

		virtual ~SceneManager() {}

		void RenderModels(const ScenePerspectiveCamera* p_pPerspectiveCamera);
		void RenderModel(Model* p_pModel, const ScenePerspectiveCamera* p_pPerspectiveCamera);
		void SetModelLights(Model* p_pModel);
		void SetMeshLights(Model* p_pModel, Mesh* p_pMesh);
		void SetMeshAmbientLight(Mesh* p_pMesh);
		void SetMeshDirectionalLight(Mesh* p_pMesh);
		void SetMeshPointLight(Model* p_pModel, Mesh* p_pMesh);
		ScenePointLight* DetermineClosestPointLight(Model* p_pModel);

		void RenderSprites(const SceneOrthographicCamera* p_pOrthographicCamera);
		void RenderSprite(Sprite* p_pSprite, const SceneOrthographicCamera* p_pOrthographicCamera);

		void RenderTextBoxes(const SceneOrthographicCamera* p_pOrthographicCamera);
		void RenderTextBox(TextBox* p_pTextBox, const SceneOrthographicCamera* p_pOrthographicCamera);
	};
}

#endif
