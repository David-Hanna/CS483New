//==================================================================================
// XmlRegistryServiceLocator
// Author: Bradley Cooper
//
// Manage access to XmlRegistryService for components.
//==================================================================================

#ifndef XML_REGISTRY_SERVICE_LOCATOR_H
#define XML_REGISTRY_SERVICE_LOCATOR_H

#include <assert.h>
#include "XmlRegistryService.h"

namespace HeatStroke
{
	class XmlRegistryServiceLocator
	{
	public:
		//-----------------------------------------------------------------------------
		// Public Interface
		//-----------------------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(XmlRegistryService* p_pXmlRegistryService);
		static void DestroyInstance();
		static XmlRegistryServiceLocator* Instance();

		// Service locator methods
		XmlRegistryService* const GetService() const { return m_pXmlRegistryService; }
		void DeleteAndReplaceXmlRegistryService(XmlRegistryService* p_pXmlRegistryService);

	private:
		//---------------------------------------------------------------------------
		// Private members
		//---------------------------------------------------------------------------
		// Service to provide
		XmlRegistryService* m_pXmlRegistryService;

		// Static singleton instance
		static XmlRegistryServiceLocator* s_pXmlRegistryServiceLocatorInstance;

		//---------------------------------------------------------------------------
		// Private methods
		//---------------------------------------------------------------------------
		// Hide constructor and destructor for singleton.
		XmlRegistryServiceLocator(XmlRegistryService* p_pXmlRegistryService);
		virtual ~XmlRegistryServiceLocator();

		// XmlRegistryServiceLocators shouldn't be copied.
		XmlRegistryServiceLocator(const XmlRegistryServiceLocator&) = delete;
		XmlRegistryServiceLocator& operator=(const XmlRegistryServiceLocator&) = delete;
	};
}

#endif // XML_REGISTRY_SERVICE_LOCATOR_H