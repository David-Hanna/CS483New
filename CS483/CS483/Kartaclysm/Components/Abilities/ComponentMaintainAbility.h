//----------------------------------------------------------------------------
// ComponentMaintainAbility.h
// Author: Bradley Cooper
//
// Juggernaut's ability to add charges to his armor plate.
//----------------------------------------------------------------------------

#ifndef COMPONENT_MAINTAIN_ABILITY_H
#define COMPONENT_MAINTAIN_ABILITY_H

#include "ComponentAIDriver.h"

#include "AudioPlayer.h"
#include "ComponentAbility.h"

#include <tinyxml2.h>
#include <string>

namespace Kartaclysm
{
	class ComponentMaintainAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_MaintainAbility"; }

		// Destruction.
		virtual ~ComponentMaintainAbility();

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
		ComponentMaintainAbility(
			HeatStroke::GameObject* p_pGameObject,
			int p_iStartCharges,
			int p_iMaxCharges
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }
		void ChargeCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			int& p_iStartCharges,
			int& p_iMaxCharges
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		std::string m_strChargeEventName;
		int m_iCurrentCharges;
		int m_iMaxCharges;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;

		// Delegate function to register with EventManager for listening to changes
		std::function<void(const HeatStroke::Event*)>* m_pChargeDelegate;
	};
}

#endif // COMPONENT_MAINTAIN_ABILITY_H