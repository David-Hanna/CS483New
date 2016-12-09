//----------------------------------------------------------------------------
// ComponentBedazzleAbility.cpp
// Author: Bradley Cooper
//
// Cleopapa's ability to spin out nearby racers.
//----------------------------------------------------------------------------

#include "ComponentBedazzleAbility.h"

namespace Kartaclysm
{
	ComponentBedazzleAbility::ComponentBedazzleAbility(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strProjectileXML)
		:
		ComponentAbility(p_pGameObject),
		m_strProjectileXML(p_strProjectileXML)
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentBedazzleAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_DriverAbility1", m_pAbilityDelegate);

		// Listen to on hit event
		m_pOnHitDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentBedazzleAbility::OnHitCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_BedazzleHit", m_pOnHitDelegate);
	}

	ComponentBedazzleAbility::~ComponentBedazzleAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_DriverAbility1", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_BedazzleHit", m_pAbilityDelegate);
		delete m_pOnHitDelegate;
		m_pOnHitDelegate = nullptr;
	}

	HeatStroke::Component* ComponentBedazzleAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		std::string strProjectileXML("");

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strProjectileXML);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strProjectileXML);
		}

		// Check that we got everything we needed.
		assert(strProjectileXML != "");

		return new ComponentBedazzleAbility(
			p_pGameObject,
			strProjectileXML
			);
	}

	void ComponentBedazzleAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
	}

	void ComponentBedazzleAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			HeatStroke::GameObject* pStrike = GetGameObject()->GetManager()->CreateGameObject(m_strProjectileXML);
			pStrike->GetTransform().SetTranslation(GetGameObject()->GetTransform().GetTranslation());

			ComponentProjectile* pProjectile = static_cast<ComponentProjectile*>(pStrike->GetComponent("GOC_Projectile"));
			assert(pProjectile != nullptr);
			pProjectile->SetOriginator(m_strPlayerX);
			pProjectile->SetOnHitEvent(m_strPlayerX + "_BedazzleHit");
		}
	}

	void ComponentBedazzleAbility::OnHitCallback(const HeatStroke::Event* p_pEvent)
	{
		std::string strTargetGUID;
		p_pEvent->GetRequiredGameObjectParameter("Target", strTargetGUID);

		HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
		pEvent->SetStringParameter("Originator", m_strPlayerX);
		pEvent->SetStringParameter("Target", strTargetGUID);
		pEvent->SetStringParameter("Ability", "Bedazzle");
		pEvent->SetStringParameter("Effect", "Slow");
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentBedazzleAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strProjectileXML)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_BedazzleAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "ProjectileXML") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strProjectileXML);
			}
		}
	}
}