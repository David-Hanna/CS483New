//----------------------------------------------------------------------------
// ComponentTinkerAbility.cpp
// Author: Bradley Cooper
//
// Clockmaker's ability to add charges to his clock bomb.
//----------------------------------------------------------------------------

#include "ComponentTinkerAbility.h"

namespace Kartaclysm
{
	ComponentTinkerAbility::ComponentTinkerAbility(
		HeatStroke::GameObject* p_pGameObject,
		int p_iStartCharges,
		int p_iMaxCharges)
		:
		ComponentAbility(p_pGameObject),
		m_iCurrentCharges(p_iStartCharges),
		m_iMaxCharges(p_iMaxCharges),
		m_pAbilityDelegate(nullptr),
		m_pChargeDelegate(nullptr),
		m_strChargeEventName(m_strPlayerX + "_ClockCharges")
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTinkerAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_DriverAbility2", m_pAbilityDelegate);

		// Listen to any events that change the charge count for armor plates
		m_pChargeDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTinkerAbility::ChargeCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strChargeEventName, m_pChargeDelegate);
	}

	ComponentTinkerAbility::~ComponentTinkerAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_DriverAbility2", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strChargeEventName, m_pChargeDelegate);
		delete m_pChargeDelegate;
		m_pChargeDelegate = nullptr;
	}

	HeatStroke::Component* ComponentTinkerAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		int iStartCharges = -1;
		int iMaxCharges = 0;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, iStartCharges, iMaxCharges);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, iStartCharges, iMaxCharges);
		}

		// Check that we got everything we needed.
		assert(iStartCharges != -1);
		assert(iMaxCharges != 0);

		return new ComponentTinkerAbility(
			p_pGameObject,
			iStartCharges,
			iMaxCharges
			);
	}

	void ComponentTinkerAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
	}

	void ComponentTinkerAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			HeatStroke::Event* pActivateEvent = new HeatStroke::Event("AbilityUse");
			pActivateEvent->SetStringParameter("Originator", m_strPlayerX);
			pActivateEvent->SetStringParameter("Ability", "Tinker");
			HeatStroke::EventManager::Instance()->TriggerEvent(pActivateEvent);

			// Yes, this sends an event that it also listens to.
			HeatStroke::Event* pChargeEvent = new HeatStroke::Event(m_strChargeEventName);
			pChargeEvent->SetIntParameter("Increase", 1);
			pChargeEvent->SetIntParameter("Reset", 0);
			HeatStroke::EventManager::Instance()->TriggerEvent(pChargeEvent);
		}
	}

	void ComponentTinkerAbility::ChargeCallback(const HeatStroke::Event* p_pEvent)
	{
		int iIncrease = 0;
		int iReset = 0;

		p_pEvent->GetOptionalIntParameter("Increase", iIncrease, 0);
		p_pEvent->GetOptionalIntParameter("Reset", iReset, 0);

		if (iIncrease == 1)
		{
			m_iCurrentCharges++;
			assert(m_iCurrentCharges <= m_iMaxCharges);
		}

		if (iReset == 1)
		{
			m_iCurrentCharges = 0;
		}

		m_pConditions->SetSpecialCondition(m_iCurrentCharges != m_iMaxCharges);
	}

	void ComponentTinkerAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		int& p_iStartCharges,
		int& p_iMaxCharges)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_TinkerAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "MaxCharges") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iMaxCharges);
			}
			else if (strcmp(szNodeName, "StartCharges") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iStartCharges);
			}
		}
	}
}