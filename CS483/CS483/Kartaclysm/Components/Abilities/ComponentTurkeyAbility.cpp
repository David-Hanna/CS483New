//----------------------------------------------------------------------------
// ComponentTurkeyAbility.h
// Author: Bradley Cooper
//
// Speedster's ability to give a temporary speed boost.
//----------------------------------------------------------------------------

#include "ComponentTurkeyAbility.h"

namespace Kartaclysm
{
	ComponentTurkeyAbility::ComponentTurkeyAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fStrength)
		:
		ComponentAbility(p_pGameObject),
		m_fStrength(p_fStrength)
	{
	}

	ComponentTurkeyAbility::~ComponentTurkeyAbility()
	{
		// TODO: Needs to have the same event string as a Strike hit
		HeatStroke::EventManager::Instance()->RemoveListener(GetGameObject()->GetGUID(), m_pStrikeDelegate);
		delete m_pStrikeDelegate;
		m_pStrikeDelegate = nullptr;
	}

	HeatStroke::Component* ComponentTurkeyAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fStrength = 0.0f;

		// All parameters are optional.
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

		return new ComponentTurkeyAbility(
			p_pGameObject,
			fStrength
			);
	}

	void ComponentTurkeyAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
		//TODO: Set Special condition to false

		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pStrikeDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTurkeyAbility::StrikeCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(GetGameObject()->GetGUID(), m_pStrikeDelegate);
	}

	void ComponentTurkeyAbility::Activate()
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
		pEvent->SetStringParameter("Originator", m_strPlayerX);
		pEvent->SetStringParameter("Ability", "Boost");
		pEvent->SetFloatParameter("Power", m_fStrength);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentTurkeyAbility::StrikeCallback(const HeatStroke::Event* p_pEvent)
	{
		m_pConditions->AddCharge();

		if (m_pConditions->GetCharges() == 3) // TODO: Change 3 to GetMaxCharges() when pulled in
		{
			m_pConditions->ResetCooldown();
			Activate();
		}
	}

	void ComponentTurkeyAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fStrength)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_TurkeyAbility") == 0);

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