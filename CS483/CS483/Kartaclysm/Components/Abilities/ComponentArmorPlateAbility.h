//----------------------------------------------------------------------------
// ComponentArmorPlateAbility.h
// Author: Bradley Cooper
//
// Juggernaut's ability to use charges to negate attacks and affect stats.
//----------------------------------------------------------------------------

#ifndef COMPONENT_ARMOR_ABILITY_H
#define COMPONENT_ARMOR_ABILITY_H

#include <tinyxml2.h>
#include <string>

#include "ComponentAbility.h"

namespace Kartaclysm
{
	class ComponentArmorPlateAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_ArmorPlateAbility"; }

		// Destruction.
		virtual ~ComponentArmorPlateAbility();

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
		virtual void Activate() override {}

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentArmorPlateAbility(
			HeatStroke::GameObject* p_pGameObject
			);

		void ChargeCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		std::string m_strChargeEventName;

		// Delegate function to register with EventManager for managing charges
		std::function<void(const HeatStroke::Event*)>* m_pChargeDelegate;
	};
}

#endif // COMPONENT_ARMOR_ABILITY_H