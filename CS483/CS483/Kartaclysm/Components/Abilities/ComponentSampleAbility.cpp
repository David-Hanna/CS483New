//----------------------------------------------------------------------------
// ComponentSampleAbility.h
// Author: Bradley Cooper
//
// Component that handles the conditions for activating an ability.
//----------------------------------------------------------------------------

#include "ComponentSampleAbility.h"

namespace Kartaclysm
{
	ComponentSampleAbility::ComponentSampleAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fChargeCooldown)
		:
		ComponentSampleAbility::ComponentAbility(p_pGameObject),
		m_fChargeCooldown(p_fChargeCooldown)
	{

	}

	ComponentSampleAbility::~ComponentSampleAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
	}

	HeatStroke::Component* ComponentSampleAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults
		float fChargeCooldown = 0.0f;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fChargeCooldown);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fChargeCooldown);
		}

		// Check that we got everything we needed.
		assert(fChargeCooldown != 0.0f);

		return new ComponentSampleAbility(
			p_pGameObject,
			fChargeCooldown
			);
	}

	void ComponentSampleAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");

		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentSampleAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
	}

	void ComponentSampleAbility::Update(const float p_fDelta)
	{
		if (m_fChargeCooldown > 0.0f)
		{
			m_fChargeCooldown -= p_fDelta;
		}
		else
		{
			m_fChargeCooldown = 3.0f;
			m_pConditions->AddCharge();
		}
	}

	void ComponentSampleAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();
			m_pConditions->RemoveCharge();
			m_fChargeCooldown = 6.0f;
		}
	}

	void ComponentSampleAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fChargeCooldown)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_SampleAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "ChargeCooldown") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fChargeCooldown);
			}
		}
	}
}
