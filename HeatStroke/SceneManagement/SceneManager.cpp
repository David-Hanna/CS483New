//------------------------------------------------------------------------
// SceneManager
// Author:	David Hanna
//	
// Responsible for rendering a whole scene. Manages models, cameras,
// and lights.
//------------------------------------------------------------------------

#include "SceneManager.h"

HeatStroke::SceneManager* HeatStroke::SceneManager::s_pSceneManagerInstance = nullptr;

void HeatStroke::SceneManager::CreateInstance(GLFWwindow* p_pWindow)
{
	assert(s_pSceneManagerInstance == nullptr);
	s_pSceneManagerInstance = new SceneManager(p_pWindow);
}

void HeatStroke::SceneManager::DestroyInstance()
{
	assert(s_pSceneManagerInstance != nullptr);
	delete s_pSceneManagerInstance;
	s_pSceneManagerInstance = nullptr;
}

HeatStroke::SceneManager* HeatStroke::SceneManager::Instance()
{
	assert(s_pSceneManagerInstance != nullptr);
	return s_pSceneManagerInstance;
}

void HeatStroke::SceneManager::AddSprite(HeatStroke::Sprite* p_pSprite)
{
	m_lSpriteList.push_back(p_pSprite);
}

void HeatStroke::SceneManager::RemoveSprite(HeatStroke::Sprite* p_pSprite)
{
	SpriteList::iterator it = std::find(m_lSpriteList.begin(), m_lSpriteList.end(), p_pSprite);
	if (it != m_lSpriteList.end())
	{
		m_lSpriteList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearSprites()
{
	m_lSpriteList.clear();
}

void HeatStroke::SceneManager::AddOrthographicCamera(SceneOrthographicCamera* p_pOrthographicCamera)
{
	m_lOrthographicCameras.push_back(p_pOrthographicCamera);

	if (m_pActiveOrthographicCamera == nullptr)
	{
		m_pActiveOrthographicCamera = p_pOrthographicCamera;
	}
}

void HeatStroke::SceneManager::SetActiveOrthographicCamera(SceneOrthographicCamera* p_pCamera)
{
	OrthographicCameraList::iterator it = std::find(m_lOrthographicCameras.begin(), m_lOrthographicCameras.end(), p_pCamera);

	if (it == m_lOrthographicCameras.end())
	{
		m_lOrthographicCameras.push_back(p_pCamera);
	}

	m_pActiveOrthographicCamera = p_pCamera;
}

HeatStroke::SceneOrthographicCamera* HeatStroke::SceneManager::GetActiveOrthographicCamera()
{
	return m_pActiveOrthographicCamera;
}

void HeatStroke::SceneManager::RemoveOrthographicCamera(HeatStroke::SceneOrthographicCamera* p_pCamera)
{
	OrthographicCameraList::iterator it = std::find(m_lOrthographicCameras.begin(), m_lOrthographicCameras.end(), p_pCamera);

	if (it != m_lOrthographicCameras.end())
	{
		m_lOrthographicCameras.erase(it);

		if (p_pCamera == m_pActiveOrthographicCamera)
		{
			if (m_lOrthographicCameras.empty())
			{
				m_pActiveOrthographicCamera = nullptr;
			}
			else
			{
				m_pActiveOrthographicCamera = m_lOrthographicCameras[0];
			}
		}
	}
}

void HeatStroke::SceneManager::ClearOrthographicCameras()
{
	m_pActiveOrthographicCamera = nullptr;
	m_lOrthographicCameras.clear();
}

void HeatStroke::SceneManager::AddModel(HeatStroke::Model* p_pModel)
{
	m_lModelList.push_back(p_pModel);
}

void HeatStroke::SceneManager::RemoveModel(HeatStroke::Model* p_pModel)
{
	ModelList::iterator it = std::find(m_lModelList.begin(), m_lModelList.end(), p_pModel);
	if (it != m_lModelList.end())
	{
		m_lModelList.erase(it);
	}	
}

void HeatStroke::SceneManager::ClearModels()
{
	m_lModelList.clear();
}

void HeatStroke::SceneManager::AddPerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera)
{
	m_lPerspectiveCameras.push_back(p_pPerspectiveCamera);

	if (m_pActivePerspectiveCamera == nullptr)
	{
		m_pActivePerspectiveCamera = p_pPerspectiveCamera;
	}
}

void HeatStroke::SceneManager::SetActivePerspectiveCamera(ScenePerspectiveCamera* p_pCamera)
{
	PerspectiveCameraList::iterator it = std::find(m_lPerspectiveCameras.begin(), m_lPerspectiveCameras.end(), p_pCamera);

	if (it == m_lPerspectiveCameras.end())
	{
		m_lPerspectiveCameras.push_back(p_pCamera);
	}

	m_pActivePerspectiveCamera = p_pCamera;
}

HeatStroke::ScenePerspectiveCamera* HeatStroke::SceneManager::GetActivePerspectiveCamera()
{
	return m_pActivePerspectiveCamera;
}

void HeatStroke::SceneManager::RemovePerspectiveCamera(HeatStroke::ScenePerspectiveCamera* p_pCamera)
{
	PerspectiveCameraList::iterator it = std::find(m_lPerspectiveCameras.begin(), m_lPerspectiveCameras.end(), p_pCamera);

	if (it != m_lPerspectiveCameras.end())
	{
		m_lPerspectiveCameras.erase(it);

		if (p_pCamera == m_pActivePerspectiveCamera)
		{
			if (m_lPerspectiveCameras.empty())
			{
				m_pActivePerspectiveCamera = nullptr;
			}
			else
			{
				m_pActivePerspectiveCamera = m_lPerspectiveCameras[0];
			}
		}
	}
}

void HeatStroke::SceneManager::ClearPerspectiveCameras()
{
	m_pActivePerspectiveCamera = nullptr;
	m_lPerspectiveCameras.clear();
}

void HeatStroke::SceneManager::AddAmbientLight(HeatStroke::SceneAmbientLight* p_pAmbientLight)
{
	m_lAmbientLightList.push_back(p_pAmbientLight);
}

void HeatStroke::SceneManager::RemoveAmbientLight(HeatStroke::SceneAmbientLight* p_pAmbientLight)
{
	AmbientLightList::iterator it = std::find(m_lAmbientLightList.begin(), m_lAmbientLightList.end(), p_pAmbientLight);
	if (it != m_lAmbientLightList.end())
	{
		m_lAmbientLightList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearAmbientLights()
{
	m_lAmbientLightList.clear();
}

void HeatStroke::SceneManager::AddDirectionalLight(HeatStroke::SceneDirectionalLight* p_pDirectionalLight)
{
	m_lDirectionalLightList.push_back(p_pDirectionalLight);
}

void HeatStroke::SceneManager::RemoveDirectionalLight(HeatStroke::SceneDirectionalLight* p_pDirectionalLight)
{
	DirectionalLightList::iterator it = std::find(m_lDirectionalLightList.begin(), m_lDirectionalLightList.end(), p_pDirectionalLight);
	if (it != m_lDirectionalLightList.end())
	{
		m_lDirectionalLightList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearDirectionalLights()
{
	m_lDirectionalLightList.clear();
}

void HeatStroke::SceneManager::AddPointLight(HeatStroke::ScenePointLight* p_pPointLight)
{
	m_lPointLightList.push_back(p_pPointLight);
}

void HeatStroke::SceneManager::RemovePointLight(HeatStroke::ScenePointLight* p_pPointLight)
{
	PointLightList::iterator it = std::find(m_lPointLightList.begin(), m_lPointLightList.end(), p_pPointLight);
	if (it != m_lPointLightList.end())
	{
		m_lPointLightList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearPointLights()
{
	m_lPointLightList.clear();
}

void HeatStroke::SceneManager::Render()
{
	if (m_pActivePerspectiveCamera != nullptr)
	{
		RenderModels();
	}

	if (m_pActiveOrthographicCamera != nullptr)
	{
		RenderSprites();
	}
}

void HeatStroke::SceneManager::RenderModels()
{
	ModelList::iterator it = m_lModelList.begin(), end = m_lModelList.end();
	for (; it != end; ++it)
	{
		RenderModel(*it);
	}
}

void HeatStroke::SceneManager::RenderModel(Model* p_pModel)
{
	SetModelLights(p_pModel);
	p_pModel->Render(m_pActivePerspectiveCamera);
}

void HeatStroke::SceneManager::SetModelLights(Model* p_pModel)
{
	std::vector<Mesh>& vMeshes = p_pModel->GetMeshes();
	std::vector<Mesh>::iterator meshIt = vMeshes.begin(), meshEnd = vMeshes.end();
	for (; meshIt != meshEnd; meshIt++)
	{
		SetMeshLights(p_pModel, &(*meshIt));
	}
}

void HeatStroke::SceneManager::SetMeshLights(Model* p_pModel, Mesh* p_pMesh)
{
	SetMeshAmbientLight(p_pMesh);
	SetMeshDirectionalLight(p_pMesh);
	SetMeshPointLight(p_pModel, p_pMesh);
}

void HeatStroke::SceneManager::SetMeshAmbientLight(Mesh* p_pMesh)
{
	if (m_lAmbientLightList.size() > 0)
	{
		p_pMesh->m_pMaterial->SetUniform("AmbientLightColor", m_lAmbientLightList[0]->GetColor());
	}
}

void HeatStroke::SceneManager::SetMeshDirectionalLight(Mesh* p_pMesh)
{
	if (m_lDirectionalLightList.size() > 0)
	{
		p_pMesh->m_pMaterial->SetUniform("DirectionalLightDirection", m_lDirectionalLightList[0]->GetDirection());
		p_pMesh->m_pMaterial->SetUniform("DirectionalLightDiffuseColor", m_lDirectionalLightList[0]->GetColor());
	}
}

void HeatStroke::SceneManager::SetMeshPointLight(Model* p_pModel, Mesh* p_pMesh)
{
	ScenePointLight* pPointLight = DetermineClosestPointLight(p_pModel);
	if (pPointLight != nullptr)
	{
		p_pMesh->m_pMaterial->SetUniform("PointLightPosition", pPointLight->GetPosition());
		p_pMesh->m_pMaterial->SetUniform("PointLightDiffuseColor", pPointLight->GetDiffuse());
		p_pMesh->m_pMaterial->SetUniform("PointLightAttenuation", pPointLight->GetAttenuation());
		p_pMesh->m_pMaterial->SetUniform("PointLightRange", pPointLight->GetRange());
	}
}

HeatStroke::ScenePointLight* HeatStroke::SceneManager::DetermineClosestPointLight(Model* p_pModel)
{
	if (m_lPointLightList.size() == 0)
	{
		return nullptr;
	}

	const glm::vec3& vTranslation = p_pModel->GetTransform().GetTranslation();

	ScenePointLight* pClosestPointLight = m_lPointLightList[0];
	float fClosestPointLightDistance = glm::distance(vTranslation, pClosestPointLight->GetPosition());

	PointLightList::iterator it = m_lPointLightList.begin(), end = m_lPointLightList.end();
	it++; // skip the first one
	for (; it != end; it++)
	{
		float fDistance = glm::distance(vTranslation, (*it)->GetPosition());
		if (fDistance < fClosestPointLightDistance)
		{
			pClosestPointLight = (*it);
		}
	}

	return pClosestPointLight;
}

void HeatStroke::SceneManager::RenderSprites()
{
	SpriteList::iterator it = m_lSpriteList.begin(), end = m_lSpriteList.end();
	for (; it != end; ++it)
	{
		RenderSprite(*it);
	}
}

void HeatStroke::SceneManager::RenderSprite(Sprite* p_pSprite)
{
	p_pSprite->Render(m_pActiveOrthographicCamera);
}