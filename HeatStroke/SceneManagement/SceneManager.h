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
#include "ModelInstance.h"
#include "LineDrawer.h"
#include "SpriteInstance.h"
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
			SVS_OVERLAY,
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

		void AddModelInstance(HeatStroke::ModelInstance* p_pModelInstance);
		void RemoveModelInstance(HeatStroke::ModelInstance* p_pModelInstance);
		void ClearModelInstances();

		void AddLineDrawer(HeatStroke::LineDrawer* p_pLineDrawer);
		void RemoveLineDrawer(HeatStroke::LineDrawer* p_pLineDrawer);
		void ClearLineDrawers();

		void AddPerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera, SceneViewportSelection p_eViewportSelection);
		void RemovePerspectiveCamera(SceneViewportSelection p_eViewportSelection);
		void ClearPerspectiveCameras();

		void AddSpriteInstance(HeatStroke::SpriteInstance* p_pSpriteInstance);
		void RemoveSpriteInstance(HeatStroke::SpriteInstance* p_pSpriteInstance);
		void ClearSpriteInstances();

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
		typedef std::vector<ModelInstance*>			ModelList;
		typedef std::vector<LineDrawer*>			LineDrawerList;
		typedef std::vector<SpriteInstance*>		SpriteList;
		typedef std::vector<TextBox*>				TextBoxList;
		
		typedef std::vector<SceneAmbientLight*>			AmbientLightList;
		typedef std::vector<SceneDirectionalLight*>		DirectionalLightList;
		typedef std::vector<ScenePointLight*>			PointLightList;

		static SceneManager*	s_pSceneManagerInstance;

		GLFWwindow*				m_pWindow;

		ModelList				m_lModelList;
		LineDrawerList			m_lLineDrawerList;
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
		void RenderModel(ModelInstance* p_pModelInstance, const ScenePerspectiveCamera* p_pPerspectiveCamera);
		void SetModelLights(ModelInstance* p_pModelInstance);
		void SetMeshLights(ModelInstance* p_pModelInstance, Mesh* p_pMesh);
		void SetMeshAmbientLight(Mesh* p_pMesh);
		void SetMeshDirectionalLight(Mesh* p_pMesh);
		void SetMeshPointLight(ModelInstance* p_pModelInstance, Mesh* p_pMesh);
		ScenePointLight* DetermineClosestPointLight(ModelInstance* p_pModelInstance);

		void RenderLineDrawers(const ScenePerspectiveCamera* p_pPerspectiveCamera);
		void RenderLineDrawer(LineDrawer* p_pLineDrawer, const ScenePerspectiveCamera* p_pPerspectiveCamera);

		void RenderSprites(const SceneOrthographicCamera* p_pOrthographicCamera);
		void RenderSprite(SpriteInstance* p_pSprite, const SceneOrthographicCamera* p_pOrthographicCamera);

		void RenderTextBoxes(const SceneOrthographicCamera* p_pOrthographicCamera);
		void RenderTextBox(TextBox* p_pTextBox, const SceneOrthographicCamera* p_pOrthographicCamera);
	};
}

#endif
