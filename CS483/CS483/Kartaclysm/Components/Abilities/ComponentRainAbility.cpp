//----------------------------------------------------------------------------
// ComponentRainAbility.cpp
// Author: Bradley Cooper
//
// Cleopapa's ability to slow passing racers.
//----------------------------------------------------------------------------

#include "ComponentRainAbility.h"

namespace Kartaclysm
{
	ComponentRainAbility::ComponentRainAbility(
		HeatStroke::GameObject* p_pGameObject,
		float p_fPower,
		float p_fDuration)
		:
		ComponentAbility(p_pGameObject),
		m_iPreviousPosition(-1),
		m_fPower(p_fPower),
		m_fDuration(p_fDuration)
	{
		// Listen to other players passing
		m_pPassedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentRainAbility::PassedCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RaceStandingsUpdate", m_pPassedDelegate);
	}

	ComponentRainAbility::~ComponentRainAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RaceStandingsUpdate", m_pPassedDelegate);
		delete m_pPassedDelegate;
		m_pPassedDelegate = nullptr;
	}

	HeatStroke::Component* ComponentRainAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fPower = 0.0f;
		float fDuration = 0.0f;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fPower, fDuration);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fPower, fDuration);
		}

		// Check that we got everything we needed.
		assert(fPower > 0.0f);
		assert(fDuration > 0.0f);

		return new ComponentRainAbility(
			p_pGameObject,
			fPower,
			fDuration
			);
	}

	void ComponentRainAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
	}

	void ComponentRainAbility::PassedCallback(const HeatStroke::Event* p_pEvent)
	{
		if (m_pConditions->CanActivate())
		{
			int iPosition;
			p_pEvent->GetRequiredIntParameter(m_strPlayerX, iPosition);

			if (iPosition < m_iPreviousPosition && m_pConditions->CanActivate())
			{
				std::string strTarget;
				p_pEvent->GetRequiredStringParameter(std::to_string(iPosition + 1), strTarget);

				// TODO: Triggering event is not sent if passing by a player who is not on the same lap
				// TODO: Check distance for 'nearby' criteria?

				HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
				pEvent->SetStringParameter("Originator", m_strPlayerX);
				pEvent->SetStringParameter("Target", strTarget);
				pEvent->SetStringParameter("Ability", "Rain");
				pEvent->SetStringParameter("Effect", "Slow");
				pEvent->SetFloatParameter("Power", m_fPower);
				pEvent->SetFloatParameter("Duration", m_fDuration);
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
			}

			m_iPreviousPosition = iPosition;
		}
	}

	void ComponentRainAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fPower,
		float& p_fDuration)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_RainAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Power") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fPower);
			}
			else if (strcmp(szNodeName, "Duration") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fDuration);
			}
		}
	}
}