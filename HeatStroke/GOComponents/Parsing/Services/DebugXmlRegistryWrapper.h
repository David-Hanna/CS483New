//==================================================================================
// DebugXmlRegistryWrapper
// Author: Bradley Cooper
//
// Wrap a XmlRegistryService and display debug messages.
//==================================================================================

#ifndef DEBUG_PARSING_SERVICE_H
#define DEBUG_PARSING_SERVICE_H

#include "XmlRegistryService.h"
#include <assert.h>
#include <typeinfo>

namespace HeatStroke
{
	class DebugXmlRegistryWrapper : public XmlRegistryService
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		DebugXmlRegistryWrapper(XmlRegistryService* p_pXmlRegistryService);
		virtual ~DebugXmlRegistryWrapper();

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
		XmlRegistryService* m_pXmlRegistryService;

		//---------------------------------------------------------------------------
		// Protected methods
		//---------------------------------------------------------------------------
		void Log(const std::string& p_strDebugMessage) const;
	};
}

#endif // DEBUG_PARSING_SERVICE_H