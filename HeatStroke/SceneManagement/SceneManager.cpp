//------------------------------------------------------------------------
// SceneManager
//
// Created:	2012/12/23
// Author:	Carel Boers
//	
// A simple scene manager to manage our models and camera.
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



void HeatStroke::SceneManager::AddCamera(SceneCamera* p_pCamera)
{
	m_lCameras.push_back(p_pCamera);

	if (m_pActiveCamera == nullptr)
	{
		m_pActiveCamera = p_pCamera;
	}
}



void HeatStroke::SceneManager::SetActiveCamera(SceneCamera* p_pCamera)
{
	CameraList::iterator it = std::find(m_lCameras.begin(), m_lCameras.end(), p_pCamera);

	if (it == m_lCameras.end())
	{
		m_lCameras.push_back(p_pCamera);
	}

	m_pActiveCamera = p_pCamera;
}



HeatStroke::SceneCamera* HeatStroke::SceneManager::GetActiveCamera()
{
	return m_pActiveCamera;
}



void HeatStroke::SceneManager::RemoveCamera(HeatStroke::SceneCamera* p_pCamera)
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



void HeatStroke::SceneManager::ClearCameras()
{
	m_pActiveCamera = nullptr;
	m_lCameras.clear();
}



void HeatStroke::SceneManager::Render()
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
		Model* pModel = *it;
		
		std::vector<Model::Mesh>& vMeshes = pModel->GetMeshes();
		std::vector<Model::Mesh>::iterator meshIt = vMeshes.begin(), meshEnd = vMeshes.end();
		for (; meshIt != meshEnd; meshIt++)
		{
			if (m_lAmbientLightList.size() > 0)
			{
				meshIt->m_pMaterial->SetUniform("AmbientLightColor", m_lAmbientLightList[0]->GetColor());
			}

			if (m_lDirectionalLightList.size() > 0)
			{
				meshIt->m_pMaterial->SetUniform("DirectionalLightDirection", m_lDirectionalLightList[0]->GetDirection());
				meshIt->m_pMaterial->SetUniform("DirectionalLightDiffuseColor", m_lDirectionalLightList[0]->GetColor());
			}
		}

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

	// Iterate and render TextBoxes using the sprite (ortho) camera
	static bool bOnce = true;
	int iCount = 0;
	TextBoxList::iterator tbIt = m_lTextBoxList.begin(), tbEnd = m_lTextBoxList.end();
	for (; tbIt != tbEnd; ++tbIt)
	{
		HeatStroke::TextBox* pTextBox = static_cast<HeatStroke::TextBox*>(*tbIt);
		if (bOnce)
		{
			printf("Textbox %i: [%.2f , %.2f]\n", iCount++, pTextBox->GetPosition().x, pTextBox->GetPosition().y);
		}

		pTextBox->Render(&mSpriteCamera);
	}
	bOnce = false;
	return;
}