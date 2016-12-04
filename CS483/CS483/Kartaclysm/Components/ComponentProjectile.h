//----------------------------------------------------------------------------
// ComponentProjectile.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#ifndef COMPONENT_PROJECTILE_H
#define COMPONENT_PROJECTILE_H

#include <tinyxml2.h>

#include "Common.h"
#include "Component.h"
#include "PlayerInputMapping.h"
#include "GameObject.h"
#include "KeyboardInputBuffer.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentProjectile : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_Projectile"; }
		virtual const std::string ComponentID() const override	{ return "GOC_Projectile"; }

		// Destruction.
		virtual ~ComponentProjectile();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		void UpdateTrackHeight(float p_fTrackHeight) { m_fGroundHeight = p_fTrackHeight; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentProjectile(
			HeatStroke::GameObject* p_pGameObject
			);

		void UpdateSpeed(float p_fDelta);
		float UpdateHop(float p_fDelta);
		void UpdateTransform(float p_fHeightMod);

		void HandleCollisionEvent(const HeatStroke::Event* p_pEvent);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strOriginator;
		std::string m_strEventName;

		glm::vec3 m_pOutsideForce;

		//const float m_fSpeedScale = 1.0f;
		const float m_fVerticalSpeedScale = 1.0f;

		float m_fHeightAboveGroundStat = 0.2f;
		//float m_fMaxSpeedStat = 20.f;
		float m_fAccelerationFrictionStat = 2.0f;
		float m_fGravityAccelerationStat = -12.0f;
		//float m_fWallBumpStat = 0.01f;
		//float m_fWallSlowdownStat = 0.8f;
		float m_fOutsideForceAccelerationStat = 0.6f;

		float m_fGroundHeight = 0.0f;
		float m_fSpeed = 0.0f;
		bool m_bAirborne = false;
		float m_fVerticalSpeed = 0.0f;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
	};
}

#endif // COMPONENT_PROJECTILE_H