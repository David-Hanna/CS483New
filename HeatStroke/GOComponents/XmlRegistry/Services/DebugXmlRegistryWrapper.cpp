//==================================================================================
// DebugXmlRegistryWrapper
// Author: Bradley Cooper
//
// Wrap a XmlRegistryService and display debug messages before calling it.
// Most methods are simple wrappers and need no method headers.
//==================================================================================

#include "DebugXmlRegistryWrapper.h"

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// Method:    DebugXmlRegistryWrapper
	// Parameter: XmlRegistryService* p_pXmlRegistryService - XmlRegistryService to wrap
	// Returns:   void
	// 
	// Constructor. Does not allow wrapping another DebugXmlRegistryWrapper.
	// Ownership of wrapped service is also passed, and will be deleted on destruct.
	//------------------------------------------------------------------------------
	DebugXmlRegistryWrapper::DebugXmlRegistryWrapper(XmlRegistryService* p_pXmlRegistryService)
		:
		m_pXmlRegistryService(p_pXmlRegistryService)
	{
		Log("Wrapping XmlRegistryService of " + std::string(typeid(m_pXmlRegistryService).name()));

		// Do not wrap another DebugXmlRegistryWrapper
		assert(dynamic_cast<DebugXmlRegistryWrapper*>(p_pXmlRegistryService) == 0);
	}

	//------------------------------------------------------------------------------
	// Method:    ~DebugXmlRegistryWrapper
	// Returns:   
	// 
	// Destructor. Deletes wrapped ServiceProvider.
	//------------------------------------------------------------------------------
	DebugXmlRegistryWrapper::~DebugXmlRegistryWrapper()
	{
		Log("Destructing and deleting wrapped service");
		delete m_pXmlRegistryService;
		m_pXmlRegistryService = nullptr;
		Log("Finished destructor");
	}

	void DebugXmlRegistryWrapper::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
	{
		Log("Registering component factory " + p_strComponentId);
		m_pXmlRegistryService->RegisterComponentFactory(p_strComponentId, p_factoryMethod);
		Log("Finished registration");
	}

	bool DebugXmlRegistryWrapper::HasComponentFactory(const std::string& p_strComponentId)
	{
		Log("Returning if mapping has component factory " + p_strComponentId);
		bool bResult = m_pXmlRegistryService->HasComponentFactory(p_strComponentId);
		Log("Returning result of " + bResult);
		return bResult;
	}

	DebugXmlRegistryWrapper::ComponentFactoryMethod DebugXmlRegistryWrapper::GetComponentFactory(const std::string& p_strComponentId)
	{
		Log("Returning component factory for " + p_strComponentId);
		ComponentFactoryMethod pFactory = m_pXmlRegistryService->GetComponentFactory(p_strComponentId);
		std::string strNullptr = (pFactory == nullptr ? "nullptr" : "not nullptr");
		Log("Returning compnent factory is " + strNullptr);
		return pFactory;
	}

	void DebugXmlRegistryWrapper::RegisterGameObjectXml(const std::string& p_strGUID, const std::string& p_strXmlPath)
	{
		Log("Registering game object " + p_strGUID + " to xml file " + p_strXmlPath);
		m_pXmlRegistryService->RegisterGameObjectXml(p_strGUID, p_strXmlPath);
		Log("Finished registration");
	}

	void DebugXmlRegistryWrapper::UnregisterGameObjectXml(const std::string& p_strGUID)
	{
		Log("Unregistering game object " + p_strGUID);
		m_pXmlRegistryService->UnregisterGameObjectXml(p_strGUID);
		Log("Finished registration");
	}

	DebugXmlRegistryWrapper::XmlFileMap DebugXmlRegistryWrapper::GetChangedXmlFiles()
	{
		Log("Looking for changed XML files");
		XmlFileMap mMapping = m_pXmlRegistryService->GetChangedXmlFiles();

		std::string strMapping = "";
		XmlFileMap::iterator it = mMapping.begin(), end = mMapping.end();
		for (; it != end; it++)
		{
			strMapping += "(" + it->first + "->" + it->second + "),";
		}
		if (strMapping == "") strMapping = "none";

		Log("Changed XML files: " + strMapping);
		return mMapping;
	}

	//----------------------------------------------------------------------------------
	// Method:    Log
	// Parameter: const std::string& p_strDebugMessage - Debug message to print or write to file
	// Returns:   void
	//
	// For now, print messages to screen. May change to file writing later.
	//----------------------------------------------------------------------------------
	void DebugXmlRegistryWrapper::Log(const std::string& p_strDebugMessage) const
	{
		printf("DebugXmlRegistryWrapper:: %s\n", p_strDebugMessage.c_str());
	}
}