#include "ComponentKartControllerAI.h"

#include <gtx/vector_angle.hpp>

namespace Kartaclysm
{
	ComponentKartControllerAI::ComponentKartControllerAI(HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_vGoalPos(p_pGameObject->GetTransform().GetTranslation())
	{

	}

	ComponentKartControllerAI::~ComponentKartControllerAI()
	{

	}

	HeatStroke::Component* ComponentKartControllerAI::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		return new ComponentKartControllerAI(p_pGameObject);
	}

	void ComponentKartControllerAI::Update(const float p_fDelta)
	{
		glm::vec3 vCurrentPos = m_pGameObject->GetTransform().GetTranslation();
		if (glm::distance(vCurrentPos, m_vGoalPos) <= 1.0f)
		{
			HeatStroke::GameObject* pTrack = m_pGameObject->GetManager()->GetGameObject("Track");
			ComponentTrack* pTrackComponent = static_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));
			ComponentRacer* pRacer = static_cast<ComponentRacer*>(m_pGameObject->GetComponent("GOC_Racer"));
			const HeatStroke::GameObject* pTrackPiece = pTrackComponent->GetNextTrackPiece(pRacer->GetCurrentTrackPiece());
			m_vGoalPos = pTrackPiece->GetTransform().GetTranslation();
		}

		glm::vec3 vPosDelta = glm::normalize(m_vGoalPos - vCurrentPos);
		glm::vec3 vForwardDir = glm::normalize(m_pGameObject->GetTransform().GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));
		float fAngle = glm::orientedAngle(vForwardDir, vPosDelta, glm::vec3(0.0f, 1.0f, 0.0f));
		m_pGameObject->GetTransform().Translate(vForwardDir * p_fDelta * 3.0f);
		m_pGameObject->GetTransform().Rotate(glm::vec3(0.0f, fAngle * p_fDelta * 3.0f, 0.0f));
	}
}