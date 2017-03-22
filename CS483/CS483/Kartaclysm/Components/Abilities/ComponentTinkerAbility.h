//----------------------------------------------------------------------------
// ComponentTinkerAbility.h
// Author: Bradley Cooper
//
// Clockmaker's ability to add charges to his clock bomb.
//----------------------------------------------------------------------------

#ifndef COMPONENT_TINKER_ABILITY_H
#define COMPONENT_TINKER_ABILITY_H

#include "ComponentAIDriver.h"

#include <tinyxml2.h>
#include <string>

#include "ComponentAbility.h"

namespace Kartaclysm
{
	class ComponentTinkerAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_TinkerAbility"; }

		// Destruction.
		virtual ~ComponentTinkerAbility();

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
		ComponentTinkerAbility(
			HeatStroke::GameObject* p_pGameObject,
			int p_iStartCharges,
			int p_iMaxCharges,
			float p_fDuration
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }
		void ChargeCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			int& p_iStartCharges,
			int& p_iMaxCharges,
			float& p_fDuration
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
		float m_fDuration;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;

		// Delegate function to register with EventManager for listening to changes
		std::function<void(const HeatStroke::Event*)>* m_pChargeDelegate;
	};
}

#endif // COMPONENT_TINKER_ABILITY_H