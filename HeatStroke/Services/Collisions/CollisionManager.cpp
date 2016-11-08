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
		printf("Sphere-Sphere Collision!");
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

	glm::vec3 c = pos1;
	glm::vec3 l1 = pos2 + (glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), p_pCollider2->GetSurfaceNormal()) * p_pCollider2->GetWidth());
	glm::vec3 l2 = pos2 + (glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), p_pCollider2->GetSurfaceNormal()) * -p_pCollider2->GetWidth());

	// Rearrange these points so that l1 is at origin, and the line is along the z-axis
	c = c - l1;
	l2 = l2 - l1;
	l1 = glm::vec3(0.0f, 0.0f, 0.0f);

	float rotAngle = glm::dot(glm::normalize(l2), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::vec3 rotAxis = glm::cross(glm::normalize(l2), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::quat rotation;
	if (rotAngle < -0.999f)
	{
		rotAxis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(l2));
		rotation = glm::angleAxis(180.0f, rotAxis);
	}
	else
	{
		float s = sqrtf((1 + rotAngle) * 2);
		float invs = 1.0f / s;
		rotation = glm::quat(s * 0.5f, rotAxis.x * invs, rotAxis.y * invs, rotAxis.z * invs);
	}

	c = c * rotation;
	l2 = l2 * rotation;

	if (l2.z < 0)
	{
		c.z *= -1.0f;
		l2.z *= -1.0f;
	}

	// Find the point on the line closest to the circle
	glm::vec2 nearPoint;
	if (c.z <= 0.0f)
	{
		nearPoint = glm::vec2(l1.x, l1.z);
	}
	else if (c.z >= l2.z)
	{
		nearPoint = glm::vec2(l2.x, l2.z);
	}
	else
	{
		nearPoint = glm::vec2(0.0f, c.z);
	}

	// See if the distance is less than the radius of the sphere
	if (glm::length(nearPoint - glm::vec2(c.x, c.z)) <= radius)
	{
		printf("Sphere-Wall Collision! ");
	}
}