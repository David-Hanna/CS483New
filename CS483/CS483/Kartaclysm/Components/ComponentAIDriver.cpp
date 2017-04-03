//----------------------------------------------------------------------------
// ComponentAIDriver.cpp
// Author: David MacIntosh
//
// Component that uses AI to control a ComponentKartController
//----------------------------------------------------------------------------

#include "ComponentAIDriver.h"

#include <gtx/vector_angle.hpp>

namespace Kartaclysm
{
	ComponentAIDriver::ComponentAIDriver(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject)
	{
		m_sCurrentNode.x = m_pGameObject->GetTransform().GetTranslation().x;
		m_sCurrentNode.z = m_pGameObject->GetTransform().GetTranslation().z;
		m_sCurrentNode.variation = 0.0f;
		m_sCurrentNode.radius = 10.f;
		m_sCurrentNode.index = -1;

		m_fXTarget = m_sCurrentNode.x;
		m_fZTarget = m_sCurrentNode.z;

		m_vInitialNodeDir = glm::vec3(1.0f, 1.0f, 1.0f);

		m_fAngleToNextNode = 0.0f;
		m_fDistanceToNextNode = 0.0f;
	}

	ComponentAIDriver::~ComponentAIDriver()
	{
	}

	HeatStroke::Component* ComponentAIDriver::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		return new ComponentAIDriver(
			p_pGameObject
			);
	}

	void ComponentAIDriver::Update(const float p_fDelta)
	{
		float x = m_pGameObject->GetTransform().GetTranslation().x;
		float z = m_pGameObject->GetTransform().GetTranslation().z;

		// Update "inputs"
		glm::vec3 vPosDelta = glm::normalize(glm::vec3(m_fXTarget, 0.0f, m_fZTarget) - glm::vec3(x, 0.0f, z));
		glm::vec3 vForwardDir = glm::normalize(m_pGameObject->GetTransform().GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));
		float fAngle = glm::orientedAngle(vForwardDir, vPosDelta, glm::vec3(0.0f, 1.0f, 0.0f));
		m_fAngleToNextNode = fabsf(fAngle);

		// Path progress
		m_fDistanceToNextNode = sqrtf(powf(x - m_fXTarget, 2) + powf(z - m_fZTarget, 2));
		float fAngleFromInitialPos = fabsf(glm::orientedAngle(vPosDelta, m_vInitialNodeDir, glm::vec3(0.0f, 1.0f, 0.0f)));
		if (m_fDistanceToNextNode <= m_sCurrentNode.radius || fAngleFromInitialPos >= 1.57f)
		{
			NextNode();
		}

		// Stop accelerating if you need to make a very tight turn
		if (m_fAngleToNextNode <= 1.0f)
		{
			m_iAccelerate = 1;
			m_iBrake = 0;
		}
		else if(m_fAngleToNextNode <= 1.2f)
		{
			m_iAccelerate = 0;
			m_iBrake = 0;
		}
		else
		{
			m_iAccelerate = 0;
			m_iBrake = 1;
		}

		if (fAngle > PI / 6.0f)
		{
			m_fTurn = 1.0f;
		}
		else if (fAngle < -PI / 6.0f)
		{
			m_fTurn = -1.0f;
		}
		else
		{
			float fTurnMagnitude = fminf(fabsf(fAngle) * (1.0f + (fabsf(fAngle) * 2.0f)), 1.0f);
			m_fTurn = fTurnMagnitude * (fAngle >= 0.0f ? 1.0f : -1.0f);
		}

		// Swerve if turning
		ComponentKartController* pKartController = static_cast<ComponentKartController*>(m_pGameObject->GetComponent("GOC_KartController"));
		if (pKartController != nullptr)
		{
			if (pKartController->IsOffroad())
			{
				m_iSlide = 0;
			}
			else
			{
				if (m_fAngleToNextNode <= 1.0f && m_fAngleToNextNode >= 0.4f && m_fDistanceToNextNode >= 4.0f && m_iSlide == 0)
				{
					m_iSlide = 1;
					m_iSlideDir = static_cast<int>(ceilf(m_fTurn));
				}
				else if (m_fAngleToNextNode <= 0.1f && m_iSlide == 1)
				{
					m_iSlide = 0;
				}
				else if ((m_iSlideDir > 0 && m_fTurn < 0) || (m_iSlideDir < 0 && m_fTurn > 0))
				{
					m_iSlide = 0;
				}
			}
		}

		// Use abilites
		for (unsigned int i = 0; i < m_vAbilities.size(); i++)
		{
			m_vAbilities.at(i)->AICheckCondition(this);
		}
	}

	int ComponentAIDriver::CurrentPosition()
	{
		int iPosition = 0;

		ComponentRacer* pRacer = static_cast<ComponentRacer*>(m_pGameObject->GetComponent("GOC_Racer"));
		if (pRacer != nullptr)
		{
			iPosition = pRacer->GetCurrentPosition();
		}

		return iPosition;
	}

	bool ComponentAIDriver::IsInWheelie()
	{
		bool bWheelie = false;

		ComponentKartController* pKartController = static_cast<ComponentKartController*>(m_pGameObject->GetComponent("GOC_KartController"));
		if (pKartController != nullptr)
		{
			bWheelie = pKartController->IsInWheelie();
		}

		return bWheelie;
	}

	float ComponentAIDriver::DistanceToClosestOpponent()
	{
		float fLowestDistance = 9999.9f;

		float x = GetGameObject()->GetTransform().GetTranslation().x;
		float z = GetGameObject()->GetTransform().GetTranslation().z;

		float fDistance;

		float xx;
		float zz;

		std::vector<HeatStroke::GameObject*> vRacers = GetGameObject()->GetManager()->GetGameObjectsByTag("Racer");
		for (unsigned int i = 0; i < vRacers.size(); i++)
		{
			if (vRacers.at(i)->GetGUID().compare(GetGameObject()->GetGUID()) != 0)
			{
				xx = vRacers.at(i)->GetTransform().GetTranslation().x;
				zz = vRacers.at(i)->GetTransform().GetTranslation().z;

				fDistance = sqrtf(powf(x - xx, 2) + powf(z - zz, 2));

				if (fDistance < fLowestDistance)
				{
					fLowestDistance = fDistance;
				}
			}
		}

		return fLowestDistance;
	}

	void ComponentAIDriver::QueryPlayerMovement(
		const int p_iPlayerNum,
		int& p_iAccelerate,
		int& p_iBrake,
		int& p_iSlide,
		float& p_fTurn) const
	{
		p_iAccelerate = m_iAccelerate;
		p_iBrake = m_iBrake;
		p_fTurn = m_fTurn;
		p_iSlide = m_iSlide;
	}

	void ComponentAIDriver::NextNode()
	{
		ComponentKartController* kartController = static_cast<ComponentKartController*>(m_pGameObject->GetComponent("GOC_KartController"));
		if (kartController != nullptr && !kartController->IsAirborne()) // Check if airborne so that jumps don't fuck up the pathing
		{
			HeatStroke::GameObject* pTrack = m_pGameObject->GetManager()->GetGameObject("Track");
			ComponentTrack* pTrackComponent = static_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));

			SetNode(pTrackComponent->GetNextNode(m_sCurrentNode.index));
		}
	}

	void ComponentAIDriver::SetNode(ComponentTrack::PathNode p_sNode)
	{
		if (p_sNode.index != m_sCurrentNode.index)
		{
			m_sCurrentNode = p_sNode;

			// Randomize target position
			float theta = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (PI * 2.0f)));

			float r = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (m_sCurrentNode.variation)));

			m_fXTarget = m_sCurrentNode.x + (cosf(theta) * r);
			m_fZTarget = m_sCurrentNode.z + (sinf(theta) * r);

			m_vInitialNodeDir = glm::normalize(glm::vec3(m_fXTarget, 0.0f, m_fZTarget) - glm::vec3(m_pGameObject->GetTransform().GetTranslation().x, 0.0f, m_pGameObject->GetTransform().GetTranslation().z));
		}
	}

	void ComponentAIDriver::RegisterComponentAbility(ComponentAbility* p_pAbility)
	{
		m_vAbilities.push_back(p_pAbility);
	}
}