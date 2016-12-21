//----------------------------------------------------------------------------
// ComponentSimplePhysics.cpp
// Author: Bradley Cooper
//
// Component that handles simple physics for GameObjects.
//----------------------------------------------------------------------------

#include "ComponentSimplePhysics.h"

namespace Kartaclysm
{
	ComponentSimplePhysics::ComponentSimplePhysics(
		HeatStroke::GameObject* p_pGameObject,
		float p_fSpeed,
		float p_fFriction)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),

		m_fHeightAboveGroundStat(0.04f),
		m_fStickyHeightStat(0.2f),
		m_fVerticalSpeedScale(0.02f),
		m_fAccelerationFrictionStat(p_fFriction),
		m_fSimplePhysicsAccelerationStat(-12.0f),
		m_fOutsideForceAccelerationStat(0.92f),

		m_fGroundHeight(0.04f),
		m_fPreviousHeight(0.04f),
		m_fSpeed(p_fSpeed),
		m_fDirection(0.0f),
		m_bAirborne(false),
		m_fVerticalSpeed(0.0f),
		m_pOutsideForce(glm::vec3()),

		m_sPreviousWallCollision("")
	{
		m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentSimplePhysics::HandleCollisionEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);

		printf("ComponentSimplePhysics - made!\n");
	}

	ComponentSimplePhysics::~ComponentSimplePhysics()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Collision", m_pCollisionDelegate);
		delete m_pCollisionDelegate;
		m_pCollisionDelegate = nullptr;

		HeatStroke::Event* pEvent = new HeatStroke::Event("TrackHeightRegister");
		pEvent->SetGameObjectParameter("Unregister", GetGameObject()->GetGUID());
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

		printf("ComponentSimplePhysics - dead!\n");
	}

	HeatStroke::Component* ComponentSimplePhysics::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fSpeed;
		float fFriction;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fSpeed, fFriction);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fSpeed, fFriction);
		}

		return new ComponentSimplePhysics(
			p_pGameObject,
			fSpeed,
			fFriction
			);
	}

	void ComponentSimplePhysics::Init()
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("TrackHeightRegister");
		pEvent->SetGameObjectParameter("Register", GetGameObject()->GetGUID());
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentSimplePhysics::Update(const float p_fDelta)
	{
		// Speeding up & slowing down
		UpdateSpeed(p_fDelta);

		// Hopping
		float fHeightMod = UpdateHeight(p_fDelta);

		// Transform
		UpdateTransform(fHeightMod, p_fDelta);
	}

	void ComponentSimplePhysics::UpdateSpeed(float p_fDelta)
	{
		// Adjust speed based on friction
		if (!m_bAirborne)
		{
			m_fSpeed -= m_fSpeed * m_fAccelerationFrictionStat * p_fDelta;
		}
	}

	float ComponentSimplePhysics::UpdateHeight(float p_fDelta)
	{
		// How much the projectile moves vertically this frame
		float fHeightMod = 0.0f;
		float fTrackHeight = m_fGroundHeight + m_fHeightAboveGroundStat;

		// Handle changes in ground height
		if (!m_bAirborne && m_pGameObject->GetTransform().GetTranslation().y < fTrackHeight + m_fStickyHeightStat)
		{
			fHeightMod = fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y;
			m_bAirborne = false;
		}
		else if (m_pGameObject->GetTransform().GetTranslation().y > fTrackHeight + m_fStickyHeightStat)
		{
			// TODO: Make this (and a lot of other stuff) use delta time properly
			float heightDifference = m_pGameObject->GetTransform().GetTranslation().y - m_fPreviousHeight;

			m_fVerticalSpeed = heightDifference;
			m_bAirborne = true;
		}

		// If airborne, fall towards the ground and attempt to land
		if (m_bAirborne)
		{
			m_fVerticalSpeed += m_fSimplePhysicsAccelerationStat * m_fVerticalSpeedScale * p_fDelta;
			fHeightMod = m_fVerticalSpeed;

			if (fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y >= fHeightMod)
			{
				fHeightMod = fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y;
				m_fVerticalSpeed = 0.0f;
				//m_fSpeed *= 0.99f;
				m_bAirborne = false;
			}
		}

		m_fPreviousHeight = m_pGameObject->GetTransform().GetTranslation().y;

		return fHeightMod;
	}

	void ComponentSimplePhysics::UpdateTransform(float p_fHeightMod, float p_fDelta)
	{
		/*m_pGameObject->GetTransform().Translate(m_pOutsideForce);
		m_pOutsideForce *= m_fOutsideForceAccelerationStat;*/

		//float fSum = sinf(m_fDirection) + cosf(m_fDirection);
		m_pGameObject->GetTransform().TranslateXYZ(m_fSpeed * sinf(m_fDirection) * p_fDelta, p_fHeightMod, m_fSpeed * cosf(m_fDirection) * p_fDelta);
	}

	void ComponentSimplePhysics::HandleCollisionEvent(const HeatStroke::Event* p_pEvent)
	{
		std::string guid1, guid2;
		p_pEvent->GetRequiredGameObjectParameter("Object1GUID", guid1);
		p_pEvent->GetRequiredGameObjectParameter("Object2GUID", guid2);

		if (guid1.compare(m_pGameObject->GetGUID()) == 0 || guid2.compare(m_pGameObject->GetGUID()) == 0)
		{
			std::string other;
			if (guid1.compare(m_pGameObject->GetGUID()) == 0)
			{
				other = guid2;
			}
			else
			{
				other = guid1;
			}
			
			// Wall collisions
			if (m_pGameObject->GetManager()->GetGameObject(other)->HasTag("Wall") && other.compare(m_sPreviousWallCollision) != 0)
			{
				m_sPreviousWallCollision = other;
				
				/*glm::vec3 contactPoint = glm::vec3();
				p_pEvent->GetRequiredFloatParameter("ContactPointX", contactPoint.x);
				p_pEvent->GetRequiredFloatParameter("ContactPointY", contactPoint.y);
				p_pEvent->GetRequiredFloatParameter("ContactPointZ", contactPoint.z);*/

				//glm::vec3 velocity = glm::vec3(sinf(m_fDirection), 0.0f, cosf(m_fDirection));
				//glm::vec3 normal = glm::normalize(m_pGameObject->GetTransform().GetTranslation() - contactPoint);

				//glm::vec3 reflection = (-2.0f * ((glm::dot(velocity, normal)) * normal)) + velocity;

				//m_fDirection = asinf(reflection.x);
				//m_fDirection = atanf(reflection.z / reflection.x);

				int xAligned;
				int zAligned;
				p_pEvent->GetOptionalIntParameter("XAligned", xAligned, 0);
				p_pEvent->GetOptionalIntParameter("ZAligned", zAligned, 0);

				if (xAligned == 1)
				{
					m_fDirection = -m_fDirection;
				}
				else if (zAligned == 1)
				{
					/*m_fDirection += 3.141592f / 2.0f;
					m_fDirection = -m_fDirection;
					m_fDirection += 3.141592f / 2.0f;*/

					m_fDirection = 3.141592f - m_fDirection;
				}
			}

			/*HeatStroke::ComponentSphereCollider* collider = (HeatStroke::ComponentSphereCollider*) m_pGameObject->GetComponent("GOC_Collider");

			glm::vec3 difference = m_pGameObject->GetTransform().GetTranslation() - contactPoint;
			float distance = collider->GetRadius() - glm::length(difference);

			difference = glm::normalize(difference) * fmaxf(distance, 0.0000001f);
			m_pGameObject->GetTransform().Translate(difference);

			glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 1.0f);
			float dotProduct = glm::dot(velocity, glm::normalize(contactPoint - m_pGameObject->GetTransform().GetTranslation()));

			m_pOutsideForce = glm::normalize(difference) * m_fWallBumpStat * ((m_fSpeed / m_fSpeedScale) / m_fMaxSpeedStat) * dotProduct;
			m_fSpeed *= m_fWallSlowdownStat;*/
		}
	}

	void ComponentSimplePhysics::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fSpeed,
		float& p_fFriction)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_SimplePhysics") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Speed") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fSpeed);
			}
			else if (strcmp(szNodeName, "Friction") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fFriction);
			}
		}
	}
}