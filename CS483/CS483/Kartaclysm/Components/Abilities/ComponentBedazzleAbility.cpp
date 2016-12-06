//----------------------------------------------------------------------------
// ComponentBedazzleAbility.cpp
// Author: Bradley Cooper
//
// Cleopapa's ability to spin out nearby racers.
//----------------------------------------------------------------------------

#include "ComponentBedazzleAbility.h"

namespace Kartaclysm
{
	ComponentBedazzleAbility::ComponentBedazzleAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fStrength)
		:
		ComponentAbility(p_pGameObject),
		m_fStrength(p_fStrength)
	{
	}

	ComponentBedazzleAbility::~ComponentBedazzleAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_KartAbility1", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
	}

	HeatStroke::Component* ComponentBedazzleAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fStrength = 0.0f;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fStrength);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fStrength);
		}

		// Check that we got everything we needed.
		assert(fStrength != 0.0f);

		return new ComponentBedazzleAbility(
			p_pGameObject,
			fStrength
			);
	}

	void ComponentBedazzleAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");

		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentBedazzleAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_KartAbility1", m_pAbilityDelegate);
	}

	void ComponentBedazzleAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			/*HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetStringParameter("Ability", "Bedazzle");
			pEvent->SetFloatParameter("Power", m_fStrength);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);*/
		}
	}

	void ComponentBedazzleAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fStrength)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_BedazzleAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Strength") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fStrength);
			}
		}
	}
}