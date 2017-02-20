//----------------------------------------------------------------------------
// ComponentAIDriver.cpp
// Author: David MacIntosh
//
// Component that uses AI to control a ComponentKartController
//----------------------------------------------------------------------------

#include "ComponentAIDriver.h"

#include "ComponentTrack.h"
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
		m_sCurrentNode.radius = 10.f;
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

		float d = sqrtf(powf(x - m_sCurrentNode.x, 2) + powf(z - m_sCurrentNode.z, 2));

		if (d <= m_sCurrentNode.radius)
		{
			NextNode();
		}
	}

	void ComponentAIDriver::QueryPlayerMovement(
		const int p_iPlayerNum,
		int& p_iAccelerate,
		int& p_iBrake,
		int& p_iSlide,
		float& p_fTurn) const
	{
		float x = m_pGameObject->GetTransform().GetTranslation().x;
		float z = m_pGameObject->GetTransform().GetTranslation().z;

		glm::vec3 vPosDelta = glm::normalize(glm::vec3(m_sCurrentNode.x, 0.0f, m_sCurrentNode.z) - glm::vec3(x, 0.0f, z));
		glm::vec3 vForwardDir = glm::normalize(m_pGameObject->GetTransform().GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));
		float fAngle = glm::orientedAngle(vForwardDir, vPosDelta, glm::vec3(0.0f, 1.0f, 0.0f));

		// Stop accelerating if you need to make a tight turn
		if (fabsf(fAngle) <= 0.5f)
		{
			p_iAccelerate = 1;
		}
		else
		{
			p_iAccelerate = 0;
		}
		p_fTurn = fmaxf(fminf(fAngle, 1.0f), -1.0f);
	}

	void ComponentAIDriver::NextNode()
	{
		ComponentKartController* kartController = static_cast<ComponentKartController*>(m_pGameObject->GetComponent("GOC_KartController"));
		if (kartController != nullptr && !kartController->IsAirborne()) // Check if airborne so that jumps don't fuck up the pathing
		{
			HeatStroke::GameObject* pTrack = m_pGameObject->GetManager()->GetGameObject("Track");
			ComponentTrack* pTrackComponent = static_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));
			ComponentRacer* pRacer = static_cast<ComponentRacer*>(m_pGameObject->GetComponent("GOC_Racer"));
			const HeatStroke::GameObject* pTrackPiece = pTrackComponent->GetNextTrackPiece(pRacer->GetCurrentTrackPiece());

			PathNode nextNode;
			nextNode.x = pTrackPiece->GetTransform().GetTranslation().x;
			nextNode.z = pTrackPiece->GetTransform().GetTranslation().z;
			nextNode.radius = 8.0f;

			m_sCurrentNode = nextNode;
		}
	}
}