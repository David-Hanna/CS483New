//------------------------------------------------------------------------
// SceneManager
// Author:	David Hanna
//	
// Responsible for rendering a whole scene. Manages models, cameras,
// and lights.
//------------------------------------------------------------------------

#include "SceneManager.h"

HeatStroke::SceneManager* HeatStroke::SceneManager::s_pSceneManagerInstance = nullptr;

HeatStroke::SceneManager::SceneViewportSelection HeatStroke::SceneManager::ParseViewportSelection(const std::string& p_strViewportSelection)
{
	if (p_strViewportSelection == "full")
		return SVS_FULL;
	if (p_strViewportSelection == "overlay")
		return SVS_OVERLAY;
	if (p_strViewportSelection == "top")
		return SVS_TOP;
	if (p_strViewportSelection == "bottom")
		return SVS_BOTTOM;
	if (p_strViewportSelection == "top_left")
		return SVS_TOP_LEFT;
	if (p_strViewportSelection == "top_right")
		return SVS_TOP_RIGHT;
	if (p_strViewportSelection == "bottom_left")
		return SVS_BOTTOM_LEFT;
	if (p_strViewportSelection == "bottom_right")
		return SVS_BOTTOM_RIGHT;

#ifdef _DEBUG
	assert(false && "Invalid viewport selection.");
#endif
	return SVS_INVALID;
}

HeatStroke::SceneManager::SceneManager(GLFWwindow* p_pWindow) : m_pWindow(p_pWindow) 
{
	for (int i = 0; i < SVS_LENGTH; i++)
	{
		m_lPerspectiveCameras[i] = nullptr;
		m_lOrthographicCameras[i] = nullptr;
	}
}

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

void HeatStroke::SceneManager::AddModelInstance(HeatStroke::ModelInstance* p_pModelInstance)
{
	m_lModelList.push_back(p_pModelInstance);
}

void HeatStroke::SceneManager::RemoveModelInstance(HeatStroke::ModelInstance* p_pModelInstance)
{
	ModelList::iterator it = std::find(m_lModelList.begin(), m_lModelList.end(), p_pModelInstance);
	if (it != m_lModelList.end())
	{
		m_lModelList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearModelInstances()
{
	m_lModelList.clear();
}

void HeatStroke::SceneManager::AddPerspectiveCamera(ScenePerspectiveCamera* p_pPerspectiveCamera, SceneViewportSelection p_eViewportSelection)
{
	m_lPerspectiveCameras[p_eViewportSelection] = p_pPerspectiveCamera;
}

void HeatStroke::SceneManager::RemovePerspectiveCamera(SceneViewportSelection p_eViewportSelection)
{
	m_lPerspectiveCameras[p_eViewportSelection] = nullptr;
}

void HeatStroke::SceneManager::ClearPerspectiveCameras()
{
	for (int i = 0; i < SVS_LENGTH; i++)
	{
		m_lPerspectiveCameras[i] = nullptr;
	}
}

void HeatStroke::SceneManager::AddSpriteInstance(HeatStroke::SpriteInstance* p_pSpriteInstance)
{
	m_lSpriteList.push_back(p_pSpriteInstance);
}

void HeatStroke::SceneManager::RemoveSpriteInstance(HeatStroke::SpriteInstance* p_pSpriteInstance)
{
	SpriteList::iterator it = std::find(m_lSpriteList.begin(), m_lSpriteList.end(), p_pSpriteInstance);
	if (it != m_lSpriteList.end())
	{
		m_lSpriteList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearSpriteInstances()
{
	m_lSpriteList.clear();
}

void HeatStroke::SceneManager::AddTextBox(HeatStroke::TextBox* p_pTextBox)
{
	m_lTextBoxList.push_back(p_pTextBox);
}

void HeatStroke::SceneManager::RemoveTextBox(HeatStroke::TextBox* p_pTextBox)
{
	TextBoxList::iterator it = std::find(m_lTextBoxList.begin(), m_lTextBoxList.end(), p_pTextBox);
	if (it != m_lTextBoxList.end())
	{
		m_lTextBoxList.erase(it);
	}
}

void HeatStroke::SceneManager::ClearTextBoxes()
{
	m_lTextBoxList.clear();
}

void HeatStroke::SceneManager::AddOrthographicCamera(SceneOrthographicCamera* p_pOrthographicCamera, SceneViewportSelection p_eViewportSelection)
{
	m_lOrthographicCameras[p_eViewportSelection] = p_pOrthographicCamera;
}

void HeatStroke::SceneManager::RemoveOrthographicCamera(SceneViewportSelection p_eViewportSelection)
{
	m_lOrthographicCameras[p_eViewportSelection] = nullptr;
}

void HeatStroke::SceneManager::ClearOrthographicCameras()
{
	for (int i = 0; i < SVS_LENGTH; i++)
	{
		m_lOrthographicCameras[i] = nullptr;
	}
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
	int width, height;
	glfwGetWindowSize(m_pWindow, &width, &height);
	int halfWidth = width / 2;
	int halfHeight = height / 2;

	if (m_lPerspectiveCameras[SVS_FULL] != nullptr || m_lOrthographicCameras[SVS_FULL] != nullptr)
	{
		glViewport(0, 0, width, height);
		if (m_lPerspectiveCameras[SVS_FULL] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_FULL]);
		}
		if (m_lOrthographicCameras[SVS_FULL] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_FULL]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_FULL]);
		}
	}

	if (m_lPerspectiveCameras[SVS_OVERLAY] != nullptr || m_lOrthographicCameras[SVS_OVERLAY] != nullptr)
	{
		glViewport(0, 0, width, height);
		if (m_lPerspectiveCameras[SVS_OVERLAY] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_OVERLAY]);
		}
		if (m_lOrthographicCameras[SVS_OVERLAY] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_OVERLAY]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_OVERLAY]);
		}
	}

	if (m_lPerspectiveCameras[SVS_TOP] != nullptr || m_lOrthographicCameras[SVS_TOP] != nullptr)
	{
		glViewport(0, halfHeight, width, halfHeight);
		if (m_lPerspectiveCameras[SVS_TOP] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_TOP]);
		}
		if (m_lOrthographicCameras[SVS_TOP] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_TOP]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_TOP]);
		}
	}

	if (m_lPerspectiveCameras[SVS_BOTTOM] != nullptr || m_lOrthographicCameras[SVS_BOTTOM] != nullptr)
	{
		glViewport(0, 0, width, halfHeight);
		if (m_lPerspectiveCameras[SVS_BOTTOM] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_BOTTOM]);
		}
		if (m_lOrthographicCameras[SVS_BOTTOM] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_BOTTOM]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_BOTTOM]);
		}
	}

	if (m_lPerspectiveCameras[SVS_TOP_LEFT] != nullptr || m_lOrthographicCameras[SVS_TOP_LEFT] != nullptr)
	{
		glViewport(0, halfHeight, halfWidth, halfHeight);
		if (m_lPerspectiveCameras[SVS_TOP_LEFT] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_TOP_LEFT]);
		}
		if (m_lOrthographicCameras[SVS_TOP_LEFT] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_TOP_LEFT]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_TOP_LEFT]);
		}
	}

	if (m_lPerspectiveCameras[SVS_TOP_RIGHT] != nullptr || m_lOrthographicCameras[SVS_TOP_RIGHT] != nullptr)
	{
		glViewport(halfWidth, halfHeight, halfWidth, halfHeight);
		if (m_lPerspectiveCameras[SVS_TOP_RIGHT] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_TOP_RIGHT]);
		}
		if (m_lOrthographicCameras[SVS_TOP_RIGHT] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_TOP_RIGHT]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_TOP_RIGHT]);
		}
	}

	if (m_lPerspectiveCameras[SVS_BOTTOM_LEFT] != nullptr || m_lOrthographicCameras[SVS_BOTTOM_LEFT] != nullptr)
	{
		glViewport(0, 0, halfWidth, halfHeight);
		if (m_lPerspectiveCameras[SVS_BOTTOM_LEFT] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_BOTTOM_LEFT]);
		}
		if (m_lOrthographicCameras[SVS_BOTTOM_LEFT] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_BOTTOM_LEFT]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_BOTTOM_LEFT]);
		}
	}

	if (m_lPerspectiveCameras[SVS_BOTTOM_RIGHT] != nullptr || m_lOrthographicCameras[SVS_BOTTOM_RIGHT] != nullptr)
	{
		glViewport(halfWidth, 0, halfWidth, halfHeight);
		if (m_lPerspectiveCameras[SVS_BOTTOM_RIGHT] != nullptr)
		{
			RenderModels(m_lPerspectiveCameras[SVS_BOTTOM_RIGHT]);
		}
		if (m_lOrthographicCameras[SVS_BOTTOM_RIGHT] != nullptr)
		{
			RenderSprites(m_lOrthographicCameras[SVS_BOTTOM_RIGHT]);
			RenderTextBoxes(m_lOrthographicCameras[SVS_BOTTOM_RIGHT]);
		}
	}
}

void HeatStroke::SceneManager::RenderModels(const ScenePerspectiveCamera* p_pPerspectiveCamera)
{
	ModelList::iterator it = m_lModelList.begin(), end = m_lModelList.end();
	for (; it != end; ++it)
	{
		RenderModel(*it, p_pPerspectiveCamera);
	}
}

void HeatStroke::SceneManager::RenderModel(ModelInstance* p_pModelInstance, const ScenePerspectiveCamera* p_pPerspectiveCamera)
{
	SetModelLights(p_pModelInstance);
	p_pModelInstance->Render(p_pPerspectiveCamera);
}

void HeatStroke::SceneManager::SetModelLights(ModelInstance* p_pModelInstance)
{
	std::vector<Mesh>& vMeshes = p_pModelInstance->GetModel()->GetMeshes();
	std::vector<Mesh>::iterator meshIt = vMeshes.begin(), meshEnd = vMeshes.end();
	for (; meshIt != meshEnd; meshIt++)
	{
		SetMeshLights(p_pModelInstance, &(*meshIt));
	}
}

void HeatStroke::SceneManager::SetMeshLights(ModelInstance* p_pModelInstance, Mesh* p_pMesh)
{
	SetMeshAmbientLight(p_pMesh);
	SetMeshDirectionalLight(p_pMesh);
	SetMeshPointLight(p_pModelInstance, p_pMesh);
}

void HeatStroke::SceneManager::SetMeshAmbientLight(Mesh* p_pMesh)
{
	if (m_lAmbientLightList.size() > 0)
	{
		p_pMesh->m_pMaterial->SetUniform("AmbientLightColor", m_lAmbientLightList[0]->GetColor());
	}
	else
	{
		p_pMesh->m_pMaterial->SetUniform("AmbientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	}
}

void HeatStroke::SceneManager::SetMeshDirectionalLight(Mesh* p_pMesh)
{
	if (m_lDirectionalLightList.size() > 0)
	{
		p_pMesh->m_pMaterial->SetUniform("DirectionalLightDirection", m_lDirectionalLightList[0]->GetDirection());
		p_pMesh->m_pMaterial->SetUniform("DirectionalLightDiffuseColor", m_lDirectionalLightList[0]->GetColor());
	}
	else
	{
		p_pMesh->m_pMaterial->SetUniform("DirectionalLightDirection", glm::vec3(0.0f, 1.0f, 0.0f));
		p_pMesh->m_pMaterial->SetUniform("DirectionalLightDiffuseColor", glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

void HeatStroke::SceneManager::SetMeshPointLight(ModelInstance* p_pModelInstance, Mesh* p_pMesh)
{
	ScenePointLight* pPointLight = DetermineClosestPointLight(p_pModelInstance);
	if (pPointLight != nullptr)
	{
		p_pMesh->m_pMaterial->SetUniform("PointLightPosition", pPointLight->GetPosition());
		p_pMesh->m_pMaterial->SetUniform("PointLightDiffuseColor", pPointLight->GetDiffuse());
		p_pMesh->m_pMaterial->SetUniform("PointLightAttenuation", pPointLight->GetAttenuation());
		p_pMesh->m_pMaterial->SetUniform("PointLightRange", pPointLight->GetRange());
	}
	else
	{
		p_pMesh->m_pMaterial->SetUniform("PointLightPosition", glm::vec3(0.0f, 0.0f, 0.0f));
		p_pMesh->m_pMaterial->SetUniform("PointLightDiffuseColor", glm::vec3(0.0f, 0.0f, 0.0f));
		p_pMesh->m_pMaterial->SetUniform("PointLightAttenuation", glm::vec3(0.0f, 0.0f, 1.0f));
		p_pMesh->m_pMaterial->SetUniform("PointLightRange", 0.0f);
	}
}

HeatStroke::ScenePointLight* HeatStroke::SceneManager::DetermineClosestPointLight(ModelInstance* p_pModelInstance)
{
	if (m_lPointLightList.size() == 0)
	{
		return nullptr;
	}

	const glm::vec3& vTranslation = glm::vec3(p_pModelInstance->GetTransform()[3]);

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

void HeatStroke::SceneManager::RenderSprites(const SceneOrthographicCamera* p_pOrthographicCamera)
{
	SpriteList::iterator it = m_lSpriteList.begin(), end = m_lSpriteList.end();
	for (; it != end; ++it)
	{
		RenderSprite(*it, p_pOrthographicCamera);
	}
}

void HeatStroke::SceneManager::RenderSprite(SpriteInstance* p_pSpriteInstance, const SceneOrthographicCamera* p_pOrthographicCamera)
{
	p_pSpriteInstance->Render(p_pOrthographicCamera);
}

void HeatStroke::SceneManager::RenderTextBoxes(const SceneOrthographicCamera* p_pOrthographicCamera)
{
	TextBoxList::iterator it = m_lTextBoxList.begin(), end = m_lTextBoxList.end();
	for (; it != end; ++it)
	{
		RenderTextBox(*it, p_pOrthographicCamera);
	}
}

void HeatStroke::SceneManager::RenderTextBox(TextBox* p_pTextBox, const SceneOrthographicCamera* p_pOrthographicCamera)
{
	p_pTextBox->Render(p_pOrthographicCamera);
}