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
		HeatStroke::GameObject* p_pGameObject)		
		:
		ComponentAbility(p_pGameObject)
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

		//// Defaults
		//float fDuration = 0.0f;

		//if (p_pBaseNode != nullptr)
		//{
		//	ParseNode(p_pBaseNode, fDuration);
		//}
		//if (p_pOverrideNode != nullptr)
		//{
		//	ParseNode(p_pOverrideNode, fDuration);
		//}

		//// Check that we got everything we needed.
		//assert(fDuration != 0.0f);

		return new ComponentWheelieAbility(
			p_pGameObject
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
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentWheelieAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_WheelieAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			/*if (strcmp(szNodeName, "Power") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fPower);
			}*/
		}
	}
}