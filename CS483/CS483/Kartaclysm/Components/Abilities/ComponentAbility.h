//----------------------------------------------------------------------------
// ComponentAbility.h
// Author: Bradley Cooper
//
// Abstract Component that provides an interface for abilities.
//----------------------------------------------------------------------------

#ifndef COMPONENT_ABILITY_H
#define COMPONENT_ABILITY_H

#include <tinyxml2.h>
#include <string>
#include <functional>

#include "Component.h"
#include "GameObject.h"
#include "ComponentAbilityConditions.h"

namespace Kartaclysm
{
	class ComponentAbility : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_Ability"; }

		// Destruction.
		virtual ~ComponentAbility() {};

		// Game Loop methods.
		virtual void Init() = 0;
		virtual void Update(const float p_fDelta) = 0;

		// Required ability override
		virtual void Activate() = 0;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAbility(
			HeatStroke::GameObject* p_pGameObject)
			:
			HeatStroke::Component(p_pGameObject) {}

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;
	};
}

#endif // COMPONENT_ABILITY_CONDITIONS_H