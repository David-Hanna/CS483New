//----------------------------------------------------------------------------
// ComponentArmorPlateAbility.cpp
// Author: Bradley Cooper
//
// Juggernaut's ability to use charges to negate attacks and affect stats.
//----------------------------------------------------------------------------

#include "ComponentArmorPlateAbility.h"

namespace Kartaclysm
{
	ComponentArmorPlateAbility::ComponentArmorPlateAbility(
		HeatStroke::GameObject* p_pGameObject
		)
		:
		ComponentAbility(p_pGameObject),
		m_pChargeDelegate(nullptr),
		m_bSentImmuneEvent(false),
		m_strChargeEventName(m_strPlayerX + "_ArmorPlate")
	{
		// Listen to any events that change the charge count
		m_pChargeDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentArmorPlateAbility::ChargeCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strChargeEventName, m_pChargeDelegate);
	}

	ComponentArmorPlateAbility::~ComponentArmorPlateAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strChargeEventName, m_pChargeDelegate);
		delete m_pChargeDelegate;
		m_pChargeDelegate = nullptr;
	}

	HeatStroke::Component* ComponentArmorPlateAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode);
		}

		// Check that we got everything we needed.

		return new ComponentArmorPlateAbility(
			p_pGameObject
			);
	}

	void ComponentArmorPlateAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
		m_pConditions->ResetCooldown(); // quick-fix to send HUD event to display charges
		
	}

	void ComponentArmorPlateAbility::Update(const float p_fDelta)
	{
		if (!m_bSentImmuneEvent && m_pConditions->CanActivate())
		{
			m_bSentImmuneEvent = true;
			Activate();
		}
	}

	void ComponentArmorPlateAbility::Activate()
	{
		// Send immunity event for kart controller
		HeatStroke::Event* pEvent = new HeatStroke::Event("Ability");
		pEvent->SetStringParameter("Originator", m_strPlayerX);
		pEvent->SetStringParameter("Ability", "Immune");
		pEvent->SetStringParameter("ListenEvent", m_strChargeEventName);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentArmorPlateAbility::ChargeCallback(const HeatStroke::Event* p_pEvent)
	{
		int iChange = 0;
		int iNegated = 0;

		p_pEvent->GetOptionalIntParameter("ArmorChange", iChange, 0);
		p_pEvent->GetOptionalIntParameter("Negated", iNegated, 0);

		if (iChange == 1)
		{
			m_pConditions->AddCharge();
		}
		else if (iChange == -1)
		{
			m_pConditions->RemoveCharge();
		}

		if (iNegated == 1)
		{
			iChange--;
			m_pConditions->RemoveCharge();
			m_pConditions->ResetCooldown();
		}

		if (iChange != 0)
		{
			// Charge count changed: send armor plate event for kart controller (changes stats)
			HeatStroke::Event* pEvent = new HeatStroke::Event("Ability");
			pEvent->SetStringParameter("Originator", m_strPlayerX);
			pEvent->SetStringParameter("Ability", "ArmorPlate");
			pEvent->SetIntParameter("Layers", m_pConditions->GetCharges());
			pEvent->SetIntParameter("MaxLayers", m_pConditions->GetMaxCharges());
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentArmorPlateAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode
		)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_ArmorPlateAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			/*
			if (strcmp(szNodeName, "Strength") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fStrength);
			}
			*/
		}
	}
}