//----------------------------------------------------------------------------
// ComponentWheelieAbility.h
// Author: Bradley Cooper
//
// Showoff's ability to reduce steering but gain speed.
//----------------------------------------------------------------------------

#ifndef COMPONENT_WHEELIE_ABILITY_H
#define COMPONENT_WHEELIE_ABILITY_H

#include "ComponentAIDriver.h"

#include <tinyxml2.h>
#include <string>

#include "ComponentAbility.h"

namespace Kartaclysm
{
	class ComponentWheelieAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_WheelieAbility"; }

		// Destruction.
		virtual ~ComponentWheelieAbility();

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
		virtual void AICheckCondition(HeatStroke::Component* p_pAIDriver) override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentWheelieAbility(
			HeatStroke::GameObject* p_pGameObject
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;
	};
}

#endif // COMPONENT_WHEELIE_ABILITY_H