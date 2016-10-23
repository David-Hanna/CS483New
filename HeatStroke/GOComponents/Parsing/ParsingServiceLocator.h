//==================================================================================
// ParsingServiceLocator
// Author: Bradley Cooper
//
// Manage access to ParsingService for components.
//==================================================================================

#ifndef PARSING_SERVICE_LOCATOR_H
#define PARSING_SERVICE_LOCATOR_H

#include <assert.h>
#include "ParsingService.h"

namespace HeatStroke
{
	class ParsingServiceLocator
	{
	public:
		//-----------------------------------------------------------------------------
		// Public Interface
		//-----------------------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(ParsingService* p_pParsingService);
		static void DestroyInstance();
		static ParsingServiceLocator* Instance();

		// Service locator methods
		ParsingService* const GetService() const { return m_pParsingService; }
		void DeleteAndReplaceParsingService(ParsingService* p_pParsingService);

	private:
		//---------------------------------------------------------------------------
		// Private members
		//---------------------------------------------------------------------------
		// Service to provide
		ParsingService* m_pParsingService;

		// Static singleton instance
		static ParsingServiceLocator* s_pParsingServiceLocatorInstance;

		//---------------------------------------------------------------------------
		// Private methods
		//---------------------------------------------------------------------------
		// Hide constructor and destructor for singleton.
		ParsingServiceLocator(ParsingService* p_pParsingService);
		virtual ~ParsingServiceLocator();

		// ParsingServiceLocators shouldn't be copied.
		ParsingServiceLocator(const ParsingServiceLocator&) = delete;
		ParsingServiceLocator& operator=(const ParsingServiceLocator&) = delete;
	};
}

#endif // PARSING_SERVICE_LOCATOR_H