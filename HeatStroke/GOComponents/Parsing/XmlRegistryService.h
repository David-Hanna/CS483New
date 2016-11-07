//==================================================================================
// XmlRegistryService
// Author: Bradley Cooper
//
// Interface for a service to parse GameObjects and components from XML.
//==================================================================================

#ifndef XML_REGISTRY_SERVICE_H
#define XML_REGISTRY_SERVICE_H

#include "tinyxml2.h"
#include "Component.h"
#include "boost\filesystem.hpp"
#include <map>
#include <set>

namespace HeatStroke
{
	// Forward declaration
	class GameObject;

	class XmlRegistryService
	{
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

		typedef std::map<std::string, std::string> XmlFileMap; // Map < GUID, XML >

		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		XmlRegistryService() {};
		virtual ~XmlRegistryService() {};

		// Public interface for Component Factory Methods
		virtual void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod) = 0;
		virtual bool HasComponentFactory(const std::string& p_strComponentId) = 0;
		virtual ComponentFactoryMethod GetComponentFactory(const std::string& p_strComponentId) = 0;

		// Pubic interface for GameObject XML live reload
		virtual void RegisterGameObjectXml(const std::string& p_strGUID, const std::string& p_strXmlPath) = 0;
		virtual void UnregisterGameObjectXml(const std::string& p_strGUID) = 0;
		virtual XmlFileMap GetChangedXmlFiles() = 0;

	protected:
		//---------------------------------------------------------------------------
		// Protected type
		//---------------------------------------------------------------------------
		typedef std::map<std::string, ComponentFactoryMethod> ComponentFactoryMap;

		//---------------------------------------------------------------------------
		// Protected members
		//---------------------------------------------------------------------------
		ComponentFactoryMap m_mComponentFactoryMap;
		XmlFileMap m_mXmlFileMap;
	};
}

#endif // XML_REGISTRY_H