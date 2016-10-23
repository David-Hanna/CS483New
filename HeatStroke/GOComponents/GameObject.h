//------------------------------------------------------------------------
// GameObject
//
// Created:	2012/12/14
// Author:	Carel Boers
// Editor:	David Hanna
// Editor:	Matthew White
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

namespace HeatStroke
{
    class GameObjectManager;
    
	class GameObject
	{
		// Typedef for convenience
		typedef std::map<std::string, Component*> ComponentMap;
		typedef std::map<std::string, GameObject*> ChildMap;

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

		void AddComponent(Component* p_pComponent);
		Component* GetComponent(const std::string &p_strFamilyId) const;
		Component* RemoveComponent(const std::string &p_strFamilyId);
		void DeleteAllComponents();

		void AddChild(GameObject* p_pChild);
		GameObject* RemoveChild(const std::string& p_strChildGuid);
		GameObject* GetChild(const std::string& p_strChildGuid);
		void DeleteAllChildren();

		virtual void Init();
		virtual void Update(float p_fDelta);

		//TODO: delete this
		//Matt: just using to show that children are being added properly
		void Print();
		
	private:
		//------------------------------------------------------------------------------
		// Private methods.
		//------------------------------------------------------------------------------

		// Only GameObjectManager can create instances (private constructor/destructor)
		GameObject(GameObjectManager* p_pGameObjectManager, const std::string& p_strGUID);
		~GameObject();

		// Only GameObjectManager can set new GUID as it maintains the mapping of GUID -> GO
		void SetGUID(const std::string& p_strGUID) { m_strGUID = p_strGUID; }

		void SetParent(GameObject* p_pParent);

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

		GameObject* m_pParent;
		ChildMap m_mChildMap;
	};
}

#endif // GAMEOBJECT_H