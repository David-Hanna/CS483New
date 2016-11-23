//----------------------------------------------------------------------------
// ComponentBoostAbility.h
// Author: Bradley Cooper
//
// Speedster's ability to give a temporary speed boost.
//----------------------------------------------------------------------------

#include "ComponentBoostAbility.h"

namespace Kartaclysm
{
	ComponentBoostAbility::ComponentBoostAbility(
		HeatStroke::GameObject* p_pGameObject,
		int p_iStrength)
		:
		ComponentAbility(p_pGameObject),
		m_iStrength(p_iStrength)
	{
	}

	ComponentBoostAbility::~ComponentBoostAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
	}

	HeatStroke::Component* ComponentBoostAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		int iStrength = 0;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, iStrength);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, iStrength);
		}

		// Check that we got everything we needed.
		assert(iStrength != 0);

		return new ComponentBoostAbility(
			p_pGameObject,
			iStrength
			);
	}

	void ComponentBoostAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
		
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentBoostAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
	}

	void ComponentBoostAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			HeatStroke::Event* pEvent = new HeatStroke::Event(m_strPlayerX + "_Ability");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetIntParameter("MaxSpeed", m_iStrength);
			pEvent->SetIntParameter("Acceleration", m_iStrength);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentBoostAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		int& p_iStrength)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_BoostAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Strength") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iStrength);
			}
		}
	}
}