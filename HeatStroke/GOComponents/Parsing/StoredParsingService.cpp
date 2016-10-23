//==================================================================================
// StoredParsingService
// Author: Bradley Cooper
//
// Provide the service to parse components from XML.
//==================================================================================

#include "StoredParsingService.h"
#include "GameObject.h"

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// Method:    StoredParsingService
	// Returns:   
	// 
	// Constructor.
	//------------------------------------------------------------------------------
	StoredParsingService::StoredParsingService()
		:
		m_mComponentFactoryMap(ComponentFactoryMap()),
		m_mLoadedGameObjectFilesMap(LoadedGameObjectFilesMap())
	{
	}

	//------------------------------------------------------------------------------
	// Method:    ~StoredParsingService
	// Returns:   
	// 
	// Destructor.
	//------------------------------------------------------------------------------
	StoredParsingService::~StoredParsingService()
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
	void StoredParsingService::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
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
	void StoredParsingService::UnloadGameObjectBaseFiles()
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
	// Method:    GetGameObjectBaseNode
	// Parameter: const std::string& p_strBase - File location for base component XML
	// Returns:   tinyxml2::XMLNode*
	//
	// Checks the map to see if this base node has already been loaded. If so, return it.
	// Otherwise, we have to load it into the map first, then return it. Returns nullptr on failed load.
	//-------------------------------------------------------------------------------
	tinyxml2::XMLNode* StoredParsingService::GetGameObjectBaseNode(const std::string& p_strBase)
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
	// StoredParsingService::ParseBaseNodeComponents
	//
	// Parses the components of a base Game Object node, passing it the
	// matching override node if it exists.
	//-------------------------------------------------------------------
	void StoredParsingService::ParseBaseNodeComponents(
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
	// StoredParsingService::ParseOverrideComponents
	//
	// Parses the components of an instance Game Object node.
	//-------------------------------------------------------------------
	void StoredParsingService::ParseOverrideComponents(
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
	void StoredParsingService::ParseComponent(
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
}