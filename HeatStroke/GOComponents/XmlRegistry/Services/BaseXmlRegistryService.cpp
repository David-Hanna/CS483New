//==================================================================================
// BaseXmlRegistryService
// Author: Bradley Cooper
//
// Provide the service to store component factory methods.
//==================================================================================

#include "BaseXmlRegistryService.h"

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// Method:    BaseXmlRegistryService
	// Returns:   
	// 
	// Constructor.
	//------------------------------------------------------------------------------
	BaseXmlRegistryService::BaseXmlRegistryService()
		:
		m_mComponentFactoryMap()
	{
	}

	//------------------------------------------------------------------------------
	// Method:    ~BaseXmlRegistryService
	// Returns:   
	// 
	// Destructor.
	//------------------------------------------------------------------------------
	BaseXmlRegistryService::~BaseXmlRegistryService()
	{
	}

	//------------------------------------------------------------------------------
	// Method:    RegisterComponentFactory
	// Parameter: const std::string & p_strComponentId
	// Parameter: ComponentFactoryMethod p_factoryMethod
	// Returns:   void
	// 
	// Registers a component factory for a given component Id.
	//------------------------------------------------------------------------------
	void BaseXmlRegistryService::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
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
	// Method:    HasComponentFactory
	// Parameter: const std::string & p_strComponentId
	// Returns:   bool
	// 
	// Returns whether there is a ComponentFactoryMethod mapped to the passed key.
	//------------------------------------------------------------------------------
	bool BaseXmlRegistryService::HasComponentFactory(const std::string& p_strComponentId)
	{
		return m_mComponentFactoryMap.find(p_strComponentId) != m_mComponentFactoryMap.end();
	}

	//------------------------------------------------------------------------------
	// Method:    GetComponentFactory
	// Parameter: const std::string & p_strComponentId
	// Returns:   ComponentFactoryMethod
	// 
	// Gets the Component Factory Method mapped to the string.
	// Returns nullptr if it does not exist.
	//------------------------------------------------------------------------------
	BaseXmlRegistryService::ComponentFactoryMethod BaseXmlRegistryService::GetComponentFactory(const std::string& p_strComponentId)
	{
		ComponentFactoryMap::const_iterator find = m_mComponentFactoryMap.find(p_strComponentId);
		return (find == m_mComponentFactoryMap.end() ? nullptr : find->second);
	}
}