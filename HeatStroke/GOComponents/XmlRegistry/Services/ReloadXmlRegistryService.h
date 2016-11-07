//==================================================================================
// ReloadXmlRegistryService
// Author: Bradley Cooper
//
// Provide the service to store component factory methods and GameObject XML paths.
//==================================================================================

#ifndef RELOAD_XML_REGISTRY_SERVICE_H
#define RELOAD_XML_REGISTRY_SERVICE_H

#include "XmlRegistryService.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ReloadXmlRegistryService : public XmlRegistryService
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		ReloadXmlRegistryService();
		virtual ~ReloadXmlRegistryService();

		// Public interface for Component Factory Methods
		virtual void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod) override;
		virtual bool HasComponentFactory(const std::string& p_strComponentId) override;
		virtual ComponentFactoryMethod GetComponentFactory(const std::string& p_strComponentId) override;

		// Pubic interface for GameObject XML live reload
		virtual void RegisterGameObjectXml(const std::string& p_strGUID, const std::string& p_strXmlPath) override;
		virtual void UnregisterGameObjectXml(const std::string& p_strGUID) override;
		virtual XmlFileMap GetChangedXmlFiles() override;

	protected:
		//---------------------------------------------------------------------------
		// Protected members
		//---------------------------------------------------------------------------
		ComponentFactoryMap m_mComponentFactoryMap;
		XmlFileMap m_mXmlFileMap;

		// Time XML files were last checked
		std::time_t m_LastTimeChecked;
	};
}

#endif // RELOAD_XML_REGISTRY_SERVICE_H