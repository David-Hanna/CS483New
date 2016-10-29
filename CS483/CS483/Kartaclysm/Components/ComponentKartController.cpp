//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#include "ComponentKartController.h"

namespace Kartaclysm
{
	ComponentKartController::ComponentKartController(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_iPlayerNum(0), // TO DO, handle this number better

		m_fSpeedScale(0.002f),
		m_fVerticalSpeedScale(0.002f),
		m_fMaxSpeedStat(20.0f),
		m_fMaxReverseSpeedStat(4.0f),
		m_fAccelerationStat(2.0f),
		m_fReverseAccelerationStat(2.0f),
		m_fAccelerationFrictionStat(2.0f),
		m_fSpeedWhileTurningStat(0.8f),
		m_fSpeedWhileSlidingMinStat(0.95f),
		m_fSpeedWhileSlidingMaxStat(0.9f),
		m_fMaxTurnStat(0.8f),
		m_fTurnAccelerationStat(120.0f),
		m_fHopInitialSpeedStat(2.0f),
		m_fGravityAccelerationStat(-12.0f),
		m_fSlideModifierStat(0.006f),
		m_fSlideMaxTurnModifierStat(1.5f),
		m_fTurnAtMaxSpeedStat(0.8f),
		m_fPeakTurnRatio(0.2f),

		m_fGroundHeight(0.0f),
		m_fSpeed(0.0f),
		m_fDirection(0.0f),
		m_fTurnSpeed(0.0f),
		m_bAirborne(false),
		m_fVerticalSpeed(0.0f),
		m_bSliding(false),
		m_iSlideDirection(0)
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
		// Manually query for user input
		int iAccelerate, iBrake, iSlide;
		float fTurn;
		PlayerInputMapping::Instance()->QueryPlayerMovement(m_iPlayerNum, iAccelerate, iBrake, iSlide, fTurn);
		fTurn *= -1.0f; // TO DO, the calculations seem reversed...

		// Speeding up & slowing down

		float fSpeedModifer;
		if (!m_bSliding)
		{
			fSpeedModifer = 1.0f - ((1.0f - m_fSpeedWhileTurningStat) * (abs(m_fTurnSpeed) / m_fMaxTurnStat));
		}
		else
		{
			fSpeedModifer = m_fSpeedWhileSlidingMinStat - ((m_fSpeedWhileSlidingMinStat - m_fSpeedWhileSlidingMaxStat) * (abs(m_fTurnSpeed) / m_fMaxTurnStat));
		}

		if (iAccelerate != 0)
		{
			m_fSpeed += fmax((m_fMaxSpeedStat * m_fSpeedScale * fSpeedModifer - fmax(m_fSpeed, 0.0f)) * m_fAccelerationStat, -m_fSpeed * m_fAccelerationFrictionStat * 2.0f) * p_fDelta;
		}
		else if (iBrake != 0)
		{

			m_fSpeed += fmin((-m_fMaxReverseSpeedStat * m_fSpeedScale * fSpeedModifer - fmin(m_fSpeed, 0.0f)) * m_fReverseAccelerationStat, -m_fSpeed * m_fAccelerationFrictionStat * 2.0f) * p_fDelta;
		}
		else
		{
			m_fSpeed -= m_fSpeed * m_fAccelerationFrictionStat * p_fDelta;
		}

		// Turning

		if (m_bSliding)
		{
			if (fTurn < 0 && m_iSlideDirection >= 0)
			{
				m_iSlideDirection = 1;
			}
			else if (fTurn > 0 && m_iSlideDirection <= 0)
			{
				m_iSlideDirection = -1;
			}
		}

		float fTurnTarget = m_fMaxTurnStat * fTurn;
		float fModifier = 1.0f;

		if (m_bSliding)
		{
			fTurnTarget *= m_fSlideMaxTurnModifierStat;
			fModifier = m_fSlideModifierStat;
		}

		if (m_fSpeed <= m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio)
		{
			fTurnTarget *= m_fSpeed / (m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio);
		}
		else
		{
			fTurnTarget *= m_fTurnAtMaxSpeedStat + (1.0f - m_fTurnAtMaxSpeedStat) * (((m_fMaxSpeedStat * m_fSpeedScale) - m_fSpeed) / ((m_fMaxSpeedStat * m_fSpeedScale) - (m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio)));
		}

		if (fTurnTarget < m_fTurnSpeed)
		{
			m_fTurnSpeed += fmax(-m_fTurnAccelerationStat * fModifier * p_fDelta, fTurnTarget - m_fTurnSpeed);
		}
		else if (fTurnTarget > m_fTurnSpeed)
		{
			m_fTurnSpeed += fmin(m_fTurnAccelerationStat * fModifier * p_fDelta, fTurnTarget - m_fTurnSpeed);
		}

		m_fDirection += m_fTurnSpeed * p_fDelta;
		if (m_fDirection < 0)
		{
			m_fDirection += PI * 2.0f;
		}
		if (m_fDirection >= PI * 2.0f)
		{
			m_fDirection -= PI * 2.0f;
		}

		// Hopping

		float fHeightMod = 0.0f;

		if (m_bAirborne)
		{
			m_fVerticalSpeed += m_fGravityAccelerationStat * m_fVerticalSpeedScale * p_fDelta;
			fHeightMod = m_fVerticalSpeed;

			if (m_fGroundHeight - m_pGameObject->GetTransform().GetTranslation().y >= fHeightMod)
			{
				fHeightMod = m_fGroundHeight - m_pGameObject->GetTransform().GetTranslation().y;
				m_fVerticalSpeed = 0.0f;
				m_fSpeed *= 0.99f;
				m_bAirborne = false;

				// landing
				if (iSlide != 0)
				{
					m_bSliding = true;
					m_iSlideDirection = 0;
				}
			}
		}
		else if (iSlide != 0 && !m_bSliding)
		{
			m_fVerticalSpeed = m_fHopInitialSpeedStat * m_fVerticalSpeedScale;
			m_bAirborne = true;
		}
		
		// Sliding

		if (m_bSliding)
		{
			if (m_fSpeed < m_fMaxSpeedStat * 0.2f * m_fSpeedScale)
			{
				m_bSliding = false;
				m_iSlideDirection = 0;
			}

			if (iSlide == 0)
			{
				m_bSliding = false;
				m_iSlideDirection = 0;

				// BOOST HERE
			}
		}

		// Transform

		m_pGameObject->GetTransform().TranslateXYZ(m_fSpeed * sinf(m_fDirection), fHeightMod, m_fSpeed * cosf(m_fDirection));
		m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, m_fDirection, 0.0f)));

		HeatStroke::HierarchicalTransform transform = m_pGameObject->GetTransform();
		//printf("Position:\n  X: %f\n  Y: %f\n  Z: %f\nRotation:\n  X: %f\n  Y: %f\n  Z: %f\nSpeed:\n  %f\nTurn speed:\n  %f\nVertical Speed:\n  %f\nSliding:\n  %i\nSlide direction:\n  %i\nTurn Modifier:\n  %f\n\n", transform.GetTranslation().x, transform.GetTranslation().y, transform.GetTranslation().z, transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, m_fSpeed, m_fTurnSpeed, m_fVerticalSpeed,m_bSliding, m_iSlideDirection, fSpeedModifer);
	}
}