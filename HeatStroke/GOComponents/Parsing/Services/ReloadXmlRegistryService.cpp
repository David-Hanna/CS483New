//==================================================================================
// ReloadXmlRegistryService
// Author: Bradley Cooper
//
// Provide the service to store components and GameObject XML paths.
//==================================================================================

#include "ReloadXmlRegistryService.h"

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// Method:    ReloadXmlRegistryService
	// Returns:   
	// 
	// Constructor.
	//------------------------------------------------------------------------------
	ReloadXmlRegistryService::ReloadXmlRegistryService()
		:
		m_mComponentFactoryMap(),
		m_mXmlFileMap(),
		m_LastTimeChecked(time(0))
	{
	}

	//------------------------------------------------------------------------------
	// Method:    ~ReloadXmlRegistryService
	// Returns:   
	// 
	// Destructor.
	//------------------------------------------------------------------------------
	ReloadXmlRegistryService::~ReloadXmlRegistryService()
	{
	}

	//------------------------------------------------------------------------------
	// Method:    RegisterComponentFactory
	// Parameter: const std::string & p_strComponentId
	// Parameter: ComponentFactoryMethod p_factoryMethod
	// 
	// Registers a component factory for a given component Id.
	//------------------------------------------------------------------------------
	void ReloadXmlRegistryService::RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod p_factoryMethod)
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
	bool ReloadXmlRegistryService::HasComponentFactory(const std::string& p_strComponentId)
	{
		return m_mComponentFactoryMap.find(p_strComponentId) != m_mComponentFactoryMap.end();
	}

	//------------------------------------------------------------------------------
	// Method:    GetComponentFactory
	// Parameter: const std::string & p_strComponentId
	// Returns:   ComponentFactoryMethod
	// 
	// Gets the Component Factory Method mapped to the string.
	// Asserts false and returns nullptr if it does not exist.
	//------------------------------------------------------------------------------
	ReloadXmlRegistryService::ComponentFactoryMethod ReloadXmlRegistryService::GetComponentFactory(const std::string& p_strComponentId)
	{
		ComponentFactoryMap::const_iterator find = m_mComponentFactoryMap.find(p_strComponentId);
		return (find == m_mComponentFactoryMap.end() ? nullptr : find->second);
	}

	//------------------------------------------------------------------------------
	// Method:    RegisterGameObjectXml
	// Parameter: const std::string& p_strGUID
	//			  const std::string& p_strXmlPath
	// 
	// Register a GameObject's GUID to its associated XML file path for reload.
	// If registering a mapping again, overrides previous mapping.
	//------------------------------------------------------------------------------
	void ReloadXmlRegistryService::RegisterGameObjectXml(const std::string& p_strGUID, const std::string& p_strXmlPath)
	{
		// Ensure the component is only registered once
		XmlFileMap::iterator find = m_mXmlFileMap.find(p_strGUID);
		if (find != m_mXmlFileMap.end())
		{
			find->second = p_strXmlPath;
		}

		m_mXmlFileMap.insert(std::pair<std::string, std::string>(p_strGUID, p_strXmlPath));
	}

	//------------------------------------------------------------------------------
	// Method:    UnregisterGameObjectXml
	// Parameter: const std::string& p_strGUID
	// 
	// Remove any mappings associated with the GameObject GUID.
	//------------------------------------------------------------------------------
	void ReloadXmlRegistryService::UnregisterGameObjectXml(const std::string& p_strGUID)
	{
		XmlFileMap::iterator find = m_mXmlFileMap.find(p_strGUID);
		if (find != m_mXmlFileMap.end())
		{
			m_mXmlFileMap.erase(find);
		}
	}

	//------------------------------------------------------------------------------
	// Method:    GetChangedXmlFiles
	// Returns:   XmlFileMap
	// 
	// Returns mapping for GameObject GUID to changed XML file path.
	//------------------------------------------------------------------------------
	ReloadXmlRegistryService::XmlFileMap ReloadXmlRegistryService::GetChangedXmlFiles()
	{
		XmlFileMap mMapping;
		XmlFileMap::iterator it = m_mXmlFileMap.begin();
		while (it != m_mXmlFileMap.end())
		{
			// Check if file exists
			if (boost::filesystem::exists(it->second))
			{
				// Check if file has changed since last check
				if (boost::filesystem::last_write_time(it->second) > m_LastTimeChecked)
				{
					mMapping[it->first] = it->second;
				}

				it++;
			}
			else
			{
#ifdef _DEBUG
				assert(false && "XML file does not exist.");
#endif
				it = m_mXmlFileMap.erase(it);
			}
		}

		m_LastTimeChecked = time(0);
		return mMapping;
	}
}