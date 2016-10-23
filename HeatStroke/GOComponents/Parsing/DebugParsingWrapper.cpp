//==================================================================================
// DebugParsingWrapper
// Author: Bradley Cooper
//
// Wrap a ParsingService and display debug messages before calling it.
// Most methods are simple wrappers and need no method headers.
//==================================================================================

#include "DebugParsingWrapper.h"
#include <typeinfo>

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// Method:    DebugParsingWrapper
	// Parameter: ParsingService* p_pParsingService - ParsingService to wrap
	// Returns:   void
	// 
	// Constructor. Does not allow wrapping another DebugParsingWrapper.
	// Ownership of wrapped service is also passed, and will be deleted on destruct.
	//------------------------------------------------------------------------------
	DebugParsingWrapper::DebugParsingWrapper(ParsingService* p_pParsingService)
		:
		m_pParsingService(p_pParsingService)
	{
		Log("Wrapping ParsingService of " + std::string(typeid(m_pParsingService).name()));

		// Do not wrap another DebugParsingWrapper
		assert(dynamic_cast<DebugParsingWrapper*>(p_pParsingService) == 0);
	}

	//------------------------------------------------------------------------------
	// Method:    ~DebugParsingWrapper
	// Returns:   
	// 
	// Destructor. Deletes wrapped ServiceProvider.
	//------------------------------------------------------------------------------
	DebugParsingWrapper::~DebugParsingWrapper()
	{
		Log("Destructing and deleting wrapped service");
		delete m_pParsingService;
		m_pParsingService = nullptr;
	}

	// Debug wrapper
	void DebugParsingWrapper::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
	{
		Log("Registering component factory for " + p_strComponentId);
		m_pParsingService->RegisterComponentFactory(p_strComponentId, p_factoryMethod);
	}

	// Debug wrapper
	void DebugParsingWrapper::UnloadGameObjectBaseFiles()
	{
		Log("Unloading base files");
		m_pParsingService->UnloadGameObjectBaseFiles();
	}

	// Debug wrapper
	void DebugParsingWrapper::LiveReloadXml(
		std::map<std::string, GameObject*>::const_iterator begin,
		std::map<std::string, GameObject*>::const_iterator end)
	{
		Log("Live reloading XML files");
		m_pParsingService->LiveReloadXml(begin, end);
	}

	// Debug wrapper
	tinyxml2::XMLNode* DebugParsingWrapper::GetGameObjectBaseNode(const std::string& p_strBase)
	{
		Log("Getting GO base node for " + p_strBase);
		return m_pParsingService->GetGameObjectBaseNode(p_strBase);
	}

	// Debug wrapper
	void DebugParsingWrapper::ParseBaseNodeComponents(
		GameObject* p_pGameObject,
		std::set<std::string>& p_mComponentSet,
		tinyxml2::XMLNode* p_pGameObjectBaseNode,
		tinyxml2::XMLNode* p_pGameObjectOverrideNode)
	{
		Log("Parsing base node components");
		m_pParsingService->ParseBaseNodeComponents(p_pGameObject, p_mComponentSet, p_pGameObjectBaseNode, p_pGameObjectOverrideNode);
	}

	// Debug wrapper
	void DebugParsingWrapper::ParseOverrideComponents(
		GameObject* p_pGameObject,
		std::set<std::string>& p_mComponentSet,
		tinyxml2::XMLNode* p_pGameObjectOverrideNode)
	{
		Log("Parsing override components");
		m_pParsingService->ParseOverrideComponents(p_pGameObject, p_mComponentSet, p_pGameObjectOverrideNode);
	}

	// Debug wrapper
	void DebugParsingWrapper::ParseComponent(
		GameObject* p_pGameObject,
		std::set<std::string>& p_mComponentSet,
		const char* p_szComponentName,
		tinyxml2::XMLNode* p_pComponentNode,
		tinyxml2::XMLNode* p_pComponentOverrideNode)
	{
		Log("Parsing component with name" + std::string(p_szComponentName));
		m_pParsingService->ParseComponent(p_pGameObject, p_mComponentSet, p_szComponentName, p_pComponentNode, p_pComponentOverrideNode);
	}

	//----------------------------------------------------------------------------------
	// Method:    Log
	// Parameter: const std::string& p_strDebugMessage - Debug message to print or write to file
	// Returns:   void
	//
	// For now, print messages to screen. May change to file writing later.
	//----------------------------------------------------------------------------------
	void DebugParsingWrapper::Log(const std::string& p_strDebugMessage) const
	{
		printf("DebugParsingWrapper:: %s\n", p_strDebugMessage.c_str());
	}
}