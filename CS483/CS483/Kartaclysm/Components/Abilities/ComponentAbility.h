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

		// Ability information accessors
		const std::string& GetName() const			{ return m_strName; }
		const std::string& GetDescription() const	{ return m_strDescription; }
		const std::string& GetIdentifier() const	{ return m_strIdentifier; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAbility(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strName,
			const std::string& p_strDescription,
			const std::string& p_strIdentifier)
			:
			HeatStroke::Component(p_pGameObject),
			m_strName(p_strName),
			m_strDescription(p_strDescription),
			m_strIdentifier(p_strIdentifier) {}

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Ability name, description, and identifier prefix ("P0_Kart1" as example)
		std::string m_strName;
		std::string m_strDescription;
		std::string m_strIdentifier;
	};
}

#endif // COMPONENT_ABILITY_CONDITIONS_H