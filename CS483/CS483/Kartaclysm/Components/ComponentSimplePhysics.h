//----------------------------------------------------------------------------
// ComponentSimplePhysics.cpp
// Author: Bradley Cooper
//
// Component that handles simple physics for GameObjects.
//----------------------------------------------------------------------------

#ifndef COMPONENT_SIMPLE_PHYSICS_H
#define COMPONENT_SIMPLE_PHYSICS_H

#include <tinyxml2.h>

#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentSimplePhysics : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_SimplePhysics"; }
		virtual const std::string ComponentID() const override	{ return "GOC_SimplePhysics"; }

		// Destruction.
		virtual ~ComponentSimplePhysics();

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
		ComponentSimplePhysics(
			HeatStroke::GameObject* p_pGameObject,
			float p_fSpeed,
			float p_fFriction
			);

		void UpdateSpeed(float p_fDelta);
		float UpdateHop(float p_fDelta);
		void UpdateTransform(float p_fHeightMod);

		void HandleCollisionEvent(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& p_fSpeed,
			float& p_fFriction
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		glm::vec3 m_pOutsideForce;

		//const float m_fSpeedScale = 1.0f;
		const float m_fVerticalSpeedScale;

		float m_fHeightAboveGroundStat;
		float m_fStickyHeightStat;
		//float m_fMaxSpeedStat = 20.f;
		float m_fAccelerationFrictionStat;
		float m_fSimplePhysicsAccelerationStat;
		//float m_fWallBumpStat = 0.01f;
		//float m_fWallSlowdownStat = 0.8f;
		float m_fOutsideForceAccelerationStat;

		float m_fGroundHeight;
		float m_fPreviousHeight;
		float m_fSpeed;
		bool m_bAirborne;
		float m_fVerticalSpeed;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
	};
}

#endif // COMPONENT_SIMPLE_PHYSICS_H