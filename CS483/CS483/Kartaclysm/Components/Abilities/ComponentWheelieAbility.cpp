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
		float p_fStatusDuration)		
		:
		ComponentAbility(p_pGameObject),
		m_iStrength(p_iStrength),
		m_fStatusDuration(p_fStatusDuration)
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
		float fStatusDuration = 0.0f;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, iStrength, fStatusDuration);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, iStrength, fStatusDuration);
		}

		// Check that we got everything we needed.
		assert(iStrength != 0);
		assert(iStrength != 0.0f);

		return new ComponentWheelieAbility(
			p_pGameObject,
			iStrength,
			fStatusDuration
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

			HeatStroke::Event* pEvent = new HeatStroke::Event(m_strPlayerX + "_Ability");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetIntParameter("MaxSpeed", m_iStrength);
			pEvent->SetIntParameter("Acceleration", m_iStrength);
			pEvent->SetStringParameter("Status", "TurnReduce");
			pEvent->SetFloatParameter("StatusDuration", m_fStatusDuration);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentWheelieAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		int& p_iStrength,
		float& p_fStatusDuration)
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
			else if (strcmp(szNodeName, "TurnReduce") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "duration", p_fStatusDuration);
			}
		}
	}
}