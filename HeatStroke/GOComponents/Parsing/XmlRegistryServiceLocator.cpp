//==================================================================================
// XmlRegistryServiceLocator
// Author: Bradley Cooper
//
// Manage access to XmlRegistryService for components.
//==================================================================================

#include "XmlRegistryServiceLocator.h"

namespace HeatStroke
{
	// Static singleton instance
	XmlRegistryServiceLocator* XmlRegistryServiceLocator::s_pXmlRegistryServiceLocatorInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Parameter: XmlRegistryService* p_pXmlRegistryService - initial parsing service
	// Returns:   void
	// 
	// Creates the singletone instance.
	//------------------------------------------------------------------------------
	void XmlRegistryServiceLocator::CreateInstance(XmlRegistryService* p_pXmlRegistryService)
	{
		assert(s_pXmlRegistryServiceLocatorInstance == nullptr);
		s_pXmlRegistryServiceLocatorInstance = new XmlRegistryServiceLocator(p_pXmlRegistryService);
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void XmlRegistryServiceLocator::DestroyInstance()
	{
		assert(s_pXmlRegistryServiceLocatorInstance != nullptr);
		delete s_pXmlRegistryServiceLocatorInstance;
		s_pXmlRegistryServiceLocatorInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   SceneManager::SceneManager*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	XmlRegistryServiceLocator* XmlRegistryServiceLocator::Instance()
	{
		assert(s_pXmlRegistryServiceLocatorInstance != nullptr);
		return s_pXmlRegistryServiceLocatorInstance;
	}

	//------------------------------------------------------------------------------
	// Method:    XmlRegistryServiceLocator
	// Parameter: XmlRegistryService* p_pXmlRegistryService - initial parsing service
	// Returns:   
	// 
	// Constructor. Passes ownership of XmlRegistryService for deletion.
	//------------------------------------------------------------------------------
	XmlRegistryServiceLocator::XmlRegistryServiceLocator(XmlRegistryService* p_pXmlRegistryService)
		:
		m_pXmlRegistryService(p_pXmlRegistryService)
	{
		assert(m_pXmlRegistryService != nullptr);
	}

	//------------------------------------------------------------------------------
	// Method:    ~XmlRegistryServiceLocator
	// Returns:   
	// 
	// Destructor. Deletes attached ServiceProvider.
	//------------------------------------------------------------------------------
	XmlRegistryServiceLocator::~XmlRegistryServiceLocator()
	{
		if (m_pXmlRegistryService != nullptr)
		{
			delete m_pXmlRegistryService;
			m_pXmlRegistryService = nullptr;
		}
	}

	//------------------------------------------------------------------------------
	// Method:    DeleteAndReplaceXmlRegistryService
	// Parameter: XmlRegistryService* p_pXmlRegistryService - initial parsing service
	// Returns:   
	// 
	// Deletes current XmlRegistryService and replaces with new XmlRegistryService with ownership.
	//------------------------------------------------------------------------------
	void XmlRegistryServiceLocator::DeleteAndReplaceXmlRegistryService(XmlRegistryService* p_pXmlRegistryService)
	{
		// Handle nullptr services loudly
		assert(p_pXmlRegistryService != nullptr);

		// Delete and replace
		delete m_pXmlRegistryService;
		m_pXmlRegistryService = nullptr;
		m_pXmlRegistryService = p_pXmlRegistryService;
	}
}