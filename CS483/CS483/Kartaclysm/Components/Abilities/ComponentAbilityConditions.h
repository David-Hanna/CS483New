//----------------------------------------------------------------------------
// ComponentAbilityConditions.h
// Author: Bradley Cooper
//
// Component that handles the conditions for activating an ability.
//----------------------------------------------------------------------------

#ifndef COMPONENT_ABILITY_CONDITIONS_H
#define COMPONENT_ABILITY_CONDITIONS_H

#include <tinyxml2.h>
#include <string>

#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentAbilityConditions : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_AbilityConditions"; }
		virtual const std::string ComponentID() const override	{ return "GOC_AbilityConditions"; }

		// Destruction.
		virtual ~ComponentAbilityConditions();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;

		// Returns if the ability meets all conditions to be activated
		virtual bool CanActivate() const;

		// Cooldown timer control
		float GetCooldown() const { return m_fCurrentCooldown; }
		void ResetCooldown();
		void ReduceCooldown(float p_fSeconds);

		// Charge control
		int GetCharges() const { return m_iCurrentCharges; }
		void AddCharge();
		void RemoveCharge();

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAbilityConditions(
			HeatStroke::GameObject* p_pGameObject,
			float p_fCooldown,
			int p_iCharges
			);

		virtual void SendEventForHud() const;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& p_fCooldown,
			int& p_iCharges);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Name for the event to send to the HUD
		std::string m_strEventName;

		// Only send one event per multiple calls
		mutable bool m_bSendEvent;

		// Cooldown
		float	m_fMaxCooldown;
		float	m_fCurrentCooldown;

		// Charges
		int		m_iMaxCharges;
		int		m_iCurrentCharges;
	};
}

#endif // COMPONENT_ABILITY_CONDITIONS_H