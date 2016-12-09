//----------------------------------------------------------------------------
// ComponentBedazzleAbility.h
// Author: Bradley Cooper
//
// Cleopapa's ability to spin out nearby racers.
//----------------------------------------------------------------------------

#ifndef COMPONENT_BEDAZZLE_ABILITY_H
#define COMPONENT_BEDAZZLE_ABILITY_H

#include <tinyxml2.h>
#include <string>

#include "ComponentAbility.h"
#include "ComponentProjectile.h"

namespace Kartaclysm
{
	class ComponentBedazzleAbility : public ComponentAbility
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_BedazzleAbility"; }

		// Destruction.
		virtual ~ComponentBedazzleAbility();

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
		ComponentBedazzleAbility(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strProjectileXML
			);

		void AbilityCallback(const HeatStroke::Event* p_pEvent) { Activate(); }
		void OnHitCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strProjectileXML
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strProjectileXML;

		// Prevent querying the GameObject for the ComponentAbilityConditions
		ComponentAbilityConditions* m_pConditions;

		// Delegate function to register with EventManager for ability activation
		std::function<void(const HeatStroke::Event*)>* m_pAbilityDelegate;

		// Delegate function to register with EventManager for bedazzle ability hit event
		std::function<void(const HeatStroke::Event*)>* m_pOnHitDelegate;
	};
}

#endif // COMPONENT_BEDAZZLE_ABILITY_H