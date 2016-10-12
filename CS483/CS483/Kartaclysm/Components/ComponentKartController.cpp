//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#include "ComponentKartController.h"

#include "Common.h"
#include "GameObject.h"
#include "IO\KeyboardInputBuffer.h"

namespace Kartaclysm
{
	ComponentKartController::ComponentKartController(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fSpeedScale(0.001f),
		m_fMaxSpeedStat(10.0f),
		m_fMaxReverseSpeedStat(4.0f),
		m_fAccelerationStat(0.3f),
		m_fReverseAccelerationStat(0.6f),
		m_fAccelerationFrictionStat(1.2f),
		m_fTurningStat(2.0f),
		m_fSpeed(0.0f),
		m_fDirection(0.0f)
	{
	}

	ComponentKartController::~ComponentKartController()
	{
	}

	HeatStroke::Component* ComponentKartController::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults
		/*float fFOV = 45.0f;
		float fAspectRatioWidth = 1280.0f;
		float fAspectRatioHeight = 720.0f;
		float fNearClip = 0.1f;
		float fFarClip = 1000.0f;*/
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		// All parameters are optional. Some cameras may be attached "as is". (example_level.xml)
		if (p_pBaseNode != nullptr)
		{
			//ParseNode(p_pBaseNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}
		if (p_pOverrideNode != nullptr)
		{
			//ParseNode(p_pOverrideNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}

		// Apply the target if one was set.
		/*if (strTargetGUID != "")
		{
		pTargetGameObject = p_pGameObject->GetManager()->GetGameObject(strTargetGUID);
		}*/

		return new ComponentKartController(
			p_pGameObject/*,
			fFOV,
			fAspectRatioWidth / fAspectRatioHeight,
			fNearClip,
			fFarClip,
			pTargetGameObject*/
			);
	}

	void ComponentKartController::Update(const float p_fDelta)
	{

		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_UP))
		{
			m_fSpeed += fmax((m_fMaxSpeedStat * m_fSpeedScale - fmax(m_fSpeed, 0.0f)) * m_fAccelerationStat, -m_fSpeed * m_fAccelerationFrictionStat * 2.0f) * p_fDelta;
		}
		else if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_DOWN))
		{

			m_fSpeed += fmin((-m_fMaxReverseSpeedStat * m_fSpeedScale - fmin(m_fSpeed, 0.0f)) * m_fReverseAccelerationStat, -m_fSpeed * m_fAccelerationFrictionStat * 2.0f) * p_fDelta;
		}
		else
		{
			m_fSpeed -= m_fSpeed * m_fAccelerationFrictionStat * p_fDelta;
		}

		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_LEFT))
		{
			m_fDirection += m_fTurningStat * p_fDelta;

			if (m_fDirection >= PI * 2.0f)
			{
				m_fDirection -= PI * 2.0f;
			}
		}
		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_RIGHT))
		{
			m_fDirection += -m_fTurningStat * p_fDelta;

			if (m_fDirection < 0)
			{
				m_fDirection += PI * 2.0f;
			}
		}

		m_pGameObject->GetTransform().TranslateXYZ(m_fSpeed * cosf(m_fDirection), 0.0f, m_fSpeed * sinf(m_fDirection));
		m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, m_fDirection, 0.0f)));

		HeatStroke::HierarchicalTransform transform = m_pGameObject->GetTransform();
		printf("Position:\n  X: %f\n  Y: %f\n  Z: %f\nRotation:\n  X: %f\n  Y: %f\n  Z: %f\nSpeed:\n  %f\n\n", transform.GetTranslation().x, transform.GetTranslation().y, transform.GetTranslation().z, transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, m_fSpeed);
	}
}