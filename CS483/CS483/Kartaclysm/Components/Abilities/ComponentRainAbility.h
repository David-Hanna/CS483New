//----------------------------------------------------------------------------
// ComponentRainAbility.h
// Author: Bradley Cooper
//
// Cleopapa's ability to slow passing racers.
//----------------------------------------------------------------------------

#ifndef COMPONENT_RAIN_ABILITY_H
#define COMPONENT_RAIN_ABILITY_H

#include <tinyxml2.h>
#include <string>

#include "ComponentAbility.h"

namespace Kartaclysm
{
	class ComponentRainAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_RainAbility"; }

		// Destruction.
		virtual ~ComponentRainAbility();

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
		ComponentRainAbility(
			HeatStroke::GameObject* p_pGameObject,
			float p_fStrength
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& p_fStrength
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		float m_fStrength;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;
	};
}

#endif // COMPONENT_RAIN_ABILITY_H