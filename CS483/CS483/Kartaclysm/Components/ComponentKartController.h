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

		const float m_fSpeedScale;
		const float m_fVerticalSpeedScale;

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

		float m_fGroundHeight;
		float m_fPreviousHeight;
		float m_fSpeed;
		float m_fDirection;
		float m_fTurnSpeed;
		bool m_bAirborne;
		float m_fVerticalSpeed;
		bool m_bSliding;
		int m_iSlideDirection;
		float m_fSwerve;
		float m_fSlideCharge;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;
	};
}

#endif // COMPONENT_KART_CONTROLLER_H