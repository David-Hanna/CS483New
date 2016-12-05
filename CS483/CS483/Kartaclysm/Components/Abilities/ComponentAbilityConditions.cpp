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
		const std::string& p_strAbility,
		float p_fCooldown,
		int p_iCharges)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_strEventName(p_strAbility),
		m_fMaxCooldown(p_fCooldown),
		m_fCurrentCooldown(p_fCooldown),
		m_iMaxCharges(p_iCharges),
		m_iCurrentCharges(std::min(0, p_iCharges)),
		m_bSendEvent(false)
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
		std::string strAbility("");
		float fCooldown = -2.0f;
		int iCharges = -1;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strAbility, fCooldown, iCharges);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strAbility, fCooldown, iCharges);
		}

		// Check that we got everything we needed.
		assert(strAbility != "");
		assert(fCooldown >= 0.0f);
		assert(iCharges >= -1);

		return new ComponentAbilityConditions(
			p_pGameObject,
			strAbility,
			fCooldown,
			iCharges
			);
	}

	void ComponentAbilityConditions::Init()
	{
		//GUID follows format of "Player0": needs to be "Player0_HUD_KartAbility1"
		std::string strPlayerX = GetGameObject()->GetParent()->GetParent()->GetGUID();
		m_strEventName = strPlayerX + "_HUD_" + m_strEventName;
	}

	void ComponentAbilityConditions::Update(const float p_fDelta)
	{
		// Decrease cooldown timer
		if (m_fMaxCooldown >= 0.0f && m_fCurrentCooldown > 0.0f)
		{
			m_fCurrentCooldown -= p_fDelta;


			if (m_fCurrentCooldown <= 0.0f)
			{
				m_bSendEvent = true;
			}
		}

		// Send HUD event if necessary
		if (m_bSendEvent)
		{
			SendEventForHud();
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

		m_bSendEvent = true;

		return true;
	}

	void ComponentAbilityConditions::ResetCooldown()
	{
		if (m_fMaxCooldown > 0.0f)
		{
			m_fCurrentCooldown = m_fMaxCooldown;
			m_bSendEvent = true;
		}
	}

	void ComponentAbilityConditions::ReduceCooldown(float p_fSeconds)
	{ 
		if (m_fMaxCooldown > 0.0f)
		{
			m_fCurrentCooldown = std::fminf(0.0f, m_fCurrentCooldown - p_fSeconds);
			m_bSendEvent = true;
		}
	}

	void ComponentAbilityConditions::AddCharge()
	{ 
		if (m_iCurrentCharges < m_iMaxCharges)
		{
			m_iCurrentCharges++;
			m_bSendEvent = true;
		}
	}

	void ComponentAbilityConditions::RemoveCharge()
	{ 
		if (m_iCurrentCharges > 0)
		{
			m_iCurrentCharges--;
			m_bSendEvent = true;
		}
	}

	void ComponentAbilityConditions::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strAbility,
		float& p_fCooldown,
		int& p_iCharges)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_AbilityConditions") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Ability") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "value", p_strAbility);
			}
			else if (strcmp(szNodeName, "Cooldown") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fCooldown);
			}
			else if (strcmp(szNodeName, "Charges") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iCharges);
			}
		}
	}

	void ComponentAbilityConditions::SendEventForHud() const
	{
		m_bSendEvent = false;
		HeatStroke::Event* pEvent = new HeatStroke::Event(m_strEventName);
		pEvent->SetFloatParameter("Cooldown", m_fCurrentCooldown);
		pEvent->SetFloatParameter("MaxCooldown", m_fMaxCooldown);
		pEvent->SetIntParameter("Charges", m_iCurrentCharges);
		pEvent->SetIntParameter("MaxCharges", m_iMaxCharges);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}
}
