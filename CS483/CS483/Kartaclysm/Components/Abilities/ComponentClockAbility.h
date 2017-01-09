//----------------------------------------------------------------------------
// ComponentClockAbility.h
// Author: Bradley Cooper
//
// Clockmaker's ability to throw an AoE lobbed bomb forward.
//----------------------------------------------------------------------------

#ifndef COMPONENT_CLOCK_ABILITY_H
#define COMPONENT_CLOCK_ABILITY_H

#include "Common.h"
#include "ComponentAbility.h"
#include "ComponentProjectile.h"
#include "ComponentKartController.h"
#include "ComponentSimplePhysics.h"
#include "AudioPlayer.h"

#include <tinyxml2.h>
#include <string>

namespace Kartaclysm
{
	class ComponentClockAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_ClockAbility"; }

		// Destruction.
		virtual ~ComponentClockAbility();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override;
		virtual void Update(const float p_fDelta) override {}

		// Required ability override
		virtual void Activate() override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentClockAbility(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strProjectileXML,
			float p_fDuration
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }
		void OnHitCallback(const HeatStroke::Event* p_pEvent);
		void ChargeCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strProjectileXML,
			float& p_fDuration
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strProjectileXML;
		std::string m_strChargeEventName;
		float m_fDuration;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;

		// Delegate function to register with EventManager for clock blast ability hit event
		std::function<void(const HeatStroke::Event*)>* m_pOnHitDelegate;

		// Delegate function to register with EventManager for managing charges
		std::function<void(const HeatStroke::Event*)>* m_pChargeDelegate;
	};
}

#endif // COMPONENT_CLOCK_ABILITY_H