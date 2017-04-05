//----------------------------------------------------------------------------
// ComponentBoostAbility.cpp
// Author: Bradley Cooper
//
// Speedster's ability to give a temporary speed boost.
//----------------------------------------------------------------------------

#include "ComponentBoostAbility.h"

namespace Kartaclysm
{
	ComponentBoostAbility::ComponentBoostAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fPower)
		:
		ComponentAbility(p_pGameObject),
		m_fPower(p_fPower)
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentBoostAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_KartAbility1", m_pAbilityDelegate);
	}

	ComponentBoostAbility::~ComponentBoostAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_KartAbility1", m_pAbilityDelegate);
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
		float fPower = 0.0f;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fPower);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fPower);
		}

		// Check that we got everything we needed.
		assert(fPower != 0.0f);

		return new ComponentBoostAbility(
			p_pGameObject,
			fPower
			);
	}

	void ComponentBoostAbility::Init()
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

	void ComponentBoostAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			//HeatStroke::Event* pEvent = new HeatStroke::Event(m_strPlayerX + "_Ability");
			HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetStringParameter("Ability", "Boost");
			pEvent->SetFloatParameter("Power", m_fPower);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentBoostAbility::AICheckCondition(HeatStroke::Component* p_pAIDriver)
	{
		ComponentAIDriver* pAIDriver = static_cast<ComponentAIDriver*>(p_pAIDriver);
		if (pAIDriver != nullptr)
		{
			if (m_pConditions->CanActivate() &&
				pAIDriver->AngleToNextNode() <= 0.5f &&
				rand() % 8 == 0)
			{
				Activate();
			}
		}
	}

	void ComponentBoostAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fPower)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_BoostAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Power") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fPower);
			}
		}
	}
}