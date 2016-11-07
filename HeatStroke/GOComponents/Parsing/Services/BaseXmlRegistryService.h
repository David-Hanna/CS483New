//==================================================================================
// BaseXmlRegistryService
// Author: Bradley Cooper
//
// Provide the service to store component factory methods.
//==================================================================================

#ifndef BASE_XML_REGISTRY_SERVICE_H
#define BASE_XML_REGISTRY_SERVICE_H

#include "XmlRegistryService.h"
#include "GameObject.h"

namespace HeatStroke
{
	class BaseXmlRegistryService : public XmlRegistryService
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		BaseXmlRegistryService();
		virtual ~BaseXmlRegistryService();

		// Public interface for Component Factory Methods
		virtual void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod) override;
		virtual bool HasComponentFactory(const std::string& p_strComponentId) override;
		virtual ComponentFactoryMethod GetComponentFactory(const std::string& p_strComponentId) override;

		// Pubic interface for GameObject XML live reload (all stubbed)
		virtual void RegisterGameObjectXml(const std::string& p_strGUID, const std::string& p_strXmlPath) override {};
		virtual void UnregisterGameObjectXml(const std::string& p_strGUID) override {};
		virtual XmlFileMap GetChangedXmlFiles() override { return XmlFileMap(); };

	protected:
		//---------------------------------------------------------------------------
		// Protected members
		//---------------------------------------------------------------------------
		ComponentFactoryMap m_mComponentFactoryMap;
	};
}

#endif // BASE_XML_REGISTRY_SERVICE_H