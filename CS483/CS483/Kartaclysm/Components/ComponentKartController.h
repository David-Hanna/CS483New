//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#ifndef COMPONENT_KART_CONTROLLER_H
#define COMPONENT_KART_CONTROLLER_H

#include <tinyxml2.h>

#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "KeyboardInputBuffer.h"
#include "ComponentSphereCollider.h"
#include "AudioPlayer.h"

namespace Kartaclysm
{
	class ComponentTrack; // forward declaration necessary for the code to compile, seems good
	
	class ComponentKartController : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_KartController"; }
		virtual const std::string ComponentID() const override	{ return "GOC_KartController"; }

		// Destruction.
		virtual ~ComponentKartController();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		// Used in determining camera position
		glm::quat GetRotationMinusSwerve();

		void UpdateTrackHeight(float p_fTrackHeight) { m_fGroundHeight = p_fTrackHeight; }
		float GetDirection() const { return m_fDirection; }
		float GetGroundHeight() const { return m_fGroundHeight; }
		float GetSwerve() const { return m_fSwerve; }
		float GetRotationMod();
		bool IsInWheelie() const { return m_bWheelie; }
		bool IsAirborne() const { return m_bAirborne; }
		bool IsOffroad() const { return m_bOffroad; }
		float GetWheelieRotation() const { return m_fWheelieRotation; }
		void SetSpeed(float p_fSpeed) {m_fSpeed = p_fSpeed;}
		bool GetAI() const { return m_bAI; }
		void SetOffroad(bool p_bOffroad) { if (!m_bAirborne) m_bOffroad = p_bOffroad; }
		float GetOffroadRumble() const { return m_fOffroadRumble; }

		void RaceFinishParticles();

		void SetAI(bool p_bAI);
		void SetKartStats(int p_iMaxSpeed, int p_iAcceleration, int p_iHandling, int p_iDurability);
		void SetDriverStatMods(int p_iMaxSpeed, int p_iAcceleration, int p_iHandling, int p_iDurability);

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentKartController(
			HeatStroke::GameObject* p_pGameObject
			);

		void UpdateSpeed(int p_iAccelerateInput, int p_iBrakeInput, float p_fDelta);
		void UpdateTurn(float p_fTurnInput, float p_fDelta);
		float UpdateHop(int p_iSlideInput, float p_fDelta);
		void UpdateSlide(int p_iSlideInput, float p_fDelta);
		void UpdateTransform(float p_fHeightMod, float p_fDelta);

		void HandleCollisionEvent(const HeatStroke::Event* p_pEvent);
		void HandleAbilityEvent(const HeatStroke::Event* p_pEvent);
		void HandleCountdownEvent(const HeatStroke::Event* p_pEvent);

		void UpdateStats(int p_iMaxSpeed, int p_iAcceleration, int p_iHandling, int p_iDurability);

		void UpdateOffroadParticle();

		void Boost(float p_fPower);
		void WheelieToggle();
		void Spinout(float p_fDuration);
		void ArmorPlate(int p_iCurrentArmorStack);
		void TurnLock(float p_fDuration);
		void Slow(float p_fPower, float p_fDuration);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;
		int m_iPlayerNum;
		bool m_bAI;
		bool m_bDisabled;
		std::string m_strHitCallback;

		ComponentTrack* m_pTrackComponent;

		glm::vec3 m_pOutsideForce;

		const float m_fSpeedScale;
		const float m_fVerticalSpeedScale;
		const float m_fWheelieRotation;

		// Core stats all range from 0 (worst) to 6 (best)
		// 6 - S
		// 5 - A
		// 4 - B
		// 3 - C
		// 2 - D
		// 1 - E
		// 0 - F

		int m_iMaxSpeedCoreStat;
		int m_iAccelerationCoreStat;
		int m_iHandlingCoreStat;
		int m_iDurabilityCoreStat;

		int m_iMaxSpeedCoreStatMod;
		int m_iAccelerationCoreStatMod;
		int m_iHandlingCoreStatMod;
		int m_iDurabilityCoreStatMod;

		float m_fHeightAboveGroundStat;
		float m_fStickyHeightStat;
		float m_fMaxSpeedStat;
		float m_fMaxReverseSpeedStat;
		float m_fAccelerationStat;
		float m_fReverseAccelerationStat;
		float m_fAccelerationFrictionStat;
		float m_fSpeedWhileTurningStat;
		float m_fSpeedWhileSlidingMinStat;
		float m_fSpeedWhileSlidingMaxStat;
		float m_fMaxTurnStat;
		float m_fTurnAccelerationStat;
		float m_fHopInitialSpeedStat;
		float m_fGravityAccelerationStat;
		float m_fSlideModifierStat;
		float m_fSlideMaxTurnModifierStat;
		float m_fTurnAtMaxSpeedStat;
		float m_fPeakTurnRatio;
		float m_fSwerveTurnModifier;
		float m_fSwerveAccelerationStat;
		float m_fWallBumpStat;
		float m_fWallSlowdownStat;
		float m_fOutsideForceAccelerationStat;
		float m_fSlideChargeAccelerationStat;
		float m_fSlideChargeMaxStat;
		float m_fSlideChargeThreshold;
		float m_fWheelieTurnModStat;
		float m_fWheelieSpeedModStat;
		float m_fDurabilityStat;
		float m_fSpinSpeedStat;
		float m_fKartCollisionStat;
		float m_fOffroadFactorStat;
		float m_fOffroadRumbleFactor;
		float m_fAIRubberBandingFactorFirst;
		float m_fAIRubberBandingFactorLast;

		float m_fGroundHeight;
		float m_fPreviousHeight;
		float m_fSpeed;
		float m_fDirection;
		float m_fTurnSpeed;
		bool m_bAirborne;
		float m_fVerticalSpeed;
		bool m_bSliding;
		int m_iSlideDirection;
		bool m_bSlideParticle;
		float m_fSwerve;
		float m_fSlideCharge;
		bool m_bWheelie;
		float m_fSpinout;
		float m_fTurnLock;
		float m_fSlowDuration;
		float m_fSlowPower;
		float m_fSpinFactor;
		bool m_bOffroad;
		float m_fOffroadRumble;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pCountdownDelegate;
	};
}

#endif // COMPONENT_KART_CONTROLLER_H
