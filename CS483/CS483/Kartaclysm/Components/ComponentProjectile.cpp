//----------------------------------------------------------------------------
// ComponentProjectile.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#include "ComponentProjectile.h"

#include "ComponentSphereCollider.h"

namespace Kartaclysm
{
	ComponentProjectile::ComponentProjectile(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_strOriginator(""),
		m_strEventName(""),

		m_fHeightAboveGroundStat(0.04f),
		m_fVerticalSpeedScale(0.02f),
		m_fAccelerationFrictionStat(2.0f),
		m_fGravityAccelerationStat(-12.0f),
		m_fOutsideForceAccelerationStat(0.92f),

		m_fGroundHeight(0.04f),
		m_fSpeed(2.0f),
		m_bAirborne(false),
		m_fVerticalSpeed(0.0f)
	{
		m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentProjectile::HandleCollisionEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);

		m_pOutsideForce = glm::vec3();
	}

	ComponentProjectile::~ComponentProjectile()
	{
	}

	HeatStroke::Component* ComponentProjectile::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		return new ComponentProjectile(
			p_pGameObject
			);
	}

	void ComponentProjectile::Update(const float p_fDelta)
	{
		// Speeding up & slowing down
		UpdateSpeed(p_fDelta);

		// Hopping
		float fHeightMod = UpdateHop(p_fDelta);

		// Transform
		UpdateTransform(fHeightMod);
	}

	void ComponentProjectile::UpdateSpeed(float p_fDelta)
	{
		// Adjust speed based on friction
		if (!m_bAirborne)
		{
			m_fSpeed -= m_fSpeed * m_fAccelerationFrictionStat * p_fDelta;
		}
	}

	float ComponentProjectile::UpdateHop(float p_fDelta)
	{
		// How much the kart moves vertically this frame
		float fHeightMod = 0.0f;
		float fTrackHeight = m_fGroundHeight + m_fHeightAboveGroundStat;

		// Handle changes in ground height
		if (m_pGameObject->GetTransform().GetTranslation().y < fTrackHeight)
		{
			fHeightMod = fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y;
			m_bAirborne = false;
		}
		else if (m_pGameObject->GetTransform().GetTranslation().y > fTrackHeight)
		{
			m_bAirborne = true;
		}

		// If airborne, fall towards the ground and attempt to land
		if (m_bAirborne)
		{
			m_fVerticalSpeed += m_fGravityAccelerationStat * m_fVerticalSpeedScale * p_fDelta;
			fHeightMod = m_fVerticalSpeed;

			if (fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y >= fHeightMod)
			{
				fHeightMod = fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y;
				m_fVerticalSpeed = 0.0f;
				m_fSpeed *= 0.99f;
				m_bAirborne = false;
			}
		}

		return fHeightMod;
	}

	void ComponentProjectile::UpdateTransform(float p_fHeightMod)
	{
		m_pGameObject->GetTransform().Translate(m_pOutsideForce);
		m_pOutsideForce *= m_fOutsideForceAccelerationStat;

		m_pGameObject->GetTransform().TranslateXYZ(0, p_fHeightMod, m_fSpeed);
	}

	void ComponentProjectile::HandleCollisionEvent(const HeatStroke::Event* p_pEvent)
	{
		std::string guid1, guid2;
		p_pEvent->GetRequiredGameObjectParameter("Object1GUID", guid1);
		p_pEvent->GetRequiredGameObjectParameter("Object2GUID", guid2);

		if (guid1.compare(m_pGameObject->GetGUID()) == 0 || guid2.compare(m_pGameObject->GetGUID()) == 0)
		{


			/*
			glm::vec3 contactPoint = glm::vec3();
			p_pEvent->GetRequiredFloatParameter("ContactPointX", contactPoint.x);
			p_pEvent->GetRequiredFloatParameter("ContactPointY", contactPoint.y);
			p_pEvent->GetRequiredFloatParameter("ContactPointZ", contactPoint.z);

			HeatStroke::ComponentSphereCollider* collider = (HeatStroke::ComponentSphereCollider*) m_pGameObject->GetComponent("GOC_Collider");

			glm::vec3 difference = m_pGameObject->GetTransform().GetTranslation() - contactPoint;
			float distance = collider->GetRadius() - glm::length(difference);

			difference = glm::normalize(difference) * fmaxf(distance, 0.0000001f);
			m_pGameObject->GetTransform().Translate(difference);

			glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 1.0f);
			float dotProduct = glm::dot(velocity, glm::normalize(contactPoint - m_pGameObject->GetTransform().GetTranslation()));

			m_pOutsideForce = glm::normalize(difference) * m_fWallBumpStat * ((m_fSpeed / m_fSpeedScale) / m_fMaxSpeedStat) * dotProduct;
			m_fSpeed *= m_fWallSlowdownStat;
			*/
		}
	}
}