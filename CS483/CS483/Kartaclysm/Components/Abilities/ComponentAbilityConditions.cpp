//----------------------------------------------------------------------------
// ComponentAbilityConditions.h
// Author: Bradley Cooper
//
// Component that handles the conditions for activating an ability.
//----------------------------------------------------------------------------

#include "ComponentAbilityConditions.h"

namespace Kartaclysm
{
	ComponentAbilityConditions::ComponentAbilityConditions(
		HeatStroke::GameObject* p_pGameObject,
		float p_fCooldown,
		int p_iCharges,
		bool p_bUseOnGround,
		bool p_bUseInAir,
		bool p_bUseWhileSpinningOut)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fMaxCooldown(p_fCooldown),
		m_fCurrentCooldown(p_fCooldown),
		m_iMaxCharges(p_iCharges),
		m_iCurrentCharges(std::min(0, p_iCharges)),
		m_bUseOnGround(p_bUseOnGround),
		m_bUseInAir(p_bUseInAir),
		m_bUseWhileSpinningOut(p_bUseWhileSpinningOut)
	{
	}

	ComponentAbilityConditions::~ComponentAbilityConditions()
	{
	}

	HeatStroke::Component* ComponentAbilityConditions::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults (some for testing purposes only)
		float fCooldown = 1.0f;
		int iCharges = 3;
		bool bUseOnGround = true;
		bool bUseInAir = false;
		bool bUseWhileSpinningOut = false;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			//ParseNode(p_pBaseNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}
		if (p_pOverrideNode != nullptr)
		{
			//ParseNode(p_pOverrideNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}

		return new ComponentAbilityConditions(
			p_pGameObject,
			fCooldown,
			iCharges,
			bUseOnGround,
			bUseInAir,
			bUseWhileSpinningOut
			);
	}

	void ComponentAbilityConditions::Init()
	{
		// TO DO, this is hardcoded for the first ability of the first player
		m_strEventName = "Player0_HUD_KartAbility1";
	}

	void ComponentAbilityConditions::Update(const float p_fDelta)
	{
		// Decrease cooldown timer
		if (m_fMaxCooldown >= 0.0f && m_fCurrentCooldown > 0.0f)
		{
			m_fCurrentCooldown -= p_fDelta;

			if (m_fCurrentCooldown <= 0.0f)
			{
				SendEventForHud();
			}
		}
	}

	bool ComponentAbilityConditions::CanActivate() const
	{
		// Assume the ability can be activated, and only return false if a condition is broke
		if (m_fCurrentCooldown > 0.0f)
		{
			return false;
		}
		if (m_iCurrentCharges == 0)
		{
			return false;
		}

		// TO DO, figure out the way to do special conditions (ground/air/spinning out)

		SendEventForHud();

		return true;
	}

	void ComponentAbilityConditions::ResetCooldown()
	{
		// Do not reset cooldown if it cannot be reduced
		if (m_fMaxCooldown > 0.0f)
		{
			m_fCurrentCooldown = m_fMaxCooldown;
			SendEventForHud();
		}
	}

	void ComponentAbilityConditions::ReduceCooldown(float p_fSeconds)
	{ 
		// Do not reduce cooldown if it cannot be reduced
		if (m_fMaxCooldown > 0.0f)
		{
			m_fCurrentCooldown = std::fminf(0.0f, m_fCurrentCooldown - p_fSeconds);
			SendEventForHud();
		}
	}

	void ComponentAbilityConditions::AddCharge()
	{ 
		if (m_iCurrentCharges < m_iMaxCharges)
		{
			m_iCurrentCharges++;
			SendEventForHud();
		}
	}

	void ComponentAbilityConditions::RemoveCharge()
	{ 
		if (m_iCurrentCharges > 0)
		{
			m_iCurrentCharges--;
			SendEventForHud();
		}
	}

	void ComponentAbilityConditions::SendEventForHud() const
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event(m_strEventName);
		pEvent->SetFloatParameter("Cooldown", m_fCurrentCooldown);
		pEvent->SetIntParameter("Charges", m_iCurrentCharges);
		pEvent->SetIntParameter("MaxCharges", m_iMaxCharges);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}
}