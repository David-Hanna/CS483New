//------------------------------------------------------------------------
// GameObject
//
// Created:	2012/12/14
// Author:	Carel Boers
//	
// This class defines a "GameObject". An GameObject for our purpose is 
// essentially just an empty container to define any object in our game 
// world. The definition of the object is through the aggregation of various
// components.
//------------------------------------------------------------------------

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Component.h"
#include "GameObjectManager.h"
#include "HierarchicalTransform.h"

#include <map>
#include <set>

namespace HeatStroke
{
	class GameObjectManager;

	class GameObject
	{
		// Typedef for convenience
		typedef std::map<std::string, Component*> ComponentMap;
		typedef std::set<std::string> TagList;

		// Only GameObjectManager can create instances (private constructor/destructor)
		friend class GameObjectManager;

	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		const std::string& GetGUID() const			{ return m_strGUID; }
		HierarchicalTransform& GetTransform()		{ return m_Transform; }
		const HierarchicalTransform& GetTransform() const { return m_Transform; }
		GameObjectManager* GetManager()				{ return m_pGameObjectManager; }

		// Component management
		void AddComponent(Component* p_pComponent);
		Component* GetComponent(const std::string &p_strFamilyId) const;
		Component* RemoveComponent(const std::string &p_strFamilyId);
		void DeleteAllComponents();

		// Tag management
		void AddTag(const std::string& p_strTag)		{ m_mTagList.insert(p_strTag); }
		bool HasTag(const std::string& p_strTag) const	{ return m_mTagList.find(p_strTag) != m_mTagList.end(); }
		const TagList& GetTagList() const				{ return m_mTagList; }
		void RemoveTag(const std::string& p_strTag)		{ m_mTagList.erase(p_strTag); }
		void RemoveAllTags()							{ m_mTagList.clear(); }

		virtual void Init();
		virtual void Update(float p_fDelta);

	private:
		//------------------------------------------------------------------------------
		// Private methods.
		//------------------------------------------------------------------------------

		// Only GameObjectManager can create instances (private constructor/destructor)
		GameObject(GameObjectManager* p_pGameObjectManager, const std::string& p_strGUID);
		~GameObject();

		// Only GameObjectManager can set new GUID as it maintains the mapping of GUID -> GO
		void SetGUID(const std::string& p_strGUID) { m_strGUID = p_strGUID; }

	private:
		//------------------------------------------------------------------------------
		// Private members.
		//------------------------------------------------------------------------------

		// A pointer back to the GameObjectManager that created us
		GameObjectManager* m_pGameObjectManager;

		// GUID
		std::string m_strGUID;

		// All entities have a transform
		HierarchicalTransform m_Transform;

		// List of components
		ComponentMap m_mComponentMap;

		// List of tags
		TagList m_mTagList;
	};
}

#endif // GAMEOBJECT_H