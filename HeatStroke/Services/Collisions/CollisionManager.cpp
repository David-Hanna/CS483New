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
	//m_mColliderMap.insert(std::pair<std::string, ComponentCollider*>(p_strGuid, p_pCollider));

	RegisteredCollider collider;
	collider.collider = p_pCollider;

	if (p_pCollider->ComponentID().compare("GOC_SphereCollider") == 0)
	{
		collider.type = Sphere;
	}
	else if (p_pCollider->ComponentID().compare("GOC_WallCollider") == 0)
	{
		collider.type = Wall;
	}

	m_mColliderMap.insert(std::pair<std::string, RegisteredCollider>(p_strGuid, collider));
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

void CollisionManager::CheckCollision(RegisteredCollider p_sCollider1, RegisteredCollider p_sCollider2)
{
	// Make sure they're in order, to simplify comparisons

	if (p_sCollider1.type > p_sCollider2.type)
	{
		RegisteredCollider temp = p_sCollider1;
		p_sCollider1 = p_sCollider2;
		p_sCollider2 = temp;
	}

	// Now find the right function

	if (p_sCollider1.type == Sphere)
	{
		if (p_sCollider2.type == Sphere)
		{
			CheckCollision((ComponentSphereCollider*)p_sCollider1.collider, (ComponentSphereCollider*)p_sCollider2.collider);
		}
		else if (p_sCollider2.type == Wall)
		{
			CheckCollision((ComponentSphereCollider*)p_sCollider1.collider, (ComponentWallCollider*)p_sCollider2.collider);
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

	// Do an area check to see if we can exit out of this early
	float checkDistance = p_pCollider1->GetRadius() + p_pCollider2->GetWidth();
	if (pos1.x < pos2.x - checkDistance || pos1.x > pos2.x + checkDistance || pos1.z < pos2.z - checkDistance || pos1.z > pos2.z + checkDistance)
	{
		return;
	}

	// Previous sphere position
	// (the collider's position may be a frame behind, due to the order of Update()
	//		so we'll check and use the one that is most appropriate)
	glm::vec3 pos1prev = p_pCollider1->GetPosition();
	if (pos1prev == pos1)
	{
		pos1prev = p_pCollider1->GetPreviousPosition();
	}

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
	glm::quat rotation = p_pCollider2->GetGameObject()->GetTransform().GetRotation();
	normal = normal * rotation;
	glm::vec3 contactPoint;

	bool passedThrough = false;

	// Find the contact point
	if ((normal.x > 0.9f || normal.x < -0.9f) && abs(normal.z) < 0.1f)
	{
		glm::vec3 l1 = pos2 + glm::vec3(0.0f, 0.0f, p_pCollider2->GetWidth());
		glm::vec3 l2 = pos2 - glm::vec3(0.0f, 0.0f, p_pCollider2->GetWidth());

		// pass through
		if (pos1.x > pos2.x && pos1prev.x < pos2.x)
		{
			contactPoint = pos1 + glm::vec3(0.001f, 0.0f, 0.0f);
			passedThrough = true;
		}
		else if (pos1.x < pos2.x && pos1prev.x > pos2.x)
		{
			contactPoint = pos1 + glm::vec3(-0.001f, 0.0f, 0.0f);
			passedThrough = true;
		}
		// contact at the edges of the wall
		else if (pos1.z >= l1.z)
		{
			contactPoint = glm::vec3(l1.x, pos1.y, l1.z);
		}
		else if (pos1.z <= l2.z)
		{
			contactPoint = glm::vec3(l2.x, pos1.y, l2.z);;
		}
		// contact in the middle of the wall
		else
		{
			contactPoint = glm::vec3(pos2.x, pos1.y, pos1.z);
		}
	}
	else if ((normal.z > 0.9f || normal.z < -0.9f) && abs(normal.x) < 0.1f)
	{
		glm::vec3 l1 = pos2 + glm::vec3(p_pCollider2->GetWidth(), 0.0f, 0.0f);
		glm::vec3 l2 = pos2 - glm::vec3(p_pCollider2->GetWidth(), 0.0f, 0.0f);

		// pass through
		if (pos1.z > pos2.z && pos1prev.z < pos2.z)
		{
			contactPoint = pos1 + glm::vec3(0.0f, 0.0f, 0.001f);
			passedThrough = true;
		}
		else if (pos1.z < pos2.z && pos1prev.z > pos2.z)
		{
			contactPoint = pos1 + glm::vec3(0.0f, 0.0f, -0.001f);
			passedThrough = true;
		}
		// contact at the edges of the wall
		else if (pos1.x >= l1.x)
		{
			contactPoint = glm::vec3(l1.x, pos1.y, l1.z);
		}
		else if (pos1.x <= l2.x)
		{
			contactPoint = glm::vec3(l2.x, pos1.y, l2.z);;
		}
		// contact in the middle of the wall
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
		collisionEvent->SetIntParameter("PassedThrough", (int)passedThrough);
		EventManager::Instance()->TriggerEvent(collisionEvent);
	}
}