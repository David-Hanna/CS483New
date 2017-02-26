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

		// Path progress
		float d = sqrtf(powf(x - m_fXTarget, 2) + powf(z - m_fZTarget, 2));
		if (d <= m_sCurrentNode.radius)
		{
			NextNode();
		}

		// Update "inputs"
		glm::vec3 vPosDelta = glm::normalize(glm::vec3(m_fXTarget, 0.0f, m_fZTarget) - glm::vec3(x, 0.0f, z));
		glm::vec3 vForwardDir = glm::normalize(m_pGameObject->GetTransform().GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));
		float fAngle = glm::orientedAngle(vForwardDir, vPosDelta, glm::vec3(0.0f, 1.0f, 0.0f));
		float fAngleAbs = fabsf(fAngle);

		// Stop accelerating if you need to make a very tight turn
		if (fAngleAbs <= 1.2f)
		{
			m_iAccelerate = 1;
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
			m_fTurn = fmaxf(fminf(fAngle, 1.0f), -1.0f);
		}

		// Swerve if turning
		if (fAngleAbs <= 1.2f && fAngleAbs >= 0.5f && m_iSlide == 0)
		{
			m_iSlide = 1;
			m_iSlideDir = static_cast<int>(ceilf(m_fTurn));
		}
		else if (fAngleAbs <= 0.2f && m_iSlide == 1)
		{
			m_iSlide = 0;
		}
		else if ((m_iSlideDir > 0 && m_fTurn < 0) || (m_iSlideDir < 0 && m_fTurn > 0))
		{
			m_iSlide = 0;
		}
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

			m_sCurrentNode = pTrackComponent->GetNextNode(m_sCurrentNode.index);

			// Randomize target position
			float theta = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (PI * 2.0f)));

			float r = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (m_sCurrentNode.variation)));

			m_fXTarget = m_sCurrentNode.x + (cosf(theta) * r);
			m_fZTarget = m_sCurrentNode.z + (sinf(theta) * r);
		}
	}
}