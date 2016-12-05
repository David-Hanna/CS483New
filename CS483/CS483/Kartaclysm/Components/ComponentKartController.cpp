//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#include "ComponentKartController.h"

#include "ComponentSphereCollider.h"

namespace Kartaclysm
{
	ComponentKartController::ComponentKartController(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_iPlayerNum(0), // TO DO, handle this number better
		m_strHitCallback(""),

		m_fHeightAboveGroundStat(0.04f),
		m_fSpeedScale(0.024f),
		m_fVerticalSpeedScale(0.02f),
		m_fMaxSpeedStat(20.0f),
		m_fMaxReverseSpeedStat(6.0f),
		m_fAccelerationStat(1.2f),
		m_fReverseAccelerationStat(1.2f),
		m_fAccelerationFrictionStat(2.0f),
		m_fSpeedWhileTurningStat(0.8f),
		m_fSpeedWhileSlidingMinStat(0.95f),
		m_fSpeedWhileSlidingMaxStat(0.9f),
		m_fMaxTurnStat(0.8f),
		m_fTurnAccelerationStat(120.0f),
		m_fHopInitialSpeedStat(2.5f),
		m_fGravityAccelerationStat(-12.0f),
		m_fSlideModifierStat(0.006f),
		m_fSlideMaxTurnModifierStat(1.5f),
		m_fTurnAtMaxSpeedStat(0.8f),
		m_fPeakTurnRatio(0.2f),
		m_fSwerveTurnModifier(0.35f),
		m_fSwerveAccelerationStat(2.0f),
		m_fWallBumpStat(0.15f),
		m_fWallSlowdownStat(0.3f),
		m_fOutsideForceAccelerationStat(0.92f),
		m_fSlideChargeAccelerationStat(0.5f),
		m_fSlideChargeMaxStat(1.0f),
		m_fSlideChargeThreshold(0.2f),

		m_fGroundHeight(0.04f),
		m_fSpeed(0.0f),
		m_fDirection(0.0f),
		m_fTurnSpeed(0.0f),
		m_bAirborne(false),
		m_fVerticalSpeed(0.0f),
		m_bSliding(false),
		m_iSlideDirection(0),
		m_fSwerve(0.0f),
		m_fSlideCharge(0.0f)
	{
		m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentKartController::HandleCollisionEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);

		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentKartController::HandleAbilityEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("AbilityUse", m_pAbilityDelegate);

		m_pOutsideForce = glm::vec3();
	}

	ComponentKartController::~ComponentKartController()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Collision", m_pCollisionDelegate);
		delete m_pCollisionDelegate;
		m_pCollisionDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener("AbilityUse", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
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
		// Variables!
		float fTurnTarget = m_fMaxTurnStat * p_fTurnInput;
		float fModifier = 1.0f;

		// Determine slide direction and adjust turn parameters, if sliding
		if (m_bSliding)
		{
			if (p_fTurnInput < 0 && m_iSlideDirection == 0)
			{
				m_iSlideDirection = -1;
			}
			else if (p_fTurnInput > 0 && m_iSlideDirection == 0)
			{
				m_iSlideDirection = 1;
			}

			fTurnTarget *= m_fSlideMaxTurnModifierStat;
			fModifier = m_fSlideModifierStat;

			if ((m_iSlideDirection > 0 && fTurnTarget < 0) || (m_iSlideDirection < 0 && fTurnTarget > 0))
			{
				fTurnTarget = 0.0f;
			}
		}

		// Straighten out the turn a bit if moving fast (or very slow)
		if (m_fSpeed <= m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio)
		{
			// TODO: Seeing how it feels with this disabled
			//fTurnTarget *= m_fSpeed / (m_fMaxSpeedStat * m_fSpeedScale * m_fPeakTurnRatio);
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

		// Update slide charge
		if (m_bSliding)
		{
			m_fSlideCharge += abs(m_fTurnSpeed * m_fSlideChargeAccelerationStat * p_fDelta);

			if (m_fSlideCharge > m_fSlideChargeMaxStat)
			{
				m_fSlideCharge = m_fSlideChargeMaxStat;
			}
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

		// Swerve
		float fSwerveTarget = m_fTurnSpeed * m_fSwerveTurnModifier;

		if (m_bAirborne || m_bSliding)
		{
			fSwerveTarget *= 1.5f;
		}

		if (fSwerveTarget < m_fSwerve)
		{
			m_fSwerve += fmax(-m_fSwerveAccelerationStat * p_fDelta, fSwerveTarget - m_fSwerve);
		}
		else if (fSwerveTarget > m_fSwerve)
		{
			m_fSwerve += fmin(m_fSwerveAccelerationStat * p_fDelta, fSwerveTarget - m_fSwerve);
		}
	}

	float ComponentKartController::UpdateHop(int p_iSlideInput, float p_fDelta)
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

				// Landing
				if (p_iSlideInput != 0)
				{
					m_bSliding = true;
					m_iSlideDirection = 0;
					m_fSlideCharge = 0.0f;
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
				m_fSlideCharge = 0.0f;
			}

			if (p_iSlideInput == 0)
			{
				m_bSliding = false;
				m_iSlideDirection = 0;

				if (m_fSlideCharge >= m_fSlideChargeThreshold)
				{
					Boost(1.0f + m_fSlideCharge);
				}

				m_fSlideCharge = 0.0f;
			}
		}
	}

	void ComponentKartController::UpdateTransform(float p_fHeightMod)
	{
		m_pGameObject->GetTransform().Translate(m_pOutsideForce);
		m_pOutsideForce *= m_fOutsideForceAccelerationStat;
		
		m_pGameObject->GetTransform().TranslateXYZ(m_fSpeed * sinf(m_fDirection), p_fHeightMod, m_fSpeed * cosf(m_fDirection));
		m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, m_fDirection + m_fSwerve, 0.0f)));

		//HeatStroke::HierarchicalTransform transform = m_pGameObject->GetTransform();
		//printf("Position:\n  X: %f\n  Y: %f\n  Z: %f\nRotation:\n  X: %f\n  Y: %f\n  Z: %f\nSpeed:\n  %f\nTurn speed:\n  %f\nVertical Speed:\n  %f\nSliding:\n  %i\nSlide direction:\n  %i\n\n", transform.GetTranslation().x, transform.GetTranslation().y, transform.GetTranslation().z, transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, m_fSpeed, m_fTurnSpeed, m_fVerticalSpeed,m_bSliding, m_iSlideDirection);
	}

	void ComponentKartController::Boost(float p_fPower)
	{
		if (m_fSpeed < m_fMaxSpeedStat * m_fSpeedScale * 0.5f)
		{
			m_fSpeed = m_fMaxSpeedStat * m_fSpeedScale * 0.5f;
		}

		float extra = (m_fMaxSpeedStat * m_fSpeedScale * p_fPower) - m_fSpeed;
		m_fSpeed = fmaxf(m_fSpeed, m_fSpeed + (extra * (m_fSpeed / (m_fMaxSpeedStat * m_fSpeedScale))));
	}

	glm::quat ComponentKartController::GetRotationMinusSwerve()
	{
		m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, m_fDirection, 0.0f)));

		glm::quat result = m_pGameObject->GetTransform().GetRotation();

		m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, m_fDirection + m_fSwerve, 0.0f)));

		return result;
	}

	void ComponentKartController::HandleCollisionEvent(const HeatStroke::Event* p_pEvent)
	{
		std::string guid1;
		std::string guid2;
		p_pEvent->GetRequiredGameObjectParameter("Object1GUID", guid1);
		p_pEvent->GetRequiredGameObjectParameter("Object2GUID", guid2);

		if (guid1.compare(m_pGameObject->GetGUID()) == 0 || guid2.compare(m_pGameObject->GetGUID()) == 0)
		{
			glm::vec3 contactPoint = glm::vec3();
			p_pEvent->GetRequiredFloatParameter("ContactPointX", contactPoint.x);
			p_pEvent->GetRequiredFloatParameter("ContactPointY", contactPoint.y);
			p_pEvent->GetRequiredFloatParameter("ContactPointZ", contactPoint.z);

			HeatStroke::ComponentSphereCollider* collider = (HeatStroke::ComponentSphereCollider*) m_pGameObject->GetComponent("GOC_Collider");

			glm::vec3 difference = m_pGameObject->GetTransform().GetTranslation() - contactPoint;
			float distance = collider->GetRadius() - glm::length(difference);

			difference = glm::normalize(difference) * fmaxf(distance, 0.0000001f);
			m_pGameObject->GetTransform().Translate(difference);

			glm::vec3 velocity = glm::vec3(sinf(m_fDirection), 0.0f, cosf(m_fDirection));
			float dotProduct = glm::dot(velocity, glm::normalize(contactPoint - m_pGameObject->GetTransform().GetTranslation()));

			m_pOutsideForce = glm::normalize(difference) * m_fWallBumpStat * ((m_fSpeed / m_fSpeedScale) / m_fMaxSpeedStat) * dotProduct;
			m_fSpeed *= m_fWallSlowdownStat;
		}
	}

	void ComponentKartController::HandleAbilityEvent(const HeatStroke::Event* p_pEvent)
	{
		// TODO: make this use game objects (brad, ya dingus)
		std::string originator;
		p_pEvent->GetRequiredStringParameter("Originator", originator);

		if (originator.compare("Player" + std::to_string(m_iPlayerNum)) == 0)
		{
			// TODO: make this use enums or something
			std::string ability;
			p_pEvent->GetRequiredStringParameter("Ability", ability);

			if (ability.compare("Boost") == 0)
			{
				float fPower;
				p_pEvent->GetRequiredFloatParameter("Power", fPower);

				Boost(fPower);
			}
			else if (ability.compare("Wheelie") == 0)
			{
				float fPower, fDuration;
				p_pEvent->GetRequiredFloatParameter("Power", fPower);
				p_pEvent->GetRequiredFloatParameter("Duration", fDuration);

				// TODO: MacIntosh, do your thingie thing
			}
			else if (ability.compare("ArmorPlate") == 0)
			{
				int iLayers, iMax;
				p_pEvent->GetRequiredIntParameter("Layers", iLayers);
				p_pEvent->GetRequiredIntParameter("MaxLayers", iMax);

				// TODO: MacIntosh (ya goof), do something to change the stats here
			}
			else if (ability.compare("Immune") == 0)
			{
				p_pEvent->GetRequiredStringParameter("ListenEvent", m_strHitCallback);
			}

			/*
			// See if an ability is waiting to negate an attack
			if (m_strHitCallback != "")
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event(m_strHitCallback);
				pEvent->SetIntParameter("Negated", 1);
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

				m_strHitCallback = "";
			}
			*/
		}
	}
}