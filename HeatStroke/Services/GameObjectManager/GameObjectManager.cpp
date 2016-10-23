//------------------------------------------------------------------------
// GameObjectManager
//
// Created:	2012/12/26
// Author:	Carel Boers
//	
// Manages the collection of Game Objects used by a game.
//------------------------------------------------------------------------

#include "GameObjectManager.h"
#include "ComponentRenderable.h"

using namespace HeatStroke;

//------------------------------------------------------------------------------
// Method:    GameObjectManager
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
GameObjectManager::GameObjectManager()
	:
	m_bUpdating(false),
	m_vToDelete(),
	m_uiAutoGUIDNum(0),
	m_mGameObjectMap()
{
}

//------------------------------------------------------------------------------
// Method:    ~GameObjectManager
// Returns:   
// 
// Destructor.
// The ComponentFactoryMap doesn't need to be cleaned up since they are
// function pointers, not object pointers.
//------------------------------------------------------------------------------
GameObjectManager::~GameObjectManager()
{
	DestroyAllGameObjects();
}

//------------------------------------------------------------------------------
// Method:    CreateGameObject
// Parameter: 
//	const tinyxml2::XMLNode* p_pNode: The <GameObject> node appearing as a child
//		of <Level> which describes an instance of a GameObject to load.
//
// Returns:   GameObject*: Fully constructed, ready-to-use GameObject.
// 
// Factory creation method for GameObjects defined by a <GameObject> node
// appearing as a child of <Level>. This node is instantiation data, which also
// requires loading another "base" file to get the class data. Returns nullptr
// if the GameObject couldn't be properly constructed.
// Base files are kept in a map to prevent re-loading the same file over and over,
// but can be cleared with a call to UnloadGameObjectBaseFiles() if it is expected
// to not need them again for a while (to save the memory). It is convenient to use
// this to initialize a whole scene, then clear the memory afterwards.
//------------------------------------------------------------------------------
GameObject* GameObjectManager::CreateGameObject(tinyxml2::XMLNode* p_pGameObjectInstanceNode)
{
#ifdef _DEBUG
	assert(p_pGameObjectInstanceNode != nullptr);
	assert(strcmp(p_pGameObjectInstanceNode->Value(), "GameObject") == 0);
#endif

	//-------------------------------------------------------------------
	// Parse the GUID and the base file out of the attributes.
	// Also grab the override node, if there is one.
	//-------------------------------------------------------------------
	std::string strGUID, strBase;
	ParseInstanceNodeAttributes(p_pGameObjectInstanceNode, strGUID, strBase);
	tinyxml2::XMLNode* pGameObjectOverrideNode = EasyXML::FindChildNode(p_pGameObjectInstanceNode, "Overrides");

	//-------------------------------------------------------------------
	// Start a new, blank, Game Object.
	//-------------------------------------------------------------------
	GameObject* pGameObject = AddBlankGameObject(strGUID);

	//-------------------------------------------------------------------
	// Create the GameObject we'll start attaching Components to.
	// Also start a collection of strings keeping track of which
	// component families we've already attached (since there is an engine
	// enforced limit of one component per family per Game Object).
	// We'll use this both to avoid duplication and to keep track of which
	// nodes under the <Overrides> tag we didn't get to by parsing the
	// base file.
	//-------------------------------------------------------------------
	std::set<std::string> mComponentSet;

	//----------------------------------------------------------------------------------
	// If the Instance has specified a base file, we need to get that out of the map
	// or load it if it hasn't already been loaded.
	// Then loop over the components of the base Game Object node and add them.
	//----------------------------------------------------------------------------------
	tinyxml2::XMLNode* pGameObjectBaseNode = nullptr;
	if (strBase != "")
	{
		pGameObjectBaseNode = ParsingServiceLocator::Instance()->GetService()->GetGameObjectBaseNode(strBase);
		if (pGameObjectBaseNode != nullptr)
		{
			ParsingServiceLocator::Instance()->GetService()->ParseBaseNodeComponents(pGameObject, mComponentSet, pGameObjectBaseNode, pGameObjectOverrideNode);
		}
	}
	
	//---------------------------------------------------------------------
	// Now parse the Override data that was not matched with any base
	// component node.
	//---------------------------------------------------------------------
	if (pGameObjectOverrideNode != nullptr)
	{
		// Transform is a special case.
		ParseTransformOverride(pGameObject, pGameObjectOverrideNode);
		ParsingServiceLocator::Instance()->GetService()->ParseOverrideComponents(pGameObject, mComponentSet, pGameObjectOverrideNode);
	}

	//---------------------------------------------------------------------
	// Call the initializer which may perform additional set-up.
	//---------------------------------------------------------------------
	pGameObject->Init();

	return pGameObject;
}

//------------------------------------------------------------------------------
// GameObjectManager::CreateGameObject
// Parameters:
//		const std::string& p_strBaseFileName - the name of the file to load as a GameObject.
// Returns:
//		GameObject* - the created Game Object.
//
// Overload for creating Game Objects from a base file.
//------------------------------------------------------------------------------
GameObject* GameObjectManager::CreateGameObject(const std::string& p_strBaseFileName, const std::string& p_strGUID /*= ""*/)
{
	tinyxml2::XMLNode* pBaseNode = ParsingServiceLocator::Instance()->GetService()->GetGameObjectBaseNode(p_strBaseFileName);

	GameObject* pGameObject;

	if (p_strGUID.compare("") == 0)
	{
		std::string strAutoGUID = std::string("auto_guid_") + std::to_string(m_uiAutoGUIDNum);
		++m_uiAutoGUIDNum;
		pGameObject = AddBlankGameObject(strAutoGUID);
	}
	else
	{
		pGameObject = AddBlankGameObject(p_strGUID);
	}
	
	ParsingServiceLocator::Instance()->GetService()->ParseBaseNodeComponents(pGameObject, std::set<std::string>(), pBaseNode, nullptr);

	pGameObject->Init();

	return pGameObject;
}

//------------------------------------------------------------------------------
// Method:    GetGameObject
// Parameter: const std::string & p_strGOGUID
// Returns:   GameObject*
// 
// Returns a GameObject mapped to by the given GUID. Returns NULL if no GO is 
// found.
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Method:    DestroyGameObject
// Parameter: GameObject * p_pGameObject
// Returns:   void
// 
// Destroys the given Game Object and removes it from internal map of all Game
// Objects.
//------------------------------------------------------------------------------
void GameObjectManager::DestroyGameObject(GameObject* p_pGameObject)
{
	m_vToDelete.insert(p_pGameObject);
}

//------------------------------------------------------------------------------
// Method:    DestroyAllGameObjects
// Returns:   void
// 
// Clears the internal map of all GameObjects and deletes them.
//------------------------------------------------------------------------------
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

/*
//------------------------------------------------------------------------------
// Method:    SetGameObjectGUID
// Parameter: GameObject * p_pGameObject
// Parameter: const std::string &p_strGOGUID
// Returns:   bool
//
// Changes the name of the given GUID and updates the internal mapping. Returns
// true for a successful rename, false on naming collision.
//------------------------------------------------------------------------------
bool GameObjectManager::SetGameObjectGUID(GameObject* p_pGameObject, const std::string &p_strGOGUID)
{
GameObject* pGO = this->GetGameObject(p_strGOGUID);
if (pGO)
{
// GUID already taken
return false;
}

GameObjectMap::iterator it = m_mGameObjectMap.find(p_pGameObject->GetGUID());
if (it == m_mGameObjectMap.end())
{
// No mapping - Game Object wasn't created from this GameObjectManager instance
return false;
}

// Erase the old mapping, update the GUID and add it back
m_mGameObjectMap.erase(it);
p_pGameObject->SetGUID(p_strGOGUID);
m_mGameObjectMap.insert(std::pair<std::string, GameObject*>(p_pGameObject->GetGUID(), p_pGameObject));
return true;
}
*/

//------------------------------------------------------------------------------
// Method:    Update
// Parameter: float p_fDelta
// Returns:   void
// 
// Updates all the GameObjects
//------------------------------------------------------------------------------
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
		GameObjectMap::iterator it = m_mGameObjectMap.find((*delete_it)->GetGUID());
		
		if (it != m_mGameObjectMap.end())
		{
			delete it->second;
			m_mGameObjectMap.erase(it);
		}

		delete_it = m_vToDelete.erase(delete_it);
	}
}

//------------------------------------------------------------------------------
// Method:    PreRender
// Returns:   void
// 
// Syncs the GameObject transform to the renderable component (if one 
// exists for the given GameObject).
//------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
// GameObjectManager::ParseInstanceNodeAttributes
//
// The GUID of the instance of the Game Object and base data file are attributes
// of a GameObject instance node.
//-------------------------------------------------------------------------------
void GameObjectManager::ParseInstanceNodeAttributes(
	tinyxml2::XMLNode* p_pGameObjectInstanceNode,
	std::string& p_strGUID, 
	std::string& p_strBase)
{
	tinyxml2::XMLElement* pInstanceGameObjectElement = p_pGameObjectInstanceNode->ToElement();
	EasyXML::GetOptionalStringAttribute(pInstanceGameObjectElement, "guid", p_strGUID, std::string(""));
	if (p_strGUID.compare("") == 0)
	{
		// No GUID supplied for the object, so just auto-generate.
		p_strGUID = std::string("auto_guid_") + std::to_string(m_uiAutoGUIDNum);
		++m_uiAutoGUIDNum;
	}
	EasyXML::GetOptionalStringAttribute(pInstanceGameObjectElement, "base", p_strBase, std::string(""));
}

//-------------------------------------------------------------------------------
// GameObjectManager::AddBlankGameObject
//
// Helper method for CreateGameObject to add a brand new GameObject to
// its own list and return a pointer to it to continue construction.
//-------------------------------------------------------------------------------
GameObject* GameObjectManager::AddBlankGameObject(const std::string& p_strGUID)
{
	GameObject* pGameObject = new GameObject(this, p_strGUID);

	GameObjectMap::const_iterator find = m_mGameObjectMap.find(p_strGUID);
	if (find != m_mGameObjectMap.end())
	{
		assert(false && "Duplicate GUID found.");
	}

	m_mGameObjectMap[p_strGUID] = pGameObject;

	return pGameObject;
}

//-----------------------------------------------------------------------------------
// GameObjectManager::ParseTransformOverride
//
// Handles the special case of the Transform override node.
// Simply gets the Transform node out from the Override node and delegates
// the parsing to the Transform class.
//-----------------------------------------------------------------------------------
void GameObjectManager::ParseTransformOverride(GameObject* p_pGameObject, tinyxml2::XMLNode* p_pOverrideNode)
{
	if (p_pOverrideNode == nullptr)
		return;

	tinyxml2::XMLNode* pTransformNode = EasyXML::FindChildNode(p_pOverrideNode, "Transform");
	p_pGameObject->GetTransform().ParseTransformNode(pTransformNode);

	// The GameObjectManager needs to be the one to parse parents and children though,
	// since Transforms don't know about the GameObject hierarchy.
	// <TO DO> This can be fixed using a Service Locator.
	for (tinyxml2::XMLNode* pTransformChildNode = pTransformNode->FirstChild();
		pTransformChildNode != nullptr;
		pTransformChildNode = pTransformChildNode->NextSibling())
	{
		// skip comments
		if (pTransformChildNode->ToComment() != nullptr)
			continue;

		tinyxml2::XMLElement* pTransformChildElement = pTransformChildNode->ToElement();
		const char* szTransformChildElementName = pTransformChildElement->Value();

		if (strcmp(szTransformChildElementName, "Parent") == 0)
		{
			std::string strParentGUID;
			EasyXML::GetRequiredStringAttribute(pTransformChildElement, "guid", strParentGUID);
			GameObject* pParentGameObject = GetGameObject(strParentGUID);
			assert(pParentGameObject != nullptr);
			p_pGameObject->GetTransform().SetParent(&(pParentGameObject->GetTransform()));
		}
		else if (strcmp(szTransformChildElementName, "Child") == 0)
		{
			std::string strChildGUID;
			EasyXML::GetRequiredStringAttribute(pTransformChildElement, "guid", strChildGUID);
			GameObject* pChildGameObject = GetGameObject(strChildGUID);
			assert(pChildGameObject);
			p_pGameObject->GetTransform().AddChild(&(pChildGameObject->GetTransform()));
		}
	}
}