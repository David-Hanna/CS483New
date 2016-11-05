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
		m_fMaxReverseSpeedStat(6.0f),
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
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		return new ComponentKartController(
			p_pGameObject
			);
	}

	void ComponentKartController::Update(const float p_fDelta)
	{
		// Manually query for user input
		int iAccelerate, iBrake, iSlide;
		float fTurn;
		PlayerInputMapping::Instance()->QueryPlayerMovement(m_iPlayerNum, iAccelerate, iBrake, iSlide, fTurn);
		fTurn *= -1.0f; // Reversed because of mismatch between what the game and the controller consider to be the positive horizontal direction

		// Speeding up & slowing down
		UpdateSpeed(iAccelerate, iBrake, p_fDelta);

		// Turning
		UpdateTurn(fTurn, p_fDelta);

		// Hopping
		float fHeightMod = 0.0f;
		fHeightMod = UpdateHop(iSlide, p_fDelta);
		
		// Sliding
		UpdateSlide(iSlide, p_fDelta);

		// Transform
		UpdateTransform(fHeightMod);
	}

	void ComponentKartController::UpdateSpeed(int p_iAccelerateInput, int p_iBrakeInput, float p_fDelta)
	{
		// Determine modifier from turning
		float fSpeedModifer;
		if (!m_bSliding)
		{
			fSpeedModifer = 1.0f - ((1.0f - m_fSpeedWhileTurningStat) * (abs(m_fTurnSpeed) / m_fMaxTurnStat));
		}
		else
		{
			fSpeedModifer = m_fSpeedWhileSlidingMinStat - ((m_fSpeedWhileSlidingMinStat - m_fSpeedWhileSlidingMaxStat) * (abs(m_fTurnSpeed) / m_fMaxTurnStat));
		}

		// Adjust speed based on input
		if (p_iAccelerateInput != 0)
		{
			m_fSpeed += fmax((m_fMaxSpeedStat * m_fSpeedScale * fSpeedModifer - fmax(m_fSpeed, 0.0f)) * m_fAccelerationStat, -m_fSpeed * m_fAccelerationFrictionStat * 2.0f) * p_fDelta;
		}
		else if (p_iBrakeInput != 0)
		{

			m_fSpeed += fmin((-m_fMaxReverseSpeedStat * m_fSpeedScale * fSpeedModifer - fmin(m_fSpeed, 0.0f)) * m_fReverseAccelerationStat, -m_fSpeed * m_fAccelerationFrictionStat * 2.0f) * p_fDelta;
		}
		else
		{
			m_fSpeed -= m_fSpeed * m_fAccelerationFrictionStat * p_fDelta;
		}
	}

	void ComponentKartController::UpdateTurn(float p_fTurnInput, float p_fDelta)
	{
		// Determine slide direction, if sliding
		if (m_bSliding)
		{
			if (p_fTurnInput < 0 && m_iSlideDirection >= 0)
			{
				m_iSlideDirection = 1;
			}
			else if (p_fTurnInput > 0 && m_iSlideDirection <= 0)
			{
				m_iSlideDirection = -1;
			}
		}

		// Variables!
		float fTurnTarget = m_fMaxTurnStat * p_fTurnInput;
		float fModifier = 1.0f;

		// Adjust turn parameters if sliding
		if (m_bSliding)
		{
			fTurnTarget *= m_fSlideMaxTurnModifierStat;
			fModifier = m_fSlideModifierStat;
		}

		// Starighten out the turn a bit if moving fast (or very slow)
		if (m_fSpeed <= m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio)
		{
			fTurnTarget *= m_fSpeed / (m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio);
		}
		else
		{
			fTurnTarget *= m_fTurnAtMaxSpeedStat + (1.0f - m_fTurnAtMaxSpeedStat) * (((m_fMaxSpeedStat * m_fSpeedScale) - m_fSpeed) / ((m_fMaxSpeedStat * m_fSpeedScale) - (m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio)));
		}

		// Accelerate the current turn angle closer to the target
		if (fTurnTarget < m_fTurnSpeed)
		{
			m_fTurnSpeed += fmax(-m_fTurnAccelerationStat * fModifier * p_fDelta, fTurnTarget - m_fTurnSpeed);
		}
		else if (fTurnTarget > m_fTurnSpeed)
		{
			m_fTurnSpeed += fmin(m_fTurnAccelerationStat * fModifier * p_fDelta, fTurnTarget - m_fTurnSpeed);
		}

		// Adjust kart direction based on the turn angle
		m_fDirection += m_fTurnSpeed * p_fDelta;
		if (m_fDirection < 0)
		{
			m_fDirection += PI * 2.0f;
		}
		if (m_fDirection >= PI * 2.0f)
		{
			m_fDirection -= PI * 2.0f;
		}
	}

	float ComponentKartController::UpdateHop(int p_iSlideInput, float p_fDelta)
	{
		// How much the kart moves vertically this frame
		float fHeightMod = 0.0f;
		
		// If airborne, fall towards the ground and attempt to land
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

				// Landing
				if (p_iSlideInput != 0)
				{
					m_bSliding = true;
					m_iSlideDirection = 0;
				}
			}
		}
		// If not airborne, try to hop
		else if (p_iSlideInput != 0 && !m_bSliding)
		{
			m_fVerticalSpeed = m_fHopInitialSpeedStat * m_fVerticalSpeedScale;
			m_bAirborne = true;
		}

		return fHeightMod;
	}

	void ComponentKartController::UpdateSlide(int p_iSlideInput, float p_fDelta)
	{
		// End the slide if the input is released, or if speed drops too low
		if (m_bSliding)
		{
			if (m_fSpeed < m_fMaxSpeedStat * 0.2f * m_fSpeedScale)
			{
				m_bSliding = false;
				m_iSlideDirection = 0;
			}

			if (p_iSlideInput == 0)
			{
				m_bSliding = false;
				m_iSlideDirection = 0;

				// TODO: BOOST HERE
			}
		}
	}

	void ComponentKartController::UpdateTransform(float p_fHeightMod)
	{
		m_pGameObject->GetTransform().TranslateXYZ(m_fSpeed * sinf(m_fDirection), p_fHeightMod, m_fSpeed * cosf(m_fDirection));
		m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, m_fDirection, 0.0f)));

		//HeatStroke::HierarchicalTransform transform = m_pGameObject->GetTransform();
		//printf("Position:\n  X: %f\n  Y: %f\n  Z: %f\nRotation:\n  X: %f\n  Y: %f\n  Z: %f\nSpeed:\n  %f\nTurn speed:\n  %f\nVertical Speed:\n  %f\nSliding:\n  %i\nSlide direction:\n  %i\nTurn Modifier:\n  %f\n\n", transform.GetTranslation().x, transform.GetTranslation().y, transform.GetTranslation().z, transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, m_fSpeed, m_fTurnSpeed, m_fVerticalSpeed,m_bSliding, m_iSlideDirection, fSpeedModifer);
	}
}