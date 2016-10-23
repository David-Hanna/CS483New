//==================================================================================
// ParsingServiceLocator
// Author: Bradley Cooper
//
// Manage access to ParsingService for components.
//==================================================================================

#include "ParsingServiceLocator.h"

namespace HeatStroke
{
	// Static singleton instance
	ParsingServiceLocator* ParsingServiceLocator::s_pParsingServiceLocatorInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Parameter: ParsingService* p_pParsingService - initial parsing service
	// Returns:   void
	// 
	// Creates the singletone instance.
	//------------------------------------------------------------------------------
	void ParsingServiceLocator::CreateInstance(ParsingService* p_pParsingService)
	{
		assert(s_pParsingServiceLocatorInstance == nullptr);
		s_pParsingServiceLocatorInstance = new ParsingServiceLocator(p_pParsingService);
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void ParsingServiceLocator::DestroyInstance()
	{
		assert(s_pParsingServiceLocatorInstance != nullptr);
		delete s_pParsingServiceLocatorInstance;
		s_pParsingServiceLocatorInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   SceneManager::SceneManager*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	ParsingServiceLocator* ParsingServiceLocator::Instance()
	{
		assert(s_pParsingServiceLocatorInstance != nullptr);
		return s_pParsingServiceLocatorInstance;
	}

	//------------------------------------------------------------------------------
	// Method:    ParsingServiceLocator
	// Parameter: ParsingService* p_pParsingService - initial parsing service
	// Returns:   
	// 
	// Constructor. Passes ownership of ParsingService for deletion.
	//------------------------------------------------------------------------------
	ParsingServiceLocator::ParsingServiceLocator(ParsingService* p_pParsingService)
		:
		m_pParsingService(p_pParsingService)
	{
		assert(m_pParsingService != nullptr);
	}

	//------------------------------------------------------------------------------
	// Method:    ~ParsingServiceLocator
	// Returns:   
	// 
	// Destructor. Deletes attached ServiceProvider.
	//------------------------------------------------------------------------------
	ParsingServiceLocator::~ParsingServiceLocator()
	{
		if (m_pParsingService != nullptr)
		{
			delete m_pParsingService;
			m_pParsingService = nullptr;
		}
	}

	//------------------------------------------------------------------------------
	// Method:    DeleteAndReplaceParsingService
	// Parameter: ParsingService* p_pParsingService - initial parsing service
	// Returns:   
	// 
	// Deletes current ParsingService and replaces with new ParsingService with ownership.
	//------------------------------------------------------------------------------
	void ParsingServiceLocator::DeleteAndReplaceParsingService(ParsingService* p_pParsingService)
	{
		// Handle nullptr services loudly
		assert(p_pParsingService != nullptr);

		// Delete and replace
		delete m_pParsingService;
		m_pParsingService = nullptr;
		m_pParsingService = p_pParsingService;
	}
}