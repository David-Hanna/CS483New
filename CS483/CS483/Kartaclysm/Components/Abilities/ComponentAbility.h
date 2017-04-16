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
		virtual void AICheckCondition(HeatStroke::Component* p_pAIDriver) = 0;

		// Accessor
		const std::string& GetPlayerIdentifier() const { return m_strPlayerX; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAbility(
			HeatStroke::GameObject* p_pGameObject)
			:
			HeatStroke::Component(p_pGameObject),
			m_strPlayerX(p_pGameObject->GetParent()->GetParent()->GetGUID()) {}

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// The player number determined by the Ability GUID
		std::string m_strPlayerX;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;
	};
}

#endif // COMPONENT_ABILITY_H