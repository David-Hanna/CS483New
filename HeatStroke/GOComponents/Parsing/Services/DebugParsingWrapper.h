//==================================================================================
// DebugParsingWrapper
// Author: Bradley Cooper
//
// Wrap a ParsingService and display debug messages before calling it.
//==================================================================================

#ifndef DEBUG_PARSING_SERVICE_H
#define DEBUG_PARSING_SERVICE_H

#include "ParsingService.h"

namespace HeatStroke
{
	class DebugParsingWrapper : public ParsingService
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		DebugParsingWrapper(ParsingService* p_pParsingService);
		virtual ~DebugParsingWrapper();

		virtual void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod);

		virtual void UnloadGameObjectBaseFiles();

		virtual void LiveReloadXml(
			std::map<std::string, GameObject*>::const_iterator p_begin,
			std::map<std::string, GameObject*>::const_iterator p_end);

	protected:
		//---------------------------------------------------------------------------
		// Protected methods
		//---------------------------------------------------------------------------

		// Parsing service to wrap
		ParsingService* m_pParsingService;

		//---------------------------------------------------------------------------
		// Protected methods
		//---------------------------------------------------------------------------
		virtual tinyxml2::XMLNode* GetGameObjectBaseNode(const std::string& p_strBase);

		virtual void ParseBaseNodeComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectBaseNode,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode);

		virtual void ParseOverrideComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode);

		virtual void ParseComponent(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			const char* p_szComponentName,
			tinyxml2::XMLNode* p_pComponentNode,
			tinyxml2::XMLNode* p_pComponentOverrideNode);

		void Log(const std::string& p_strDebugMessage) const;
	};
}

#endif // DEBUG_PARSING_SERVICE_H