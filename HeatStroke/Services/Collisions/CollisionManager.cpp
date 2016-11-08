//------------------------------------------------------------------------
// CollisionManager.cpp
// Author:	David MacIntosh
//	
// Manages the interactions between collider components.
//------------------------------------------------------------------------

#include "CollisionManager.h"

#include "EventManager.h"

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
				CheckCollision(i->second, j->second);
			}
		}
	}
}

void CollisionManager::PreRender()
{
}

void CollisionManager::CheckCollision(ComponentCollider* p_pCollider1, ComponentCollider* p_pCollider2)
{
	// The structure of this function is actually AIDS but I couldn't think of a better way
	
	ComponentCollider* pFirst;
	ComponentCollider* pSecond;

	if (p_pCollider1->ComponentID().compare("GOC_SphereCollider") == 0 || p_pCollider2->ComponentID().compare("GOC_SphereCollider") == 0)
	{
		if (p_pCollider1->ComponentID().compare("GOC_SphereCollider") == 0)
		{
			pFirst = p_pCollider1;
			pSecond = p_pCollider2;
		}
		else
		{
			pFirst = p_pCollider2;
			pSecond = p_pCollider1;
		}

		if (pSecond->ComponentID().compare("GOC_SphereCollider") == 0)
		{
			CheckCollision((ComponentSphereCollider*)pFirst, (ComponentSphereCollider*)pSecond);
		}
		if (pSecond->ComponentID().compare("GOC_WallCollider") == 0)
		{
			CheckCollision((ComponentSphereCollider*)pFirst, (ComponentWallCollider*)pSecond);
		}
	}
}

void CollisionManager::CheckCollision(ComponentSphereCollider* p_pCollider1, ComponentSphereCollider* p_pCollider2)
{
	// Sphere-Sphere Collision
	// Pretty easy, just check if the distance between them is less than the sum of their radii.
	
	glm::vec3 pos1 = p_pCollider1->GetGameObject()->GetTransform().GetTranslation() + p_pCollider1->GetOffset();
	glm::vec3 pos2 = p_pCollider2->GetGameObject()->GetTransform().GetTranslation() + p_pCollider2->GetOffset();

	if (glm::length(pos1 - pos2) <= p_pCollider1->GetRadius() + p_pCollider2->GetRadius())
	{
		glm::vec3 contactPoint = pos1 - (pos2 * (p_pCollider1->GetRadius()/(p_pCollider1->GetRadius() + p_pCollider2->GetRadius())));

		//printf("Sphere-Sphere Collision!");
		Event* collisionEvent = new Event("Collision");
		collisionEvent->SetGameObjectParameter("Object1GUID", p_pCollider1->GetGameObject()->GetGUID());
		collisionEvent->SetGameObjectParameter("Object2GUID", p_pCollider2->GetGameObject()->GetGUID());
		collisionEvent->SetFloatParameter("ContactPointX", contactPoint.x);
		collisionEvent->SetFloatParameter("ContactPointY", contactPoint.y);
		collisionEvent->SetFloatParameter("ContactPointZ", contactPoint.z);
		EventManager::Instance()->TriggerEvent(collisionEvent);
	}
}

void CollisionManager::CheckCollision(ComponentSphereCollider* p_pCollider1, ComponentWallCollider* p_pCollider2)
{
	// Sphere-Wall Collision
	// This is a little different; because the walls are always vertical,
	// we can reduce this to a 2D line-circle intersection problem
	
	glm::vec3 pos1 = p_pCollider1->GetGameObject()->GetTransform().GetTranslation() + p_pCollider1->GetOffset();
	glm::vec3 pos2 = p_pCollider2->GetGameObject()->GetTransform().GetTranslation() + p_pCollider2->GetOffset();

	float heightDifference = abs(pos1.y - pos2.y);
	float radius;

	// If the sphere is well above/below the wall, we know they won't collide.
	// If it's only a little above/below, it may still
	if (heightDifference <= p_pCollider2->GetHeight())
	{
		radius = p_pCollider1->GetRadius();
	}
	else if (heightDifference <= p_pCollider2->GetHeight() + p_pCollider1->GetRadius())
	{
		float h = (p_pCollider2->GetHeight() + p_pCollider1->GetRadius()) - heightDifference;
		radius = sqrtf(h * ((2 * p_pCollider1->GetRadius()) - h));
	}
	else
	{
		return;
	}

	glm::vec3 normal = p_pCollider2->GetSurfaceNormal();
	glm::vec3 contactPoint;

	// Find the contact point
	if ((normal.x == 1.0f || normal.x == -1.0f) && normal.z == 0.0f)
	{
		glm::vec3 l1 = pos2 + glm::vec3(0.0f, 0.0f, p_pCollider2->GetWidth());
		glm::vec3 l2 = pos2 - glm::vec3(0.0f, 0.0f, p_pCollider2->GetWidth());

		if (pos1.z >= l1.z)
		{
			contactPoint = glm::vec3(l1.x, pos1.y, l1.z);
		}
		else if (pos1.z <= l2.z)
		{
			contactPoint = glm::vec3(l2.x, pos1.y, l2.z);;
		}
		else
		{
			contactPoint = glm::vec3(pos2.x, pos1.y, pos1.z);
		}
	}
	else if ((normal.z == 1.0f || normal.z == -1.0f) && normal.x == 0.0f)
	{
		glm::vec3 l1 = pos2 + glm::vec3(p_pCollider2->GetWidth(), 0.0f, 0.0f);
		glm::vec3 l2 = pos2 - glm::vec3(p_pCollider2->GetWidth(), 0.0f, 0.0f);

		if (pos1.x >= l1.x)
		{
			contactPoint = glm::vec3(l1.x, pos1.y, l1.z);
		}
		else if (pos1.x <= l2.x)
		{
			contactPoint = glm::vec3(l2.x, pos1.y, l2.z);;
		}
		else
		{
			contactPoint = glm::vec3(pos1.x, pos1.y, pos2.z);
		}
	}
	else
	{
		assert(false && "Walls must be axis aligned!");
		return;
	}

	// See if we collide
	if (glm::length(pos1 - contactPoint) <= radius)
	{
		//printf("Sphere-Wall Collision!\n");
		//printf("%f, %f, %f\n", contactPoint.x, contactPoint.y, contactPoint.z);
		Event* collisionEvent = new Event("Collision");
		collisionEvent->SetGameObjectParameter("Object1GUID", p_pCollider1->GetGameObject()->GetGUID());
		collisionEvent->SetGameObjectParameter("Object2GUID", p_pCollider2->GetGameObject()->GetGUID());
		collisionEvent->SetFloatParameter("ContactPointX", contactPoint.x);
		collisionEvent->SetFloatParameter("ContactPointY", contactPoint.y);
		collisionEvent->SetFloatParameter("ContactPointZ", contactPoint.z);
		EventManager::Instance()->TriggerEvent(collisionEvent);
	}
}