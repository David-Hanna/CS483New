//==================================================================================
// ReloadParsingService
// Author: Bradley Cooper
//
// Provide the service to parse components from XML.
//==================================================================================

#include "ReloadParsingService.h"
#include "GameObject.h"

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// Method:    ReloadParsingService
	// Returns:   
	// 
	// Constructor.
	//------------------------------------------------------------------------------
	ReloadParsingService::ReloadParsingService()
		:
		m_mComponentFactoryMap(ComponentFactoryMap()),
		m_mLoadedGameObjectFilesMap(LoadedGameObjectFilesMap())
	{
	}

	//------------------------------------------------------------------------------
	// Method:    ~ReloadParsingService
	// Returns:   
	// 
	// Destructor.
	//------------------------------------------------------------------------------
	ReloadParsingService::~ReloadParsingService()
	{
		// Do not worry about ComponentFactoryMap as it has function pointers, not object pointers.
		UnloadGameObjectBaseFiles();
	}

	//------------------------------------------------------------------------------
	// Method:    RegisterComponentFactory
	// Parameter: const std::string & p_strComponentId
	// Parameter: ComponentFactoryMethod p_factoryMethod
	// Returns:   void
	// 
	// Registers a component factory for a given component Id.
	//------------------------------------------------------------------------------
	void ReloadParsingService::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
	{
		// Ensure the component is only registered once
		ComponentFactoryMap::const_iterator find = m_mComponentFactoryMap.find(p_strComponentId);
		if (find != m_mComponentFactoryMap.end())
		{
			assert(false && "Component Factory was registered twice.");
		}

		m_mComponentFactoryMap.insert(std::pair<std::string, ComponentFactoryMethod>(p_strComponentId, p_factoryMethod));
	}

	//------------------------------------------------------------------------------
	// Method:    UnloadGameObjectFiles
	// Returns:   void
	// 
	// Unloads the base Game Object files that have been collected using CreateGameObject().
	//------------------------------------------------------------------------------
	void ReloadParsingService::UnloadGameObjectBaseFiles()
	{
		LoadedGameObjectFilesMap::iterator it = m_mLoadedGameObjectFilesMap.begin();
		while (it != m_mLoadedGameObjectFilesMap.end())
		{
			if (it->second != nullptr)
			{
				delete it->second;
				it->second = nullptr;
			}

			it = m_mLoadedGameObjectFilesMap.erase(it);
		}
	}

	//-------------------------------------------------------------------------------
	// Method:    LiveReloadXml
	// Parameter: std::map<std::string, GameObject*>::const_iterator begin - GameObjects to iterate
	//			  std::map<std::string, GameObject*>::const_iterator end - GameObjects to iterate
	// Returns:   void
	// 
	// Reads XML files stored in map, detects changes, and passes changes to GameObjects in iterator.
	// Changes includes every difference to the tracked XML's since the program was compiled.
	// Does nothing if UnloadGameObjectFiles() is called and the mapping is cleared.
	//-------------------------------------------------------------------------------
	void ReloadParsingService::LiveReloadXml(
		std::map<std::string, GameObject*>::const_iterator p_begin,
		std::map<std::string, GameObject*>::const_iterator p_end)
	{
		// Track changes by reading through the XML files 
		tinyxml2::XMLDocument changesDocument;
		LoadedGameObjectFilesMap::iterator it = m_mLoadedGameObjectFilesMap.begin(), end = m_mLoadedGameObjectFilesMap.end();
		for (; it != end; it++)
		{
			// Load the XML file
			tinyxml2::XMLDocument baseDocument;
			tinyxml2::XMLError error = baseDocument.LoadFile(it->first.c_str());
			assert(error != tinyxml2::XML_NO_ERROR);

			// Do a deep copy of any changes found, storing them in changesDocument
			DeepCopyChanges(it->second->FirstChild(), baseDocument.FirstChild(), &changesDocument);
		}

		// Return if no changes are found
		if (changesDocument.NoChildren()) return;

		// TO DO, figure out which GO/component corresponds with an XML file
	}

	//-------------------------------------------------------------------------------
	// Method:    GetGameObjectBaseNode
	// Parameter: const std::string& p_strBase - File location for base component XML
	// Returns:   tinyxml2::XMLNode*
	//
	// Checks the map to see if this base node has already been loaded. If so, return it.
	// Otherwise, we have to load it into the map first, then return it. Returns nullptr on failed load.
	//-------------------------------------------------------------------------------
	tinyxml2::XMLNode* ReloadParsingService::GetGameObjectBaseNode(const std::string& p_strBase)
	{
		std::string strBasePath = "CS483/CS483/Kartaclysm/Data/" + p_strBase + ".xml";

		tinyxml2::XMLDocument* pBaseDocument;
		LoadedGameObjectFilesMap::const_iterator find = m_mLoadedGameObjectFilesMap.find(strBasePath);

		if (find == m_mLoadedGameObjectFilesMap.end())
		{
			// Wasn't in the map. Load it now.
			pBaseDocument = new tinyxml2::XMLDocument();
			tinyxml2::XMLError error = pBaseDocument->LoadFile(strBasePath.c_str());

			if (error != tinyxml2::XML_NO_ERROR)
			{
				return nullptr;
			}

			// Store it in map for quick retrieval later
			m_mLoadedGameObjectFilesMap[strBasePath] = pBaseDocument;
		}
		else
		{
			// It was in the map, just get it.
			pBaseDocument = find->second;
		}

		// Get the root node, which should be a Game Object Node.
		tinyxml2::XMLNode* pGameObjectBaseNode = pBaseDocument->RootElement();
		assert(strcmp(pGameObjectBaseNode->Value(), "GameObject") == 0 && "Root Node of Base Class Must be GameObject");
		return pGameObjectBaseNode;
	}

	//-------------------------------------------------------------------
	// ReloadParsingService::ParseBaseNodeComponents
	//
	// Parses the components of a base Game Object node, passing it the
	// matching override node if it exists.
	//-------------------------------------------------------------------
	void ReloadParsingService::ParseBaseNodeComponents(
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
	// ReloadParsingService::ParseOverrideComponents
	//
	// Parses the components of an instance Game Object node.
	//-------------------------------------------------------------------
	void ReloadParsingService::ParseOverrideComponents(
		GameObject* p_pGameObject,
		std::set<std::string>& p_mComponentSet,
		tinyxml2::XMLNode* p_pGameObjectOverrideNode)
	{
		assert(p_pGameObject != nullptr);
		assert(p_pGameObjectOverrideNode != nullptr);

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
	// Method:    ParseComponent
	// Parameter: GameObject* p_pGameObject - GameObject to attach component
	//			  std::set<std::string>& p_mComponentSet - Set to ensure unique components
	//			  const char* p_szComponentName - Component name to load from map
	//			  tinyxml2::XMLNode* p_pComponentNode - XML node to read component information
	//			  tinyxml2::XMLNode* p_pComponentOverrideNode - XML node with override information
	// Returns:   void
	//
	// Handles a single component by passing it to the delegate method.
	//----------------------------------------------------------------------------------
	void ReloadParsingService::ParseComponent(
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

	//----------------------------------------------------------------------------------
	// Method:    DeepChangesCopy
	// Parameter: tinyxml2::XMLNode* p_pOld - Original XML to compare against
	//			  tinyxml2::XMLNode* p_pNew - Newer copy to detect changes
	//			  tinyxml2::XMLDocument* p_pOwner - Owning document of the copies
	// Returns:   tinyxml2::XMLNode*
	//
	// Recursively copies changes between the old version and the new version into p_pOwner.
	// Returns the top-most node of the new changes XML, which is also stored in p_pOwner.
	// Unknown functionality if lines are deleted or added to the newer version.
	//----------------------------------------------------------------------------------
	tinyxml2::XMLNode* ReloadParsingService::DeepCopyChanges(tinyxml2::XMLNode* p_pOld, tinyxml2::XMLNode* p_pNew, tinyxml2::XMLDocument* p_pOwner)
	{
		// Code modified from: https://sourceforge.net/p/tinyxml/discussion/42748/thread/820b0377/
		//TO DO, test this recursion
		// Copy changes found at this level
		tinyxml2::XMLNode* pCurrent;
		if (p_pOld->ShallowEqual(p_pNew))
		{
			pCurrent = p_pNew->ShallowClone(p_pOwner);
		}

		// Recursively call all children
		tinyxml2::XMLNode* pOldChild = p_pOld->FirstChild();
		tinyxml2::XMLNode* pNewChild = p_pNew->FirstChild();
		while (pOldChild && pNewChild)
		{
			pCurrent->InsertEndChild(DeepCopyChanges(pOldChild, pNewChild, p_pOwner));
			pOldChild = pOldChild->NextSibling();
			pNewChild = pNewChild->NextSibling();
		}

		return pCurrent;
	}
}