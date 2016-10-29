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

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentKartController(
			HeatStroke::GameObject* p_pGameObject
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;
		int m_iPlayerNum;

		const float m_fSpeedScale = 1.0f;
		const float m_fVerticalSpeedScale = 1.0f;

		float m_fMaxSpeedStat = 10.0f;
		float m_fMaxReverseSpeedStat = 4.0f;
		float m_fAccelerationStat = 0.3f;
		float m_fReverseAccelerationStat = 0.6f;
		float m_fAccelerationFrictionStat = 1.2f;
		float m_fSpeedWhileTurningStat = 0.7f;
		float m_fSpeedWhileSlidingMinStat = 0.9f;
		float m_fSpeedWhileSlidingMaxStat = 0.7f;
		float m_fMaxTurnStat = 2.0f;
		float m_fTurnAccelerationStat = 24.0f;
		float m_fHopInitialSpeedStat = 2.0f;
		float m_fGravityAccelerationStat = -12.0f;
		float m_fSlideModifierStat = 0.1f;
		float m_fSlideMaxTurnModifierStat = 1.2f;
		float m_fTurnAtMaxSpeedStat = 0.7f;
		float m_fPeakTurnRatio = 0.2f;

		float m_fGroundHeight = 0.0f;
		float m_fSpeed = 0.0f;
		float m_fDirection = 0.0f;
		float m_fTurnSpeed = 0.0f;
		bool m_bAirborne = false;
		float m_fVerticalSpeed = 0.0f;
		bool m_bSliding = false;
		int m_iSlideDirection = 0;
	};
}

#endif // COMPONENT_KART_CONTROLLER_H