//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#include "ComponentKartController.h"
#include "ComponentParticleEffect.h"

#include "ComponentAIDriver.h"

namespace Kartaclysm
{
	ComponentKartController::ComponentKartController(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_iPlayerNum(atoi(p_pGameObject->GetGUID().substr(6).c_str())), // "PlayerX"
		m_bAI(false),
		m_bDisabled(false),
		m_strHitCallback(""),

		m_iMaxSpeedCoreStat(3),
		m_iAccelerationCoreStat(3),
		m_iHandlingCoreStat(3),
		m_iDurabilityCoreStat(3),
		m_iMaxSpeedCoreStatMod(0),
		m_iAccelerationCoreStatMod(0),
		m_iHandlingCoreStatMod(0),
		m_iDurabilityCoreStatMod(0),

		m_fHeightAboveGroundStat(0.04f),
		m_fStickyHeightStat(0.2f),
		m_fSpeedScale(0.45f),
		m_fVerticalSpeedScale(1.2f),
		m_fWheelieRotation(PI * -0.15f),
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
		m_fHopInitialSpeedStat(1.05f),
		m_fGravityAccelerationStat(-5.0f),
		m_fSlideModifierStat(0.006f),
		m_fSlideMaxTurnModifierStat(1.5f),
		m_fTurnAtMaxSpeedStat(0.8f),
		m_fPeakTurnRatio(0.2f),
		m_fSwerveTurnModifier(0.35f),
		m_fSwerveAccelerationStat(2.0f),
		m_fWallBumpStat(8.0f),
		m_fWallSlowdownStat(0.3f),
		m_fOutsideForceAccelerationStat(-3.0f),
		m_fSlideChargeAccelerationStat(0.5f),
		m_fSlideChargeMaxStat(1.0f),
		m_fSlideChargeThreshold(0.2f),
		m_fWheelieTurnModStat(0.3f),
		m_fWheelieSpeedModStat(1.2f),
		m_fDurabilityStat(1.0f),
		m_fSpinSpeedStat(10.0f),
		m_fKartCollisionStat(2.0f),
		m_fOffroadFactorStat(0.5f),
		m_fOffroadRumbleFactor(0.05f),
		m_fAIRubberBandingFactorFirst(0.85f),
		m_fAIRubberBandingFactorLast(1.15f),
		m_fJumpBoostStat(1.8f),
		m_fWheelieOffsetStat(0.1f),

		m_fGroundHeight(0.04f),
		m_fPreviousHeight(0.04f),
		m_fSpeed(0.0f),
		m_fDirection(0.0f),
		m_fTurnSpeed(0.0f),
		m_bAirborne(false),
		m_fVerticalSpeed(0.0f),
		m_bSliding(false),
		m_iSlideDirection(0),
		m_bSlideParticle(false),
		m_fSwerve(0.0f),
		m_fSlideCharge(0.0f),
		m_bWheelie(false),
		m_fSpinout(0.0f),
		m_fTurnLock(0.0f),
		m_fSlowDuration(0.0f),
		m_fSlowPower(1.0f),
		m_fSpinFactor(0.0f),
		m_bOffroad(false),
		m_fOffroadRumble(0.0f)
	{
		m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentKartController::HandleCollisionEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);

		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentKartController::HandleAbilityEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("AbilityUse", m_pAbilityDelegate);

		m_pCountdownDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentKartController::HandleCountdownEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("KartCountdown", m_pCountdownDelegate);

		m_pOutsideForce = glm::vec3();

		HeatStroke::GameObject* pTrack = m_pGameObject->GetManager()->GetGameObject("Track");
		m_pTrackComponent = static_cast<ComponentTrack*>(pTrack->GetComponent("GOC_Track"));

		UpdateStats(m_iMaxSpeedCoreStat, m_iAccelerationCoreStat, m_iHandlingCoreStat, m_iDurabilityCoreStat);
	}

	ComponentKartController::~ComponentKartController()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Collision", m_pCollisionDelegate);
		delete m_pCollisionDelegate;
		m_pCollisionDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener("AbilityUse", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener("KartCountdown", m_pCountdownDelegate);
		delete m_pCountdownDelegate;
		m_pCountdownDelegate = nullptr;
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

	void ComponentKartController::SetAI(bool p_bAI)
	{
		m_bAI = p_bAI;

		if (m_bAI)
		{
			// add ComponentAIDriver to game object if not already present
			ComponentAIDriver* pAIDriverComponent = static_cast<ComponentAIDriver*>(m_pGameObject->GetComponent("GOC_AIDriver"));
			if (pAIDriverComponent == nullptr)
			{
				Component* pAIDriverComponent = ComponentAIDriver::CreateComponent(m_pGameObject, nullptr, nullptr);
				m_pGameObject->AddComponent(pAIDriverComponent);

				// TODO - ensure humans can't activate abilities
				//			not sure of the best way to do this currently
			}
		}
		else
		{
			m_pGameObject->RemoveComponent("GOC_AIDriver");
		}
	}

	void ComponentKartController::SetKartStats(int p_iMaxSpeed, int p_iAcceleration, int p_iHandling, int p_iDurability)
	{
		m_iMaxSpeedCoreStat = p_iMaxSpeed;
		m_iAccelerationCoreStat = p_iAcceleration;
		m_iHandlingCoreStat = p_iHandling;
		m_iDurabilityCoreStat = p_iDurability;

		UpdateStats(
			m_iMaxSpeedCoreStat + m_iMaxSpeedCoreStatMod,
			m_iAccelerationCoreStat + m_iAccelerationCoreStatMod,
			m_iHandlingCoreStat + m_iHandlingCoreStatMod,
			m_iDurabilityCoreStat + m_iDurabilityCoreStatMod
			);
	}

	void ComponentKartController::SetDriverStatMods(int p_iMaxSpeed, int p_iAcceleration, int p_iHandling, int p_iDurability)
	{
		m_iMaxSpeedCoreStatMod = p_iMaxSpeed;
		m_iAccelerationCoreStatMod = p_iAcceleration;
		m_iHandlingCoreStatMod = p_iHandling;
		m_iDurabilityCoreStatMod = p_iDurability;

		UpdateStats(
			m_iMaxSpeedCoreStat + m_iMaxSpeedCoreStatMod,
			m_iAccelerationCoreStat + m_iAccelerationCoreStatMod,
			m_iHandlingCoreStat + m_iHandlingCoreStatMod,
			m_iDurabilityCoreStat + m_iDurabilityCoreStatMod
			);
	}

	void ComponentKartController::UpdateStats(int p_iMaxSpeed, int p_iAcceleration, int p_iHandling, int p_iDurability)
	{
		// Max Speed
		m_fMaxSpeedStat = 20.0f + (0.2f * p_iMaxSpeed);
		m_fMaxReverseSpeedStat = 5.6f + (0.1f * p_iMaxSpeed);

		// Acceleration
		m_fAccelerationStat = 0.8f + (0.2f * p_iAcceleration);
		m_fReverseAccelerationStat = 0.9f + (0.1f * p_iAcceleration);

		// Handling
		m_fMaxTurnStat = 0.7f + (0.04f * p_iHandling);
		m_fSlideModifierStat = 0.004f + (0.0005f * p_iHandling);
		m_fSlideMaxTurnModifierStat = 1.2f + (0.1f * p_iHandling);
		m_fTurnAtMaxSpeedStat = 0.65f + (0.05f * p_iHandling);

		// Durability
		m_fDurabilityStat = 1.0f - (0.12f * p_iDurability);
	}

	void ComponentKartController::Update(const float p_fDelta)
	{
		// Manually query for user input
		int iAccelerate = 0, iBrake = 0, iSlide = 0;
		float fTurn = 0.0f;
		if (!m_bDisabled)
		{
			if (!m_bAI)
			{
				PlayerInputMapping::Instance()->QueryPlayerMovement(m_iPlayerNum, iAccelerate, iBrake, iSlide, fTurn);
				fTurn *= -1.0f; // Reversed because of mismatch between what the game and the controller consider to be the positive horizontal direction
			}
			else
			{
				ComponentAIDriver* aiDriver = static_cast<ComponentAIDriver*>(m_pGameObject->GetComponent("GOC_AIDriver"));
				if (aiDriver != nullptr)
				{
					aiDriver->QueryPlayerMovement(m_iPlayerNum, iAccelerate, iBrake, iSlide, fTurn);
				}
			}
		}

		// Spinout causes all inputs to be ignored
		if (m_fSpinout > 0.0f)
		{
			iAccelerate = 0;
			iBrake = 0;
			iSlide = 0;
			fTurn = 0.0f;

			m_fSpinout -= p_fDelta;
			if (m_fSpinout < 0.0f) m_fSpinout = 0.0f;
		}

		// Turnlock causes turn inputs to be ignored
		if (m_fTurnLock > 0.0f)
		{
			fTurn = 0.0f;

			m_fTurnLock -= p_fDelta;
			if (m_fTurnLock < 0.0f) m_fTurnLock = 0.0f;
		}

		// Slow just reduces speed
		if (m_fSlowDuration > 0.0f)
		{
			m_fSlowDuration -= p_fDelta;
			if (m_fSlowDuration < 0.0f)
			{
				m_fSlowDuration = 0.0f;
				m_fSlowPower = 1.0f;

				// Remove the HUD element
				HeatStroke::Event* pEvent = new HeatStroke::Event(m_pGameObject->GetGUID() + "_HUD_Slow");
				pEvent->SetIntParameter("Display", 0);
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
			}
		}

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
		UpdateTransform(fHeightMod, p_fDelta);

		UpdateOffroadParticle();
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

		// And from wheelie
		if (m_bWheelie)
		{
			fSpeedModifer *= m_fWheelieSpeedModStat;
		}

		// And from slow debuffs
		if (m_fSlowDuration > 0.0f)
		{
			fSpeedModifer *= m_fSlowPower;
		}

		// ...And from offroading (also rumble)
		m_fOffroadRumble = 0.0f;
		if (m_bOffroad)
		{
			fSpeedModifer *= m_fOffroadFactorStat;

			if (!m_bAirborne)
			{
				float fRumbleLimit = fminf(m_fSpeed / (m_fMaxSpeedStat * m_fSpeedScale), 1.0f) * m_fOffroadRumbleFactor;
				m_fOffroadRumble = -fRumbleLimit + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (fRumbleLimit * 2.0f))); // thanks internet, you da real MVP
			}
		}

		// ...And from AI rubber banding
		if (m_bAI)
		{
			ComponentRacer* pRacer = (ComponentRacer*)m_pGameObject->GetComponent("GOC_Racer");
			if (pRacer != nullptr)
			{
				int iPosition = pRacer->GetCurrentPosition();
				
				if (iPosition < m_pTrackComponent->GetLeadHumanPosition())
				{
					fSpeedModifer *= m_fAIRubberBandingFactorFirst;
				}

				if (iPosition > m_pTrackComponent->GetRearHumanPosition())
				{
					fSpeedModifer *= m_fAIRubberBandingFactorLast;
				}
			}
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

		// Modify if in wheelie
		if (m_bWheelie)
		{
			fTurnTarget *= m_fWheelieTurnModStat;
			fModifier *= m_fWheelieTurnModStat;
		}

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
			else if (m_iSlideDirection == 0)
			{
				m_bSliding = false;
			}

			fTurnTarget *= m_fSlideMaxTurnModifierStat;
			fModifier *= m_fSlideModifierStat;

			if (m_iSlideDirection > 0 && fTurnTarget < 0.4f)
			{
				fTurnTarget = 0.4f;
			}
			else if (m_iSlideDirection < 0 && fTurnTarget > -0.4f)
			{
				fTurnTarget = -0.4f;
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
		if (!m_bAirborne)
		{
			if (m_pGameObject->GetTransform().GetTranslation().y < fTrackHeight + m_fStickyHeightStat)
			{
				fHeightMod = fTrackHeight - m_pGameObject->GetTransform().GetTranslation().y;
				m_bAirborne = false;
			}
			else if (m_pGameObject->GetTransform().GetTranslation().y > fTrackHeight + m_fStickyHeightStat)
			{
				float heightDifference = (m_pGameObject->GetTransform().GetTranslation().y - m_fPreviousHeight) * (1.0f / p_fDelta);

				m_fVerticalSpeed = heightDifference * m_fVerticalSpeedScale;
				m_bAirborne = true;

				Boost(m_fJumpBoostStat);
			}
		}
		
		// If airborne, fall towards the ground and attempt to land
		if (m_bAirborne)
		{
			m_bWheelie = false;
			
			m_fVerticalSpeed += m_fGravityAccelerationStat * m_fVerticalSpeedScale * p_fDelta;
			fHeightMod = m_fVerticalSpeed * p_fDelta;

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

		m_fPreviousHeight = m_pGameObject->GetTransform().GetTranslation().y;

		if (fHeightMod >= 0.5f)
		{
			fHeightMod = 0.0f;
		}

		return fHeightMod;
	}

	void ComponentKartController::UpdateSlide(int p_iSlideInput, float p_fDelta)
	{
		// End the slide if the input is released, or if speed drops too low
		if (m_bSliding)
		{
			// End the slide if offroad, without a boost
			if (m_bOffroad)
			{
				m_bSliding = false;
				m_iSlideDirection = 0;
				m_fSlideCharge = 0.0f;
			}
			else
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

		// Particles
		if (m_bSliding && !m_bSlideParticle)
		{
			HeatStroke::ComponentParticleEffect* pComponentParticleEffect = (HeatStroke::ComponentParticleEffect*)m_pGameObject->GetComponent("GOC_ParticleEffect");
			if (pComponentParticleEffect)
			{
				if (m_iSlideDirection > 0)
				{
					HeatStroke::Effect* pSwerveLeftEffect = pComponentParticleEffect->GetEffect("swerve_left");
					if (pSwerveLeftEffect != nullptr)
					{
						pSwerveLeftEffect->Start();
					}
					m_bSlideParticle = true;
				}
				else if (m_iSlideDirection < 0)
				{
					HeatStroke::Effect* pSwerveRightEffect = pComponentParticleEffect->GetEffect("swerve_right");
					if (pSwerveRightEffect != nullptr)
					{
						pSwerveRightEffect->Start();
					}
					m_bSlideParticle = true;
				}
			}
		}
		else if (!m_bSliding && m_bSlideParticle)
		{
			HeatStroke::ComponentParticleEffect* pComponentParticleEffect = (HeatStroke::ComponentParticleEffect*)m_pGameObject->GetComponent("GOC_ParticleEffect");
			if (pComponentParticleEffect)
			{
				HeatStroke::Effect* pSwerveLeftEffect = pComponentParticleEffect->GetEffect("swerve_left");
				HeatStroke::Effect* pSwerveRightEffect = pComponentParticleEffect->GetEffect("swerve_right");
				if (pSwerveLeftEffect != nullptr)
				{
					pSwerveLeftEffect->Stop();
				}
				if (pSwerveRightEffect != nullptr)
				{
					pSwerveRightEffect->Stop();
				}

				m_bSlideParticle = false;
			}
		}
	}

	void ComponentKartController::UpdateTransform(float p_fHeightMod, float p_fDelta)
	{
		m_pGameObject->GetTransform().Translate(m_pOutsideForce * p_fDelta);
		m_pOutsideForce *= powf(2.718281828f, p_fDelta * m_fOutsideForceAccelerationStat);

		float spin = m_fSpinout * m_fSpinFactor;
		
		m_pGameObject->GetTransform().TranslateXYZ(m_fSpeed * sinf(m_fDirection) * p_fDelta, p_fHeightMod, m_fSpeed * cosf(m_fDirection) * p_fDelta);
		// swerve temporarily disabled until the camera transform heirarchy is fixed
		if (m_bWheelie)
		{
			m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(m_fWheelieRotation + m_fOffroadRumble, m_fDirection + GetRotationMod(), 0.0f)));
		}
		else
		{
			m_pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(m_fOffroadRumble, m_fDirection + GetRotationMod(), 0.0f)));
		}

		ComponentRacer* pRacer = static_cast<ComponentRacer*>(m_pGameObject->GetComponent("GOC_Racer"));
		if (pRacer != nullptr)
		{
			if (m_bWheelie)
			{
				pRacer->GetDriver()->GetTransform().SetTranslation(glm::vec3(0.0f, m_fWheelieOffsetStat, 0.0f));
				pRacer->GetKart()->GetTransform().SetTranslation(glm::vec3(0.0f, m_fWheelieOffsetStat, 0.0f));
			}
			else
			{
				pRacer->GetDriver()->GetTransform().SetTranslation(glm::vec3(0.0f, 0.0f, 0.0f));
				pRacer->GetKart()->GetTransform().SetTranslation(glm::vec3(0.0f, 0.0f, 0.0f));
			}
		}

		//HeatStroke::HierarchicalTransform transform = m_pGameObject->GetTransform();
		//printf("Position:\n  X: %f\n  Y: %f\n  Z: %f\nRotation:\n  X: %f\n  Y: %f\n  Z: %f\nSpeed:\n  %f\nTurn speed:\n  %f\nVertical Speed:\n  %f\nSliding:\n  %i\nSlide direction:\n  %i\n\n", transform.GetTranslation().x, transform.GetTranslation().y, transform.GetTranslation().z, transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, m_fSpeed, m_fTurnSpeed, m_fVerticalSpeed,m_bSliding, m_iSlideDirection);
	}

	void ComponentKartController::UpdateOffroadParticle()
	{
		HeatStroke::ComponentParticleEffect* pComponentParticleEffect = static_cast<HeatStroke::ComponentParticleEffect*>(m_pGameObject->GetComponent("GOC_ParticleEffect"));
		if (pComponentParticleEffect)
		{
			HeatStroke::Effect* pBoostParticleEffect = pComponentParticleEffect->GetEffect("offroad");
			if (pBoostParticleEffect != nullptr)
			{
				if (m_bOffroad && !m_bAirborne && m_fSpeed >= m_fMaxSpeedStat * m_fSpeedScale * 0.2f)
				{
					pBoostParticleEffect->Start(); 
				}
				else
				{
					pBoostParticleEffect->Stop();
				}
			}
		}
	}

	void ComponentKartController::RaceFinishParticles()
	{
		HeatStroke::ComponentParticleEffect* pComponentParticleEffect = static_cast<HeatStroke::ComponentParticleEffect*>(m_pGameObject->GetComponent("GOC_ParticleEffect"));
		if (pComponentParticleEffect)
		{
			HeatStroke::Effect* pFinishParticleEffect = pComponentParticleEffect->GetEffect("race_finish");
			if (pFinishParticleEffect != nullptr)
			{
				pFinishParticleEffect->Start();
			}
		}
	}

	void ComponentKartController::Boost(float p_fPower)
	{
		if (m_fSpeed < m_fMaxSpeedStat * m_fSpeedScale * 0.5f)
		{
			m_fSpeed = m_fMaxSpeedStat * m_fSpeedScale * 0.5f;
		}

		float extra = (m_fMaxSpeedStat * m_fSpeedScale * p_fPower) - m_fSpeed;
		m_fSpeed = fmaxf(m_fSpeed, m_fSpeed + (extra * (m_fSpeed / (m_fMaxSpeedStat * m_fSpeedScale))));

		HeatStroke::ComponentParticleEffect* pComponentParticleEffect = (HeatStroke::ComponentParticleEffect*)m_pGameObject->GetComponent("GOC_ParticleEffect");
		if (pComponentParticleEffect)
		{
			HeatStroke::Effect* pBoostParticleEffect = pComponentParticleEffect->GetEffect("boost");
			if (pBoostParticleEffect != nullptr)
			{
				pBoostParticleEffect->Start();
			}
		}
	}

	void ComponentKartController::WheelieToggle()
	{
		m_bWheelie = !m_bWheelie;

		if (m_bSliding)
		{
			m_bWheelie = false;
		}
	}

	void ComponentKartController::Spinout(float p_fDuration)
	{
		m_bWheelie = false;
		
		m_fSpinout = fmaxf(p_fDuration * m_fDurabilityStat, m_fSpinout);

		m_fSpinFactor = m_fSpinSpeedStat;
	}

	void ComponentKartController::ArmorPlate(int p_iCurrentArmorStack)
	{
		m_iMaxSpeedCoreStat = 1 + p_iCurrentArmorStack;
		m_iAccelerationCoreStat = 5 - p_iCurrentArmorStack;
		m_iHandlingCoreStat = 5 - p_iCurrentArmorStack;
		m_iDurabilityCoreStat = 1 + p_iCurrentArmorStack;

		UpdateStats(
			m_iMaxSpeedCoreStat + m_iMaxSpeedCoreStatMod,
			m_iAccelerationCoreStat + m_iAccelerationCoreStatMod,
			m_iHandlingCoreStat + m_iHandlingCoreStatMod,
			m_iDurabilityCoreStat + m_iDurabilityCoreStatMod
			);
	}

	void ComponentKartController::Slow(float p_fPower, float p_fDuration)
	{
		m_fSlowDuration = fmaxf(p_fDuration * m_fDurabilityStat, m_fSlowDuration);
		m_fSlowPower = p_fPower;
	}

	void ComponentKartController::TurnLock(float p_fDuration)
	{
		m_fTurnLock = fmaxf(p_fDuration * m_fDurabilityStat, m_fTurnLock);
	}

	glm::quat ComponentKartController::GetRotationMinusSwerve()
	{
		return glm::quat(glm::vec3(0.0f, m_fDirection, 0.0f));
	}

	float ComponentKartController::GetRotationMod()
	{
		float spin = m_fSpinout * m_fSpinFactor;
		return m_fSwerve + spin;
	}

	void ComponentKartController::HandleCollisionEvent(const HeatStroke::Event* p_pEvent)
	{
		std::string guid1;
		std::string guid2;
		p_pEvent->GetRequiredGameObjectParameter("Object1GUID", guid1);
		p_pEvent->GetRequiredGameObjectParameter("Object2GUID", guid2);

		HeatStroke::GameObject* pOther = nullptr;
		if (guid1.compare(m_pGameObject->GetGUID()) == 0)
		{
			pOther = m_pGameObject->GetManager()->GetGameObject(guid2);
		}
		else if (guid2.compare(m_pGameObject->GetGUID()) == 0)
		{
			pOther = m_pGameObject->GetManager()->GetGameObject(guid1);
		}

		if (pOther != nullptr)
		{
			HeatStroke::ComponentCollider* pOtherCollider = static_cast<HeatStroke::ComponentCollider*>(pOther->GetComponent("GOC_Collider"));
			if (pOtherCollider->HasPhysics())
			{
				glm::vec3 contactPoint = glm::vec3();
				p_pEvent->GetRequiredFloatParameter("ContactPointX", contactPoint.x);
				p_pEvent->GetRequiredFloatParameter("ContactPointY", contactPoint.y);
				p_pEvent->GetRequiredFloatParameter("ContactPointZ", contactPoint.z);
				int passedThroughInt;
				p_pEvent->GetOptionalIntParameter("PassedThrough", passedThroughInt, 0);
				bool passedThrough = (passedThroughInt != 0); // I know

				HeatStroke::ComponentSphereCollider* collider = static_cast<HeatStroke::ComponentSphereCollider*>(m_pGameObject->GetComponent("GOC_Collider"));

				std::string otherColliderID = pOtherCollider->ComponentID();
				if (otherColliderID.compare("GOC_WallCollider") == 0)
				{
					// Wall Collision
					glm::vec3 difference = m_pGameObject->GetTransform().GetTranslation() - contactPoint;
					float distance = abs(collider->GetRadius() - glm::length(difference));

					difference = glm::normalize(difference) * fmaxf(distance, 0.0000001f);
					m_pGameObject->GetTransform().Translate(difference);

					glm::vec3 velocity = glm::vec3(sinf(m_fDirection), 0.0f, cosf(m_fDirection));
					float dotProduct = glm::dot(velocity, glm::normalize(contactPoint - m_pGameObject->GetTransform().GetTranslation()));

					m_pOutsideForce = glm::normalize(difference) * m_fWallBumpStat * ((m_fSpeed / m_fSpeedScale) / m_fMaxSpeedStat) * dotProduct;
					m_pOutsideForce = m_pOutsideForce * glm::vec3(1.0f, 0.0f, 1.0f); // Extra check to make sure there's no vertical speed
					m_fSpeed *= m_fWallSlowdownStat;
				}
				else
				{
					// Kart Collision
					glm::vec3 difference = m_pGameObject->GetTransform().GetTranslation() - pOther->GetTransform().GetTranslation();
					float distance = abs(collider->GetRadius() - glm::length(difference));

					difference = glm::normalize(difference) * fmaxf(distance, 0.0000001f);
					m_pGameObject->GetTransform().Translate(difference);

					m_pOutsideForce = glm::normalize(difference) * m_fKartCollisionStat;
					m_fSpeed *= m_fWallSlowdownStat;
				}

				if (passedThrough)
				{
					// Again, not sure which is the right one, will depend on what order Update()s happen
					if (m_pGameObject->GetTransform().GetTranslation() == collider->GetPosition())
					{
						m_pGameObject->GetTransform().SetTranslation(collider->GetPreviousPosition());
					}
					else
					{
						m_pGameObject->GetTransform().SetTranslation(collider->GetPosition());
					}
				}
			}
		}
	}

	void ComponentKartController::HandleAbilityEvent(const HeatStroke::Event* p_pEvent)
	{
		// TODO: make this use game objects (brad, ya dingus)
		std::string originator, ability, target = "";
		float power = 0.0f, duration = 0.0f;
		p_pEvent->GetRequiredStringParameter("Originator", originator);
		p_pEvent->GetRequiredStringParameter("Ability", ability);
		p_pEvent->GetOptionalStringParameter("Target", target, target);
		p_pEvent->GetOptionalFloatParameter("Power", power, power);
		p_pEvent->GetOptionalFloatParameter("Duration", duration, duration);

		if (target.compare(m_pGameObject->GetGUID()) == 0)
		{
#ifdef _DEBUG
			printf("KartController: Ability %s from %s targetting %s\n",
				ability.c_str(), originator.c_str(), target.c_str());
#endif

			// See if an ability is waiting to negate an attack
			if (m_strHitCallback != "")
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event(m_strHitCallback);
				pEvent->SetIntParameter("Negated", 1);
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

				m_strHitCallback = "";
				return;
			}

			std::string effect;
			p_pEvent->GetRequiredStringParameter("Effect", effect);

			if (ability.compare("Strike") == 0)
			{
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/kingpin_strike_hit.flac");
				Spinout(duration);
			}
			else if (ability.compare("Clock") == 0)
			{
				Spinout(duration);
			}
			else if (ability.compare("Rain") == 0)
			{
				// Send event for HUD
				HeatStroke::Event* pEvent = new HeatStroke::Event(target + "_HUD_Slow");
				pEvent->SetIntParameter("Display", 1);
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/cleopapa_make_it_rain.flac");
				Slow(power, duration);

				HeatStroke::ComponentParticleEffect* pComponentParticleEffect = (HeatStroke::ComponentParticleEffect*)m_pGameObject->GetComponent("GOC_ParticleEffect");
				if (pComponentParticleEffect)
				{
					HeatStroke::Effect* pMakeItRainParticleEffect = pComponentParticleEffect->GetEffect("make_it_rain");
					if (pMakeItRainParticleEffect != nullptr)
					{
						pMakeItRainParticleEffect->Start();
					}
				}
			}
			else if (ability.compare("Bedazzle") == 0)
			{
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/cleopapa_bedazzle.flac");
				Spinout(duration);
			}
		}
		else if (originator.compare(m_pGameObject->GetGUID()) == 0)
		{
#ifdef _DEBUG
			if (target == "")
			{
				printf("KartController: Ability %s from %s targetting %s\n",
					ability.c_str(), originator.c_str(), "self");
			}
#endif

			if (ability.compare("Boost") == 0)
			{
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/speedster_boost.flac");
				Boost(power);
			}
			else if (ability.compare("Wheelie") == 0)
			{
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/showoff_wheelie.flac");
				WheelieToggle();
			}
			else if (ability.compare("Tinker") == 0)
			{
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/clockmaker_tinker.flac");
				TurnLock(duration);
			}
			else if (ability.compare("ArmorPlate") == 0)
			{
				int iLayers, iMax;
				p_pEvent->GetRequiredIntParameter("Layers", iLayers);
				p_pEvent->GetRequiredIntParameter("MaxLayers", iMax);

				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/juggernaut_armor.flac");
				ArmorPlate(iLayers);
			}
			else if (ability.compare("Immune") == 0)
			{
				p_pEvent->GetRequiredStringParameter("ListenEvent", m_strHitCallback);
			}
		}
	}

	void ComponentKartController::HandleCountdownEvent(const HeatStroke::Event* p_pEvent)
	{
		int iDisable;
		float fBoost;
		p_pEvent->GetOptionalIntParameter("Disable", iDisable, 0);
		p_pEvent->GetOptionalFloatParameter(m_pGameObject->GetGUID(), fBoost, 0.0f);

		m_bDisabled = (iDisable != 0);
		if (fBoost > 0.0f)
		{
			Boost(fBoost);
		}
	}
}
