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

#include "ComponentRacer.h"
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
		float GetMaxCooldown() const { return m_fMaxCooldown; }
		void ResetCooldown();
		void ReduceCooldown(float p_fSeconds);

		// Charge control
		int GetCharges() const { return m_iCurrentCharges; }
		int GetMaxCharges() const { return m_iMaxCharges; }
		void AddCharge();
		void RemoveCharge();
		void ResetCharges();

		// Manual control of a special condition
		void SetSpecialCondition(bool p_bSpecial) { m_bSpecial = p_bSpecial; m_bSendEvent = true; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAbilityConditions(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strAbility,
			float p_fCooldown,
			int p_iMaxCharges,
			int p_iStartCharges
			);

		virtual void SendEventForHud() const;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strAbility,
			float& p_fCooldown,
			int& p_iMaxCharges,
			int& p_iStartCharges);

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

		// Special condition manually controlled by ability
		bool	m_bSpecial;

		// Needed for a race completed check
		ComponentRacer* m_pRacer;
	};
}

#endif // COMPONENT_ABILITY_CONDITIONS_H