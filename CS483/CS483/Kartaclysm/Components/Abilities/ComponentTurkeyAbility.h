//----------------------------------------------------------------------------
// ComponentTurkeyAbility.h
// Author: Bradley Cooper
//
// Kingpin's ability to give a temporary speed boost.
//----------------------------------------------------------------------------

#ifndef COMPONENT_TURKEY_H
#define COMPONENT_TURKEY_H

#include "ComponentAIDriver.h"

#include "AudioPlayer.h"
#include "ComponentAbility.h"

#include <tinyxml2.h>
#include <string>


namespace Kartaclysm
{
	class ComponentTurkeyAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_TurkeyAbility"; }

		// Destruction.
		virtual ~ComponentTurkeyAbility();

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
		virtual void AICheckCondition(HeatStroke::Component* p_pAIDriver) override {}

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentTurkeyAbility(
			HeatStroke::GameObject* p_pGameObject,
			float p_fPower
			);

		void OnHitCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& m_fPower
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		float m_fPower;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for strike ability hit event
		std::function<void(const HeatStroke::Event*)>* m_pOnHitDelegate;
	};
}

#endif // COMPONENT_TURKEY_H