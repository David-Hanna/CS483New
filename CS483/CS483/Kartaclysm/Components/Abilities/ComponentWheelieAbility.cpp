//----------------------------------------------------------------------------
// ComponentWheelieAbility.h
// Author: Bradley Cooper
//
// Showoff's ability to reduce steering but gain speed.
//----------------------------------------------------------------------------

#include "ComponentWheelieAbility.h"

namespace Kartaclysm
{
	ComponentWheelieAbility::ComponentWheelieAbility(
		HeatStroke::GameObject* p_pGameObject,
		int p_iStrength,
		float p_fDuration)		
		:
		ComponentAbility(p_pGameObject),
		m_iStrength(p_iStrength),
		m_fDuration(p_fDuration)
	{
	}

	ComponentWheelieAbility::~ComponentWheelieAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
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
		int iStrength = 0;
		float fDuration = 0.0f;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, iStrength, fDuration);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, iStrength, fDuration);
		}

		// Check that we got everything we needed.
		assert(iStrength != 0);
		assert(iStrength != 0.0f);

		return new ComponentWheelieAbility(
			p_pGameObject,
			iStrength,
			fDuration
			);
	}

	void ComponentWheelieAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");

		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentWheelieAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
	}

	void ComponentWheelieAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetStringParameter("Ability", "Wheelie");
			pEvent->SetIntParameter("Power", m_iStrength);
			pEvent->SetFloatParameter("Duration", m_fDuration);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentWheelieAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		int& p_iStrength,
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
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iStrength);
			}
			else if (strcmp(szNodeName, "Duration") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fDuration);
			}
		}
	}
}