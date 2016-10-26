//==================================================================================
// ParsingService
// Author: Bradley Cooper
//
// Base class for a service to parse components from XML.
//==================================================================================

#ifndef PARSING_SERVICE_H
#define PARSING_SERVICE_H

#include "Component.h"
#include <map>
#include <set>

// For parsing GameObject XML
#include "tinyxml2.h"
#include "EasyXML.h"

namespace HeatStroke
{
	// Forward declaration to prevent circular header issue
	class GameObject;

	class ParsingService
	{
		// Allow GameObject manager and the debug wrapper to have access to protected methods
		friend class GameObjectManager;
		friend class DebugParsingWrapper;

	public:
		//---------------------------------------------------------------------
		// Public Types
		//---------------------------------------------------------------------
		// Typedef for the function signature which must be provided to RegisterComponentFactory().
		typedef Component*						// Return value - the pointer to the created Component.
			(*ComponentFactoryMethod)				// The typedef for the function pointer.
			(GameObject* p_pGameObject,				// The GameObject which owns the Component.
			tinyxml2::XMLNode* p_pBaseNode,			// The Base Node from which to construct the Component.
			tinyxml2::XMLNode* p_pOverrideNode);	// The Override Node from which to construct the Component.

		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		ParsingService() {};
		virtual ~ParsingService() {};

		// Use to register a component's Factory Method so it can be used to create the component from XML
		virtual void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod) = 0;

		// Call after initializing a level to clear out the memory from the XML documents
		// Not calling this method will leave the files in memory so parsing them will be faster
		virtual void UnloadGameObjectBaseFiles() = 0;

		// Reload XML files and inform components of the changes
		virtual void LiveReloadXml(
			std::map<std::string, GameObject*>::const_iterator p_begin,
			std::map<std::string, GameObject*>::const_iterator p_end) = 0;

	protected:
		//---------------------------------------------------------------------------
		// Protected methods
		//---------------------------------------------------------------------------
		// Fetches the GameObject node from the base file.
		virtual tinyxml2::XMLNode* GetGameObjectBaseNode(const std::string& p_strBase) = 0;

		// Parses the components of a base Game Object node, passing it the matching override node if it exists.
		virtual void ParseBaseNodeComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectBaseNode,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode) = 0;

		// Parses the components of an instance Game Object node.
		virtual void ParseOverrideComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode) = 0;

		// Delegates a single component to its factory creation method and attaches it to the GameObject.
		virtual void ParseComponent(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			const char* p_szComponentName,
			tinyxml2::XMLNode* p_pComponentNode,
			tinyxml2::XMLNode* p_pComponentOverrideNode) = 0;
	};
}

#endif // PARSING_SERVICE_H