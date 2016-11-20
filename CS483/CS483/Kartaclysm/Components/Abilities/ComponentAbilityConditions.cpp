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
		int p_iCharges)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fMaxCooldown(p_fCooldown),
		m_fCurrentCooldown(p_fCooldown),
		m_iMaxCharges(p_iCharges),
		m_iCurrentCharges(std::min(0, p_iCharges))
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
		float fCooldown = -1.0f;
		int iCharges = -1;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fCooldown, iCharges);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fCooldown, iCharges);
		}

		// Check that we got everything we needed.
		assert(fCooldown != -1.0f);
		assert(iCharges >= -1);

		return new ComponentAbilityConditions(
			p_pGameObject,
			fCooldown,
			iCharges
			);
	}

	void ComponentAbilityConditions::Init()
	{
		//GUID follows format of "Player0_KartAbility1": needs to be "Player0_HUD_KartAbility1"
		m_strEventName = GetGameObject()->GetGUID();
		int iFind = m_strEventName.find('_');
		m_strEventName = m_strEventName.substr(0, iFind) + "_HUD" + m_strEventName.substr(iFind);
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

			if (strcmp(szNodeName, "Cooldown") == 0)
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