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
#include "PlayerInputMapping.h"
#include "GameObject.h"
#include "KeyboardInputBuffer.h"
#include "EventManager.h"

namespace Kartaclysm
{
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
		void UpdateTransform(float p_fHeightMod);

		void HandleCollisionEvent(const HeatStroke::Event* p_pEvent);
		void HandleAbilityEvent(const HeatStroke::Event* p_pEvent);

		void Boost(float p_fPower);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;
		int m_iPlayerNum;

		glm::vec3 m_pOutsideForce;

		const float m_fSpeedScale = 1.0f;
		const float m_fVerticalSpeedScale = 1.0f;

		float m_fHeightAboveGroundStat = 0.2f;
		float m_fStickyHeightStat = 0.1f;
		float m_fMaxSpeedStat = 20.f;
		float m_fMaxReverseSpeedStat = 6.0f;
		float m_fAccelerationStat = 2.0f;
		float m_fReverseAccelerationStat = 2.0f;
		float m_fAccelerationFrictionStat = 2.0f;
		float m_fSpeedWhileTurningStat = 0.8f;
		float m_fSpeedWhileSlidingMinStat = 0.95f;
		float m_fSpeedWhileSlidingMaxStat = 0.9f;
		float m_fMaxTurnStat = 0.8f;
		float m_fTurnAccelerationStat = 120.0f;
		float m_fHopInitialSpeedStat = 2.0f;
		float m_fGravityAccelerationStat = -12.0f;
		float m_fSlideModifierStat = 0.006f;
		float m_fSlideMaxTurnModifierStat = 1.5f;
		float m_fTurnAtMaxSpeedStat = 0.8f;
		float m_fPeakTurnRatio = 0.2f;
		float m_fSwerveTurnModifier = 0.5f;
		float m_fSwerveAccelerationStat = 0.1f;
		float m_fWallBumpStat = 0.01f;
		float m_fWallSlowdownStat = 0.8f;
		float m_fOutsideForceAccelerationStat = 0.6f;
		float m_fSlideChargeAccelerationStat = 0.1f;
		float m_fSlideChargeMaxStat = 1.0f;
		float m_fSlideChargeThreshold = 0.5f;

		float m_fGroundHeight = 0.0f;
		float m_fPreviousHeight = 0.0f;
		float m_fSpeed = 0.0f;
		float m_fDirection = 0.0f;
		float m_fTurnSpeed = 0.0f;
		bool m_bAirborne = false;
		float m_fVerticalSpeed = 0.0f;
		bool m_bSliding = false;
		int m_iSlideDirection = 0;
		float m_fSwerve = 0.0f;
		float m_fSlideCharge = 0.0f;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;
	};
}

#endif // COMPONENT_KART_CONTROLLER_H