//------------------------------------------------------------------------
// GameObjectManager
//
// Created:	2012/12/26
// Author:	Carel Boers
//	
// Manages the collection of Game Objects used by a game.
//------------------------------------------------------------------------

#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include <map>
#include <set>

#include "GameObject.h"

// For parsing GameObject XML
#include "tinyxml2.h"
#include "EasyXML.h"

namespace HeatStroke
{
	class GameObjectManager
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
		
		// Any object which wants to iterate over the GameObject collection will need
		// this typedef.
		typedef std::map<std::string, GameObject*> GameObjectMap;

		//---------------------------------------------------------------------
		// Public interface
		//---------------------------------------------------------------------
		// Creates a new blank slate GameObjectManager
		GameObjectManager();

		// Now automatically cleans up after itself in the destructor, so no more
		// of the user of this class being forced to remember to manually clean up.
		~GameObjectManager();

		// Use to add a Component Factory Method to the GameObjectManager so it can
		// be used in the CreateGameObject method.
		void RegisterComponentFactory(const std::string& p_strComponentId, ComponentFactoryMethod);

		// Creates a game object using the given <GameObject> node that is a child
		// of <Level> (this is at the instance level, rather than class level). 
		// Make sure all the necessary Component Factory Methods have been registered before using this.
		GameObject* CreateGameObject(tinyxml2::XMLNode* p_pGameObjectInstanceNode);

		// Overload for creating Game Objects from a base file.
		GameObject* CreateGameObject(const std::string& p_strBaseFileName, const std::string& p_strGUID = "");

		// Accessor.
		GameObject* GetGameObject(const std::string &p_strGameObjectGUID);

		// Removing Game Objects from the world.
		void DestroyGameObject(GameObject* p_pGameObject);
		void DestroyAllGameObjects();

		// This shouldn't exist? Seems to be unnecessary where GUIDs should be loaded in data
		// or randomly generated at construction.
		//bool SetGameObjectGUID(GameObject* p_pGameObject, const std::string &p_strGOGUID);
		
		// Call after initializing a level to clear out the memory from the
		// XML documents that were loaded. If the memory is not an issue,
		// not calling this method will leave the files in memory so parsing
		// them will be faster.
		void UnloadGameObjectBaseFiles();

		// Game Loop Methods
		void Update(const float p_fDelta);
		void PreRender();

		// Iterator access to allow outside traversal of GameObjects
		GameObjectMap::iterator		  begin()		{ return m_mGameObjectMap.begin(); }
		GameObjectMap::iterator		  end()			{ return m_mGameObjectMap.end(); }
		GameObjectMap::const_iterator begin() const { return m_mGameObjectMap.begin(); }
		GameObjectMap::const_iterator end()   const	{ return m_mGameObjectMap.end(); }

		// Simple empty query
		bool empty() const { return m_mGameObjectMap.empty(); }

	private:
		//---------------------------------------------------------------------
		// Private types
		//---------------------------------------------------------------------
		// Convenient data structure typedefs as usual.
		typedef std::map<std::string, ComponentFactoryMethod> ComponentFactoryMap;

		// <TO DO> Handling the XML portion of Game Objects should probably be another class.
		typedef std::map<std::string, tinyxml2::XMLDocument*> LoadedGameObjectFilesMap;

		//---------------------------------------------------------------------
		// Private members
		//---------------------------------------------------------------------
		// Flags so we can move Destroying Game Objects to the end of the Update loop.
		bool m_bUpdating;
		std::set<GameObject*> m_vToDelete;

		// This number is appended to auto-generated GUIDs to ensure uniqueness.
		unsigned int m_uiAutoGUIDNum;

		// Map of Component factories
		ComponentFactoryMap m_mComponentFactoryMap;

		// Map of Game Objects
		GameObjectMap m_mGameObjectMap;

		// Map of loaded XML files with base Game Object data.
		LoadedGameObjectFilesMap m_mLoadedGameObjectFilesMap;

		//---------------------------------------------------------------------
		// Private methods
		//---------------------------------------------------------------------
		// Prevent copying.
		GameObjectManager(const GameObjectManager&) = delete;
		GameObjectManager& operator=(const GameObjectManager) = delete;

		// Parses the attributes of the Game Object Instance Node.
		void ParseInstanceNodeAttributes(
			tinyxml2::XMLNode* p_pGameObjectInstanceNode,
			std::string& p_strGUID, 
			std::string& p_strBase);

		// Helper method for CreateGameObject to add a brand new GameObject to
		// its own list and return a pointer to it to continue construction.
		GameObject* AddBlankGameObject(const std::string& p_strGUID);

		// Fetches the GameObject node from the base file.
		tinyxml2::XMLNode* GetOrLoadGameObjectBaseNode(const std::string& p_strBase);

		// Parses the components of a base Game Object node, passing it the
		// matching override node if it exists.
		void ParseBaseNodeComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectBaseNode,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode);

		// Parses the components of an instance Game Object node.
		void ParseOverrideComponents(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			tinyxml2::XMLNode* p_pGameObjectOverrideNode);

		// Delegates a single component to its factory creation method and attaches it to
		// the GameObject.
		void ParseComponent(
			GameObject* p_pGameObject,
			std::set<std::string>& p_mComponentSet,
			const char* p_szComponentName,
			tinyxml2::XMLNode* p_pComponentNode,
			tinyxml2::XMLNode* p_pComponentOverrideNode);

		// Handles the special case of the Transform override.
		// <TO DO> This needs massive improvement. Right now it does an extra loop
		// over the children of <Override> instead of mixing in with the loop that
		// already goes over them. Also need a way to let Hierarchical Transforms
		// handle parsing children and parents by themselves.
		void ParseTransformOverride(GameObject* p_pGameObject, tinyxml2::XMLNode* p_pOverrideNode);
	};
} // namespace HeatStroke

#endif // GAMEOBJECTMANAGER_H