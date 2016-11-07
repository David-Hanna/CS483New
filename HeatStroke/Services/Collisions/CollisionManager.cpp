//------------------------------------------------------------------------
// CollisionManager.cpp
// Author:	David MacIntosh
//	
// Manages the interactions between collider components.
//------------------------------------------------------------------------

#include "CollisionManager.h"

using namespace HeatStroke;

CollisionManager* CollisionManager::s_pCollisionManagerInstance = nullptr;

void CollisionManager::CreateInstance()
{
	assert(s_pCollisionManagerInstance == nullptr);
	s_pCollisionManagerInstance = new CollisionManager();
}

void CollisionManager::DestroyInstance()
{
	assert(s_pCollisionManagerInstance != nullptr);
	delete s_pCollisionManagerInstance;
	s_pCollisionManagerInstance = nullptr;
}

CollisionManager* CollisionManager::Instance()
{
	assert(s_pCollisionManagerInstance != nullptr);
	return s_pCollisionManagerInstance;
}

CollisionManager::CollisionManager()
	:
	m_mColliderMap()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::RegisterCollider(ComponentCollider* p_pCollider, std::string p_strGuid = "")
{
	m_mColliderMap.insert(std::pair<std::string, ComponentCollider*>(p_strGuid, p_pCollider));
}

void CollisionManager::Update(const float p_fDelta)
{
	// Iterate through each pair of colliders
	for (ColliderMap::iterator i = m_mColliderMap.begin(); i != m_mColliderMap.end(); i++)
	{
		for (ColliderMap::iterator j = i; j != m_mColliderMap.end(); j++)
		{
			if (i != j)
			{
				// CHECK COLLISION
			}
		}
	}
}

void CollisionManager::PreRender()
{
}