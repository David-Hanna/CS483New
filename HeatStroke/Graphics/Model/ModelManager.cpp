//============================================================================
// ModelManager
// Author: David Hanna
//
// Manages loaded models. 
// Ensures models are only loaded from disc once (unless flushed).
// Ensures models are all deleted when this object is destroyed.
//============================================================================

#include "ModelManager.h"

HeatStroke::ModelManager* HeatStroke::ModelManager::s_pModelManagerInstance = nullptr;

void HeatStroke::ModelManager::CreateInstance()
{
	assert(s_pModelManagerInstance == nullptr);
	s_pModelManagerInstance = new ModelManager();
}

void HeatStroke::ModelManager::DestroyInstance()
{
	assert(s_pModelManagerInstance != nullptr);
	delete s_pModelManagerInstance;
	s_pModelManagerInstance = nullptr;
}

HeatStroke::ModelManager* HeatStroke::ModelManager::Instance()
{
	assert(s_pModelManagerInstance != nullptr);
	return s_pModelManagerInstance;
}

HeatStroke::ModelManager::ModelManager() :
	m_mLoadedModels()
{
}

HeatStroke::ModelManager::~ModelManager()
{
	Flush();
}

void HeatStroke::ModelManager::Preload(const std::string& p_strPreloadFileName)
{
	tinyxml2::XMLDocument mPreloadDoc;
	assert(mPreloadDoc.LoadFile(p_strPreloadFileName.c_str()) == tinyxml2::XML_NO_ERROR);
	
	tinyxml2::XMLElement* pRootElement = mPreloadDoc.RootElement();
	assert(pRootElement != nullptr);

	tinyxml2::XMLElement* pModelsElement = pRootElement->FirstChildElement("Models");
	assert(pModelsElement != nullptr);

	for (tinyxml2::XMLElement* pModelElement = pModelsElement->FirstChildElement("Model");
		pModelElement != nullptr;
		pModelElement = pModelElement->NextSiblingElement("Model"))
	{
		std::string strOBJFileName;
		EasyXML::GetRequiredStringAttribute(pModelElement, "path", strOBJFileName);

		LoadedModels::const_iterator it = m_mLoadedModels.find(strOBJFileName);
		if (it == m_mLoadedModels.end())
		{
			Model* pModel = new Model(strOBJFileName);
			m_mLoadedModels[strOBJFileName] = pModel;
		}
	}
}

HeatStroke::Model* HeatStroke::ModelManager::GetOrCreateModel(const std::string& p_strOBJFileName)
{
	LoadedModels::iterator it = m_mLoadedModels.begin(), end = m_mLoadedModels.end();
	for (; it != end; it++)
	{
		if (p_strOBJFileName == it->first)
		{
			return it->second;
		}
	}

	Model* pModel = new Model(p_strOBJFileName);
	m_mLoadedModels[p_strOBJFileName] = pModel;
	return pModel;
}

void HeatStroke::ModelManager::Flush()
{
	LoadedModels::iterator it = m_mLoadedModels.begin(), end = m_mLoadedModels.end();
	for (; it != end; it++)
	{
		DELETE_IF(it->second);
	}

	m_mLoadedModels.clear();
}