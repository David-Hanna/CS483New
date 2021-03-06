//----------------------------------------------------------------------------
// ComponentMaintainAbility.cpp
// Author: Bradley Cooper
//
// Juggernaut's ability to use charges to negate attacks and affect stats.
//----------------------------------------------------------------------------

#include "ComponentMaintainAbility.h"

namespace Kartaclysm
{
	ComponentMaintainAbility::ComponentMaintainAbility(
		HeatStroke::GameObject* p_pGameObject,
		int p_iStartCharges,
		int p_iMaxCharges)
		:
		ComponentAbility(p_pGameObject),
		m_iCurrentCharges(p_iStartCharges),
		m_iMaxCharges(p_iMaxCharges),
		m_pAbilityDelegate(nullptr),
		m_pChargeDelegate(nullptr),
		m_strChargeEventName(m_strPlayerX + "_ArmorPlate")
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentMaintainAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_KartAbility2", m_pAbilityDelegate);

		// Listen to any events that change the charge count for armor plates
		m_pChargeDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentMaintainAbility::ChargeCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strChargeEventName, m_pChargeDelegate);
	}

	ComponentMaintainAbility::~ComponentMaintainAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_KartAbility2", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strChargeEventName, m_pChargeDelegate);
		delete m_pChargeDelegate;
		m_pChargeDelegate = nullptr;
	}

	HeatStroke::Component* ComponentMaintainAbility::CreateComponent(
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

		return new ComponentMaintainAbility(
			p_pGameObject,
			iStartCharges,
			iMaxCharges
			);
	}

	void ComponentMaintainAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");

		// Register with AI (if applicable)
		HeatStroke::GameObject* pAIKartObject = GetGameObject()->GetManager()->GetGameObject(m_strPlayerX);
		ComponentAIDriver* pAIDriver = static_cast<ComponentAIDriver*>(pAIKartObject->GetComponent("GOC_AIDriver"));
		if (pAIDriver != nullptr)
		{
			pAIDriver->RegisterComponentAbility(this);
		}
	}

	void ComponentMaintainAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			// Yes, this sends an event that it listens to. When an event is negated,
			// this event is also sent from that source too. Hence, it needs to listen to itself
			HeatStroke::Event* pEvent = new HeatStroke::Event(m_strChargeEventName);
			pEvent->SetIntParameter("ArmorChange", 1);
			pEvent->SetIntParameter("Negated", 0);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

			HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/juggernaut_maintain.flac");
		}
	}

	void ComponentMaintainAbility::AICheckCondition(HeatStroke::Component* p_pAIDriver)
	{
		ComponentAIDriver* pAIDriver = static_cast<ComponentAIDriver*>(p_pAIDriver);
		if (pAIDriver != nullptr)
		{
			if (m_pConditions->CanActivate())
			{
				Activate();
			}
		}
	}

	void ComponentMaintainAbility::ChargeCallback(const HeatStroke::Event* p_pEvent)
	{
		int iChange = 0;
		int iNegated = 0;

		p_pEvent->GetOptionalIntParameter("ArmorChange", iChange, 0);
		p_pEvent->GetOptionalIntParameter("Negated", iNegated, 0);

		if (iChange == 1)
		{
			m_iCurrentCharges++;
			assert(m_iCurrentCharges <= m_iMaxCharges);
		}
		else if (iChange == -1)
		{
			m_iCurrentCharges--;
			assert(m_iCurrentCharges <= m_iMaxCharges);
		}

		if (iNegated == 1)
		{
			m_iCurrentCharges--;
			assert(m_iCurrentCharges >= 0);
		}

		m_pConditions->SetSpecialCondition(m_iCurrentCharges != m_iMaxCharges);
	}

	void ComponentMaintainAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		int& p_iStartCharges,
		int& p_iMaxCharges)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_MaintainAbility") == 0);

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