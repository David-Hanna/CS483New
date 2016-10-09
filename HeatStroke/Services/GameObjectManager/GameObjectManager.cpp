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
	m_mComponentFactoryMap(),
	m_mGameObjectMap(),
	m_mLoadedGameObjectFilesMap()
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
	UnloadGameObjectBaseFiles();

#ifdef _DEBUG
	assert(m_mGameObjectMap.size() == 0);
	assert(m_mLoadedGameObjectFilesMap.size() == 0);
#endif
}

//------------------------------------------------------------------------------
// Method:    RegisterComponentFactory
// Parameter: const std::string & p_strComponentId
// Parameter: ComponentFactoryMethod p_factoryMethod
// Returns:   void
// 
// Registers a component factory for a given component Id.
//------------------------------------------------------------------------------
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
		pGameObjectBaseNode = GetOrLoadGameObjectBaseNode(strBase);
		if (pGameObjectBaseNode != nullptr)
		{
			ParseBaseNodeComponents(pGameObject, mComponentSet, pGameObjectBaseNode, pGameObjectOverrideNode);
		}
	}
	
	//---------------------------------------------------------------------
	// Now parse the Override data that was not matched with any base
	// component node.
	//---------------------------------------------------------------------
	if (pGameObjectOverrideNode != nullptr)
	{
		ParseOverrideComponents(pGameObject, mComponentSet, pGameObjectOverrideNode);
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
	tinyxml2::XMLNode* pBaseNode = GetOrLoadGameObjectBaseNode(p_strBaseFileName);

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
	
	ParseBaseNodeComponents(pGameObject, std::set<std::string>(), pBaseNode, nullptr);

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
// GameObjectManager::UnloadGameObjectFiles
// 
// Unloads the base Game Object files that have been collected using CreateGameObject().
//------------------------------------------------------------------------------
void GameObjectManager::UnloadGameObjectBaseFiles()
{
	LoadedGameObjectFilesMap::iterator it = m_mLoadedGameObjectFilesMap.begin();
	LoadedGameObjectFilesMap::iterator end = m_mLoadedGameObjectFilesMap.end();
	for (; it != end; it++)
	{
#ifdef _DEBUG
		assert(it->second != nullptr);
#endif
		delete it->second;
		it->second = nullptr;
	}
	m_mLoadedGameObjectFilesMap.clear();
}

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

//-------------------------------------------------------------------------------
// GameObjectManager::GetOrLoadGameObjectBaseNode
//
// Checks the map to see if this base node has already been loaded.
// If so, return it. Otherwise, we have to load it into the map first, then
// return it.
//-------------------------------------------------------------------------------
tinyxml2::XMLNode* GameObjectManager::GetOrLoadGameObjectBaseNode(const std::string& p_strBase)
{
	std::string strBasePath = "assignmentResources/assignment4/data/" + p_strBase + ".xml";

	tinyxml2::XMLDocument* pBaseDocument;
	LoadedGameObjectFilesMap::const_iterator find = m_mLoadedGameObjectFilesMap.find(strBasePath);

	if (find == m_mLoadedGameObjectFilesMap.end())
	{
		// Wasn't in the map. Load it now.
		pBaseDocument = new tinyxml2::XMLDocument();
		tinyxml2::XMLError error = pBaseDocument->LoadFile(strBasePath.c_str());

		if (error != tinyxml2::XML_NO_ERROR)
		{
#ifdef _DEBUG
			printf("%s\n", strBasePath.c_str());
			assert(false && "Invalid base file.");
#endif
			return nullptr;
		}

		// Put it in the map so we save on this loading time if we
		// want to read this document multiple times (ex. initialization
		// of multiple instances of the same base Game Object).
		m_mLoadedGameObjectFilesMap[strBasePath] = pBaseDocument;
	}
	else
	{
		// It was in the map, just get it.
		pBaseDocument = find->second;
	}

	// Get the root node, which should be a Game Object Node.
	tinyxml2::XMLNode* pGameObjectBaseNode = pBaseDocument->RootElement();

#ifdef _DEBUG
	assert(pGameObjectBaseNode != nullptr && "Failed to find Root Node");
	assert(strcmp(pGameObjectBaseNode->Value(), "GameObject") == 0 && "Root Node of Base Class Must be GameObject");
#endif

	return pGameObjectBaseNode;
}

//-------------------------------------------------------------------
// GameObjectManager::ParseBaseNodeComponents
//
// Parses the components of a base Game Object node, passing it the
// matching override node if it exists.
//-------------------------------------------------------------------
void GameObjectManager::ParseBaseNodeComponents(
	GameObject* p_pGameObject,
	std::set<std::string>& p_mComponentSet,
	tinyxml2::XMLNode* p_pGameObjectBaseNode,
	tinyxml2::XMLNode* p_pGameObjectOverrideNode)
{
	assert(p_pGameObject != nullptr);
	assert(p_pGameObjectBaseNode != nullptr);

	// p_pGameObjectOverrideNode is allowed to be nullptr. This means
	// there was no <Override> node under the Game Object instance node.
	// If this is the case, simply no overrides are applied.

	// Loop over the components in the Game Object base node, find the matching
	// override node, and send each one to the ParseComponent method.
	for (tinyxml2::XMLNode* pComponentNode = p_pGameObjectBaseNode->FirstChild();
		pComponentNode != nullptr;
		pComponentNode = pComponentNode->NextSibling())
	{
		// skip comments
		if (pComponentNode->ToComment() != nullptr)
			continue;

		// Get the name of the component.
		const char* szComponentName = pComponentNode->Value();

		// Find if there is a matching override node for this component. This will be nullptr if not.
		tinyxml2::XMLNode* pComponentOverrideNode = nullptr;
		if (p_pGameObjectOverrideNode != nullptr)
		{
			EasyXML::FindChildNode(p_pGameObjectOverrideNode, szComponentName);
		}
		
		// Parse the individual component.
		ParseComponent(p_pGameObject, p_mComponentSet, szComponentName, pComponentNode, pComponentOverrideNode);
	}
}

//-------------------------------------------------------------------
// GameObjectManager::ParseOverrideComponents
//
// Parses the components of an instance Game Object node.
//-------------------------------------------------------------------
void GameObjectManager::ParseOverrideComponents(
	GameObject* p_pGameObject,
	std::set<std::string>& p_mComponentSet,
	tinyxml2::XMLNode* p_pGameObjectOverrideNode)
{
	assert(p_pGameObject != nullptr);
	assert(p_pGameObjectOverrideNode != nullptr);

	// Transform is a special case.
	ParseTransformOverride(p_pGameObject, p_pGameObjectOverrideNode);

	for (tinyxml2::XMLNode* pComponentOverrideNode = p_pGameObjectOverrideNode->FirstChild();
		pComponentOverrideNode != nullptr;
		pComponentOverrideNode = pComponentOverrideNode->NextSibling())
	{
		// skip comments
		if (pComponentOverrideNode->ToComment() != nullptr)
			continue;

		// Get the name of the component.
		const char* szComponentName = pComponentOverrideNode->Value();

		// Also skip the Transform.
		if (strcmp(szComponentName, "Transform") == 0)
			continue;

		// Parse the individual component.
		ParseComponent(p_pGameObject, p_mComponentSet, szComponentName, nullptr, pComponentOverrideNode);
	}
}

//----------------------------------------------------------------------------------
// GameObjectManager::ParseComponent
//
// Handles a single component by passing it to the delegate method.
//----------------------------------------------------------------------------------
void GameObjectManager::ParseComponent(
	GameObject* p_pGameObject,
	std::set<std::string>& p_mComponentSet,
	const char* p_szComponentName,
	tinyxml2::XMLNode* p_pComponentNode,
	tinyxml2::XMLNode* p_pComponentOverrideNode)
{
	// Check that we haven't parsed this component before.
	if (p_mComponentSet.find(p_szComponentName) != p_mComponentSet.end())
		return;
	p_mComponentSet.insert(p_szComponentName);

	// Find the appropriate factory method to use for construction.
	ComponentFactoryMap::iterator it = m_mComponentFactoryMap.find(p_szComponentName);
	assert(it != m_mComponentFactoryMap.end() && "Invalid Component Name");

	// Delegate to the factory method.
	Component* pComponent = it->second(p_pGameObject, p_pComponentNode, p_pComponentOverrideNode);
	assert(pComponent != nullptr && "Component Failed to be Constructed");

	// Attach the component to the Game Object.
	p_pGameObject->AddComponent(pComponent);
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