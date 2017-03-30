//----------------------------------------------------------------------------
// ComponentStrikeAbility.h
// Author: Bradley Cooper
//
// King Pin's ability to fire a linear projectile.
//----------------------------------------------------------------------------

#ifndef COMPONENT_STRIKE_ABILITY_H
#define COMPONENT_STRIKE_ABILITY_H

#include "ComponentAIDriver.h"

#include <tinyxml2.h>
#include <string>

#include "Common.h"
#include "ComponentAbility.h"
#include "ComponentProjectile.h"
#include "ComponentKartController.h"
#include "ComponentSimplePhysics.h"
#include "AudioPlayer.h"

namespace Kartaclysm
{
	class ComponentStrikeAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_StrikeAbility"; }

		// Destruction.
		virtual ~ComponentStrikeAbility();

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
		ComponentStrikeAbility(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strProjectileXML,
			float p_fDuration
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }
		void OnHitCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strProjectileXML,
			float& p_fDuration
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strProjectileXML;
		float m_fDuration;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;

		// Delegate function to register with EventManager for strike ability hit event
		std::function<void(const HeatStroke::Event*)>* m_pOnHitDelegate;
	};
}

#endif // COMPONENT_STRIKE_ABILITY_H