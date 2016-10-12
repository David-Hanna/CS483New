//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#ifndef COMPONENT_KART_CONTROLLER_H
#define COMPONENT_KART_CONTROLLER_H

#include <tinyxml2.h>

#include "Component.h"

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

		const float m_fSpeedScale = 1.0f;

		float m_fMaxSpeedStat = 10.0f;
		float m_fMaxReverseSpeedStat = 4.0f;
		float m_fAccelerationStat = 0.3f;
		float m_fReverseAccelerationStat = 0.6f;
		float m_fAccelerationFrictionStat = 1.2f;
		float m_fTurningStat = 2.0f;

		float m_fSpeed = 0.0f;
		float m_fDirection = 0.0f;;
	};
}

#endif // COMPONENT_KART_CONTROLLER_H