//----------------------------------------------------------------------------
// ComponentTurkeyAbility.cpp
// Author: Bradley Cooper
//
// Kingpin's ability to give a temporary speed boost.
//----------------------------------------------------------------------------

#include "ComponentTurkeyAbility.h"

namespace Kartaclysm
{
	ComponentTurkeyAbility::ComponentTurkeyAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fPower)
		:
		ComponentAbility(p_pGameObject),
		m_fPower(p_fPower)
	{
		// Listen to on hit event
		m_pOnHitDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTurkeyAbility::OnHitCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_StrikeHit", m_pOnHitDelegate);
	}

	ComponentTurkeyAbility::~ComponentTurkeyAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_StrikeHit", m_pOnHitDelegate);
		delete m_pOnHitDelegate;
		m_pOnHitDelegate = nullptr;
	}

	HeatStroke::Component* ComponentTurkeyAbility::CreateComponent(
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

		return new ComponentTurkeyAbility(
			p_pGameObject,
			fPower
			);
	}

	void ComponentTurkeyAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
		m_pConditions->SetSpecialCondition(false);
	}

	void ComponentTurkeyAbility::Activate()
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
		pEvent->SetStringParameter("Originator", m_strPlayerX);
		pEvent->SetStringParameter("Ability", "Boost");
		pEvent->SetFloatParameter("Power", m_fPower);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

		HeatStroke::AudioPlayer::Instance()->PlaySoundEffectFromFile("Assets/Sounds/kingpin_turkey.wav");

		m_pConditions->ResetCharges();
	}

	void ComponentTurkeyAbility::OnHitCallback(const HeatStroke::Event* p_pEvent)
	{
		m_pConditions->AddCharge();

		if (m_pConditions->GetCharges() == m_pConditions->GetMaxCharges())
		{
			m_pConditions->ResetCooldown();
			Activate();
		}
	}

	void ComponentTurkeyAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fPower)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_TurkeyAbility") == 0);

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