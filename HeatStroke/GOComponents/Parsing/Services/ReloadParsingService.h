//==================================================================================
// ReloadParsingService
// Author: Bradley Cooper
//
// Provide the service to parse components from XML.
//==================================================================================

#ifndef RELOAD_PARSING_SERVICE_H
#define RELOAD_PARSING_SERVICE_H

#include "StoredParsingService.h"

namespace HeatStroke
{
	class ReloadParsingService : public ParsingService
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		ReloadParsingService();
		virtual ~ReloadParsingService();

		// Use to register a component's Factory Method so it can be used to create the component from XML
		virtual void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod);

		// Call after initializing a level to clear out the memory from the XML documents
		// Not calling this method will leave the files in memory so parsing them will be faster
		virtual void UnloadGameObjectBaseFiles();

		// Live reload XML files, sending changes to GameObjects in iterator list
		virtual void LiveReloadXml(
			std::map<std::string, GameObject*>::const_iterator p_begin,
			std::map<std::string, GameObject*>::const_iterator p_end);

	protected:
		//---------------------------------------------------------------------
		// Protected types
		//---------------------------------------------------------------------
		// Convenient typedefs
		typedef std::map<std::string, ComponentFactoryMethod> ComponentFactoryMap;
		typedef std::map<std::string, tinyxml2::XMLDocument*> LoadedGameObjectFilesMap;

		//---------------------------------------------------------------------------
		// Protected members
		//---------------------------------------------------------------------------
		// Map of Component factories
		ComponentFactoryMap m_mComponentFactoryMap;

		// Map of loaded XML files with base Game Object data.
		LoadedGameObjectFilesMap m_mLoadedGameObjectFilesMap;

		//---------------------------------------------------------------------------
		// Protected methods
		//---------------------------------------------------------------------------
		// Fetches the GameObject node from the base file.
		virtual tinyxml2::XMLNode* GetGameObjectBaseNode(const std::string& p_strBase);

		// Parses the components of a base Game Object node, passing it the matching override node if it exists.
		virtual void ParseBaseNodeComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectBaseNode,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode);

		// Parses the components of an instance Game Object node.
		virtual void ParseOverrideComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode);

		// Delegates a single component to its factory creation method and attaches it to the GameObject.
		virtual void ParseComponent(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			const char* p_szComponentName,
			tinyxml2::XMLNode* p_pComponentNode,
			tinyxml2::XMLNode* p_pComponentOverrideNode);

	private:
		//---------------------------------------------------------------------------
		// Private methods
		//---------------------------------------------------------------------------
		tinyxml2::XMLNode* DeepCopyChanges(tinyxml2::XMLNode* p_pOld, tinyxml2::XMLNode* p_pNew, tinyxml2::XMLDocument* p_pOwner);
	};
}

#endif // RELOAD_PARSING_SERVICE_H