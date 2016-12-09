//----------------------------------------------------------------------------
// ComponentWheelieAbility.cpp
// Author: Bradley Cooper
//
// Showoff's ability to reduce steering but gain speed.
//----------------------------------------------------------------------------

#include "ComponentWheelieAbility.h"

namespace Kartaclysm
{
	ComponentWheelieAbility::ComponentWheelieAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fStrength,
		float p_fDuration)		
		:
		ComponentAbility(p_pGameObject),
		m_fStrength(p_fStrength),
		m_fDuration(p_fDuration)
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentWheelieAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_KartAbility1", m_pAbilityDelegate);
	}

	ComponentWheelieAbility::~ComponentWheelieAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_KartAbility1", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
	}

	HeatStroke::Component* ComponentWheelieAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fStrength = 0.0;
		float fDuration = 0.0f;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fStrength, fDuration);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fStrength, fDuration);
		}

		// Check that we got everything we needed.
		assert(fStrength != 0.0f);
		assert(fDuration != 0.0f);

		return new ComponentWheelieAbility(
			p_pGameObject,
			fStrength,
			fDuration
			);
	}

	void ComponentWheelieAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
	}

	void ComponentWheelieAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetStringParameter("Ability", "Wheelie");
			pEvent->SetFloatParameter("Power", m_fStrength);
			pEvent->SetFloatParameter("Duration", m_fDuration);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentWheelieAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fStrength,
		float& p_fDuration)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_WheelieAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Strength") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fStrength);
			}
			else if (strcmp(szNodeName, "Duration") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fDuration);
			}
		}
	}
}