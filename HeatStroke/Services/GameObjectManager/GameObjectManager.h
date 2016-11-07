//------------------------------------------------------------------------
// GameObjectManager
//
// Created:	2012/12/26
// Author:	Carel Boers
// Editor:	David Hanna
// Editor:	Matthew White
//	
// Manages the collection of Game Objects used by a game.
//------------------------------------------------------------------------

//TODO: re-implement override element in xml files
//Matt: override was removed for now to simplify converting to a hierarchical GameObject.
//		it's a really powerful feature though, so we should re-implement soon

#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include <map>
#include <set>

#include "GameObject.h"
#include "XmlRegistryServiceLocator.h"
#include "KeyboardInputBuffer.h"

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
		// Any object which wants to iterate over the GameObject collection will need
		// this typedef.
		typedef std::map<std::string, GameObject*> GameObjectMap;

		//---------------------------------------------------------------------
		// Public interface
		//---------------------------------------------------------------------
		GameObjectManager();
		~GameObjectManager();	

		GameObject* CreateGameObject(const std::string& p_strGameObjectDefinitionFile, const std::string& p_strGuid = "");
		GameObject* CreateGameObject(tinyxml2::XMLElement* p_pGameObjectRootElement, const std::string& p_strGuid = "");

		GameObject* GetGameObject(const std::string &p_strGameObjectGUID);

		void DestroyGameObject(GameObject* p_pGameObject);
		void DestroyAllGameObjects();

		void Update(const float p_fDelta);
		void PreRender();

		// Iterator access to allow outside traversal of GameObjects
		GameObjectMap::iterator		  begin()		{ return m_mGameObjectMap.begin(); }
		GameObjectMap::iterator		  end()			{ return m_mGameObjectMap.end(); }
		GameObjectMap::const_iterator begin() const { return m_mGameObjectMap.begin(); }
		GameObjectMap::const_iterator end()   const	{ return m_mGameObjectMap.end(); }

		bool empty() const { return m_mGameObjectMap.empty(); }

		//TODO: delete this (once all bugs are worked out)
		//Matt: just here to make sure all game objects loaded properly
		void Print();

	private:
		//---------------------------------------------------------------------
		// Private types
		//---------------------------------------------------------------------

		//---------------------------------------------------------------------
		// Private members
		//---------------------------------------------------------------------
		GameObjectMap m_mGameObjectMap;

		// Flags so we can move Destroying Game Objects to the end of the Update loop.
		bool m_bUpdating;
		std::set<GameObject*> m_vToDelete;

		unsigned int m_uiAutoGUIDNum;

		//---------------------------------------------------------------------
		// Private methods
		//---------------------------------------------------------------------
		// Prevent copying.
		GameObjectManager(const GameObjectManager&) = delete;
		GameObjectManager& operator=(const GameObjectManager) = delete;

		void LoadComponents(tinyxml2::XMLElement* p_pComponentsRootElement, GameObject* p_pGameObject);
		void LoadChildren(tinyxml2::XMLElement* p_pChildrenRootElement, GameObject* p_pParent);
		void ParseTags(tinyxml2::XMLElement* p_pTagsRootElement, GameObject* p_pGameObject);

		std::string GetGuid(tinyxml2::XMLElement* p_pGameObjectRootElement);
		std::string GetAutoGuid();
	};
} // namespace HeatStroke

#endif // GAMEOBJECTMANAGER_H