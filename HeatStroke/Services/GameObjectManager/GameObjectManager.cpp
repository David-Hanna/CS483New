//------------------------------------------------------------------------
// GameObjectManager
//
// Created:	2012/12/26
// Author:	Carel Boers
// Editor:	David Hanna
// Editor:	Matthew White
//	
// Manages the collection of Game Objects used by a game.
//------------------------------------------------------------------------

#include "GameObjectManager.h"
#include "ComponentRenderable.h"

using namespace HeatStroke;

GameObjectManager::GameObjectManager()
	:
	m_bUpdating(false),
	m_vToDelete(),
	m_uiAutoGUIDNum(0),
	m_mGameObjectMap()
{
}

GameObjectManager::~GameObjectManager()
{
	//Note: m_mComponentFactoryMap doesn't need to be cleaned up since they are function pointers, not object pointers.
	DestroyAllGameObjects();

#ifdef _DEBUG
	assert(m_mGameObjectMap.size() == 0);
#endif
}

GameObject* GameObjectManager::CreateGameObject(const std::string& p_strGameObjectDefinitionFile, const std::string& p_strGuid /*= ""*/)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(p_strGameObjectDefinitionFile.c_str());
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	// Get GUID to register for live reload
	tinyxml2::XMLElement* pGameObjectRootElement = doc.FirstChildElement("GameObject");
	std::string strGuid = p_strGuid.empty() ? GetGuid(pGameObjectRootElement) : p_strGuid;

	// Register for live reload
	XmlRegistryService* pService = XmlRegistryServiceLocator::Instance()->GetService();
	pService->RegisterGameObjectXml(strGuid, p_strGameObjectDefinitionFile);

	return CreateGameObject(pGameObjectRootElement, strGuid);
}

GameObject* GameObjectManager::CreateGameObject(tinyxml2::XMLElement* p_pGameObjectRootElement, const std::string& p_strGuid /*= ""*/)
{
#if _DEBUG
	assert(p_pGameObjectRootElement != nullptr);
#endif

	// Has to be called (perhaps redundantly) to catch if this method was called directly
	std::string strGuid = p_strGuid.empty() ? GetGuid(p_pGameObjectRootElement) : p_strGuid;

	std::string strDefinitionFile = "";
	EasyXML::GetOptionalStringAttribute(p_pGameObjectRootElement, "definition", strDefinitionFile, strDefinitionFile);
	if (!strDefinitionFile.empty())
	{
		GameObject* pObj = CreateGameObject(strDefinitionFile, strGuid);
		pObj->m_Transform.ParseTransformNode(p_pGameObjectRootElement->FirstChildElement("Transform"));
		return pObj;
	}
	else
	{
		GameObject* pObj = new GameObject(this, strGuid);
		pObj->m_Transform.ParseTransformNode(p_pGameObjectRootElement->FirstChildElement("Transform"));

		LoadComponents(p_pGameObjectRootElement->FirstChildElement("Components"), pObj);
		LoadChildren(p_pGameObjectRootElement->FirstChildElement("Children"), pObj);
		ParseTags(p_pGameObjectRootElement->FirstChildElement("Tags"), pObj);

		m_mGameObjectMap.insert(std::pair<std::string, GameObject*>(strGuid, pObj));
		pObj->Init();

		return pObj;
	}
}

std::string GameObjectManager::GetGuid(tinyxml2::XMLElement* p_pGameObjectRootElement)
{
	std::string strGuid = "";
	EasyXML::GetOptionalStringAttribute(p_pGameObjectRootElement, "guid", strGuid, strGuid);
	if (strGuid.empty())
	{
		strGuid = GetAutoGuid();
	}

	return strGuid;
}

std::string GameObjectManager::GetAutoGuid()
{
	return "auto_guid_" + std::to_string(m_uiAutoGUIDNum++);
}

void GameObjectManager::LoadComponents(tinyxml2::XMLElement* p_pComponentsRootElement, GameObject* p_pGameObject)
{
#if _DEBUG
	assert(p_pGameObject != nullptr);
#endif

	if (p_pComponentsRootElement != nullptr)
	{
		for (tinyxml2::XMLElement* pComponentElement = p_pComponentsRootElement->FirstChildElement();
			 pComponentElement != nullptr;
			 pComponentElement = pComponentElement->NextSiblingElement())
		{
			std::string strComponentType = pComponentElement->Value();

			XmlRegistryService* pService = XmlRegistryServiceLocator::Instance()->GetService();
			XmlRegistryService::ComponentFactoryMethod pFactory = pService->GetComponentFactory(strComponentType.c_str());
#if _DEBUG
			assert(pFactory != nullptr);
#endif

			Component* pComponent = pFactory(p_pGameObject, pComponentElement, nullptr);
#if _DEBUG
			assert(pComponent != nullptr);
#endif

			p_pGameObject->AddComponent(pComponent);
		}
	}
}

void GameObjectManager::LoadChildren(tinyxml2::XMLElement* p_pChildrenRootElement, GameObject* p_pParent)
{
#if _DEBUG
	assert(p_pParent != nullptr);
#endif

	if (p_pChildrenRootElement != nullptr)
	{
		for (tinyxml2::XMLElement* pChildGameObjectElement = p_pChildrenRootElement->FirstChildElement("GameObject");
			 pChildGameObjectElement != nullptr;
			 pChildGameObjectElement = pChildGameObjectElement->NextSiblingElement("GameObject"))
		{
			GameObject* pChild = CreateGameObject(pChildGameObjectElement);
			pChild->m_Transform.ParseTransformNode(pChildGameObjectElement->FirstChildElement("Transform"));
			p_pParent->AddChild(pChild);
		}
	}
}

void GameObjectManager::ParseTags(tinyxml2::XMLElement* p_pTagsRootElement, GameObject* p_pGameObject)
{
#if _DEBUG
	assert(p_pGameObject != nullptr);
#endif

	if (p_pTagsRootElement != nullptr)
	{
		for (tinyxml2::XMLElement* pTagElement = p_pTagsRootElement->FirstChildElement("Tag");
			 pTagElement != nullptr;
			 pTagElement = pTagElement->NextSiblingElement("Tag"))
		{
			std::string strTag = "";
			EasyXML::GetRequiredStringAttribute(pTagElement, "value", strTag);
			p_pGameObject->AddTag(strTag);
		}
	}
}

GameObject* GameObjectManager::GetGameObject(const std::string &p_strGOGUID)
{
	GameObject* pGameObject = nullptr;
	GameObjectMap::iterator it = m_mGameObjectMap.find(p_strGOGUID);
	if (it != m_mGameObjectMap.end())
	{
		pGameObject = (GameObject*)it->second;
	}

	return pGameObject;
}

void GameObjectManager::DestroyGameObject(GameObject* p_pGameObject)
{
	// Remove from live reload
	XmlRegistryService* pService = XmlRegistryServiceLocator::Instance()->GetService();
	pService->UnregisterGameObjectXml(p_pGameObject->GetGUID());

	m_vToDelete.insert(p_pGameObject);
}

void GameObjectManager::DestroyAllGameObjects()
{
	GameObject* pGameObject = nullptr;
	GameObjectMap::iterator it = m_mGameObjectMap.begin(), end = m_mGameObjectMap.end();
	for (; it != end; ++it)
	{
		pGameObject = (GameObject*)it->second;
		delete pGameObject;
	}
	m_mGameObjectMap.clear();
}

void GameObjectManager::Update(const float p_fDelta)
{
	// Update all GameObjects
	GameObjectMap::iterator it = m_mGameObjectMap.begin(), end = m_mGameObjectMap.end();
	for (; it != end; ++it)
	{
		it->second->Update(p_fDelta);
	}

	// Delete here so we control that objects get deleted only at the end of the whole
	// update frame.
	std::set<GameObject*>::iterator delete_it = m_vToDelete.begin(), delete_end = m_vToDelete.end();
	for (; delete_it != delete_end;)
	{
		GameObjectMap::iterator it = m_mGameObjectMap.find((*delete_it)->GetGUID());
		
		if (it != m_mGameObjectMap.end())
		{
			delete it->second;
			m_mGameObjectMap.erase(it);
		}

		delete_it = m_vToDelete.erase(delete_it);
	}

	// Live reload if necessary
	if (KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_KP_9))
	{
		XmlRegistryService* pService = XmlRegistryServiceLocator::Instance()->GetService();
		XmlRegistryService::XmlFileMap mMapping = pService->GetChangedXmlFiles();

		// Iterate all changed files
		XmlRegistryService::XmlFileMap::iterator it = mMapping.begin(), end = mMapping.end();
		for (; it != end; it++)
		{
			// Find if this GameObjectManager is in charge of the GO by its GUID
			GameObjectMap::iterator find = m_mGameObjectMap.find(it->first);
			if (find != m_mGameObjectMap.end())
			{
				// Delete GameObject and its entry in the mapping
				delete find->second;
				find->second = nullptr;
				m_mGameObjectMap.erase(find);

				// Recreate from XML (reusing the same GUID)
				CreateGameObject(it->second, it->first);
			}
		}
	}
}

void GameObjectManager::PreRender()
{
	GameObjectMap::iterator it = m_mGameObjectMap.begin(), end = m_mGameObjectMap.end();
	for (; it != end; ++it)
	{
		// If this GameObject has a Renderable component, it has a PreRender phase which
		// needs to be run.
		HeatStroke::Component* pComponent = it->second->GetComponent("GOC_Renderable");
		if (pComponent)
		{
			ComponentRenderable* pRenderable = static_cast<ComponentRenderable*>(pComponent);
			pRenderable->PreRender();
		}
	}
}

//TODO: delete this (once all bugs are worked out)
//Matt: just using this to make sure game objects loaded properly
void GameObjectManager::Print()
{
	for (GameObjectMap::iterator it = m_mGameObjectMap.begin(); it != m_mGameObjectMap.end(); ++it)
	{
		it->second->Print();
	}
}