//------------------------------------------------------------------------
// CollisionManager.h
// Author:	David MacIntosh
//	
// Manages the interactions between collider components.
//------------------------------------------------------------------------

#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <map>

#include "ComponentCollider.h"

#include "ComponentSphereCollider.h"
#include "ComponentWallCollider.h"

namespace HeatStroke
{
	enum ColliderType
	{
		Sphere,
		Wall
	};

	struct RegisteredCollider
	{
		ComponentCollider* collider;
		ColliderType type;
	};
	
	class CollisionManager
	{
	public:
		//---------------------------------------------------------------------
		// Public Types
		//---------------------------------------------------------------------

		// Any object which wants to iterate over the Collider collection will need
		// this typedef.
		//typedef std::map<std::string, ComponentCollider*> ColliderMap;
		typedef std::map<std::string, RegisteredCollider> ColliderMap;

		//---------------------------------------------------------------------
		// Public interface
		//---------------------------------------------------------------------
		static void CreateInstance();
		static void DestroyInstance();
		static CollisionManager* Instance();

		void RegisterCollider(ComponentCollider* p_pCollider, std::string p_strGuid);

		void Update(const float p_fDelta);
		void PreRender();

		// Iterator access to allow outside traversal of Colliders
		ColliderMap::iterator		  begin()		{ return m_mColliderMap.begin(); }
		ColliderMap::iterator		  end()			{ return m_mColliderMap.end(); }
		ColliderMap::const_iterator begin() const { return m_mColliderMap.begin(); }
		ColliderMap::const_iterator end()   const	{ return m_mColliderMap.end(); }

		bool empty() const { return m_mColliderMap.empty(); }

	private:
		//---------------------------------------------------------------------
		// Private members
		//---------------------------------------------------------------------
		static CollisionManager*	s_pCollisionManagerInstance;

		ColliderMap m_mColliderMap;

		//---------------------------------------------------------------------
		// Private functions
		//---------------------------------------------------------------------
		CollisionManager();
		~CollisionManager();

		void CheckCollision(RegisteredCollider p_sCollider1, RegisteredCollider p_sCollider2);

		// Collision pairs
		void CheckCollision(ComponentSphereCollider* p_pCollider1, ComponentSphereCollider* p_pCollider2);
		void CheckCollision(ComponentSphereCollider* p_pCollider1, ComponentWallCollider* p_pCollider2);
	};
}

#endif // COLLISION_MANAGER_H