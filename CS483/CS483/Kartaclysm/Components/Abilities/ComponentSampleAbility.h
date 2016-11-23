//----------------------------------------------------------------------------
// ComponentSampleAbility.h
// Author: Bradley Cooper
//
// Sample ability that does nothing: shows off Ability Conditions.
//----------------------------------------------------------------------------

#ifndef COMPONENT_SAMPLE_ABILITY_H
#define COMPONENT_SAMPLE_ABILITY_H

#include <tinyxml2.h>
#include <string>

#include "ComponentAbility.h"

namespace Kartaclysm
{
	class ComponentSampleAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_SampleAbility"; }

		// Destruction.
		virtual ~ComponentSampleAbility();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;

		// Required ability override
		virtual void Activate() override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentSampleAbility(
			HeatStroke::GameObject* p_pGameObject,
			float p_fChargeCooldown
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& p_fChargeCooldown
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Every N seconds, add another charge
		float m_fChargeCooldown;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;
	};
}

#endif // COMPONENT_ABILITY_CONDITIONS_H