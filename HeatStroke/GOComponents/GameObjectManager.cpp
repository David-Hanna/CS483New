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
	m_mComponentFactoryMap(),
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

void GameObjectManager::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
{
#ifdef _DEBUG
	// In debug only, we check if the component factory is already in the map
	// and raise an assert to warn the programmer this is happening.
	ComponentFactoryMap::const_iterator find = m_mComponentFactoryMap.find(p_strComponentId);
	if (find != m_mComponentFactoryMap.end())
	{
		assert(false && "Component Factory was registered twice.");
	}
#endif
	// No need to check if it already exists in the map before inserting it
	// in release mode, std::map::insert does that for us.
	m_mComponentFactoryMap.insert(std::pair<std::string, ComponentFactoryMethod>(p_strComponentId, p_factoryMethod));
}

GameObject* GameObjectManager::CreateGameObject(const std::string& p_strGameObjectDefinitionFile, const std::string& p_strGuid /*= ""*/, GameObject* p_pParent /*= nullptr*/)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(p_strGameObjectDefinitionFile.c_str());
#if _DEBUG
	assert(err == tinyxml2::XML_NO_ERROR);
#endif

	return CreateGameObject(doc.FirstChildElement("GameObject"), p_strGuid, p_pParent);
}

GameObject* GameObjectManager::CreateGameObject(tinyxml2::XMLElement* p_pGameObjectRootElement, const std::string& p_strGuid /*= ""*/, GameObject* p_pParent /*= nullptr*/)
{
#if _DEBUG
	assert(p_pGameObjectRootElement != nullptr);
#endif

	std::string strGuid = p_strGuid.empty() ? GetGuid(p_pGameObjectRootElement) : p_strGuid;

	std::string strDefinitionFile = "";
	EasyXML::GetOptionalStringAttribute(p_pGameObjectRootElement, "definition", strDefinitionFile, strDefinitionFile);
	if (!strDefinitionFile.empty())
	{
		GameObject* pObj = CreateGameObject(strDefinitionFile, strGuid, p_pParent);
		pObj->m_Transform.ParseTransformNode(p_pGameObjectRootElement->FirstChildElement("Transform"));
		return pObj;
	}
	else
	{
		GameObject* pObj = new GameObject(this, strGuid);
		pObj->SetParent(p_pParent);
		pObj->m_Transform.ParseTransformNode(p_pGameObjectRootElement->FirstChildElement("Transform"));

		LoadComponents(p_pGameObjectRootElement->FirstChildElement("Components"), pObj);
		LoadChildren(p_pGameObjectRootElement->FirstChildElement("Children"), pObj);
		ParseTags(p_pGameObjectRootElement->FirstChildElement("Tags"), pObj);

		m_mGameObjectMap.insert(std::pair<std::string, GameObject*>(strGuid, pObj));
		pObj->Init();

		pObj->m_Transform.ManuallyUpdateParent();

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
			ComponentFactoryMap::iterator it = m_mComponentFactoryMap.find(strComponentType.c_str());
#if _DEBUG
			assert(it != m_mComponentFactoryMap.end());
#endif

			Component* pComponent = it->second(p_pGameObject, pComponentElement, nullptr);
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
			GameObject* pChild = CreateGameObject(pChildGameObjectElement, "", p_pParent);

			tinyxml2::XMLElement* pChildTransformElement = pChildGameObjectElement->FirstChildElement("Transform");
			if (pChildTransformElement != nullptr)
			{
				pChild->m_Transform.ParseTransformNode(pChildTransformElement);
			}
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

std::vector<GameObject*> GameObjectManager::GetGameObjectsByTag(const std::string& p_strGameObjectTag)
{
	std::vector<GameObject*> vResult;
	GameObjectMap::iterator it = m_mGameObjectMap.begin(), end = m_mGameObjectMap.end();
	for (; it != end; it++)
	{
		if (it->second->HasTag(p_strGameObjectTag))
		{
			vResult.push_back(it->second);
		}
	}

	return vResult;
}

void GameObjectManager::DestroyGameObject(GameObject* p_pGameObject)
{
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
	m_vToDelete.clear(); // Parents register children for destruction
}

void GameObjectManager::Update(const float p_fDelta)
{
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
		if ((*delete_it) == nullptr)
		{
#ifdef _DEBUG
			assert(false && "Attempting to delete nullptr");
#endif
			delete_it = m_vToDelete.erase(delete_it);
			continue;
		}

		GameObjectMap::iterator it = m_mGameObjectMap.find((*delete_it)->GetGUID());
		
		if (it != m_mGameObjectMap.end())
		{
			delete it->second;
			it->second = nullptr;
			m_mGameObjectMap.erase(it);
		}
#ifdef _DEBUG
		else
		{
			assert(false && "Could not find GameObject to delete.");
		}
#endif

		delete_it = m_vToDelete.erase(delete_it);
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