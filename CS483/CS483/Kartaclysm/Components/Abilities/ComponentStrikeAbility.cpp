//----------------------------------------------------------------------------
// ComponentStrikeAbility.cpp
// Author: Bradley Cooper
//
// King Pin's ability to fire a linear projectile.
//----------------------------------------------------------------------------

#include "ComponentStrikeAbility.h"

namespace Kartaclysm
{
	ComponentStrikeAbility::ComponentStrikeAbility(
		HeatStroke::GameObject* p_pGameObject,
		std::string p_strProjectileXML)
		:
		ComponentAbility(p_pGameObject),
		m_strProjectileXML(p_strProjectileXML)
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentStrikeAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_DriverAbility1", m_pAbilityDelegate);

		// Listen to on hit event
		m_pOnHitDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentStrikeAbility::OnHitCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_StrikeHit", m_pOnHitDelegate);
	}

	ComponentStrikeAbility::~ComponentStrikeAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_DriverAbility1", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_StrikeHit", m_pAbilityDelegate);
		delete m_pOnHitDelegate;
		m_pOnHitDelegate = nullptr;
	}

	HeatStroke::Component* ComponentStrikeAbility::CreateComponent(
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

		return new ComponentStrikeAbility(
			p_pGameObject,
			strProjectileXML
			);
	}

	void ComponentStrikeAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
	}

	void ComponentStrikeAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			HeatStroke::GameObject* pStrike = GetGameObject()->GetManager()->CreateGameObject(m_strProjectileXML);
			pStrike->GetTransform().SetTranslation(GetGameObject()->GetTransform().GetTranslation());
			pStrike->GetTransform().SetRotation(GetGameObject()->GetTransform().GetRotation());

			ComponentProjectile* pProjectile = static_cast<ComponentProjectile*>(pStrike->GetComponent("GOC_Projectile"));
			assert(pProjectile != nullptr);
			pProjectile->SetOriginator(m_strPlayerX);
			pProjectile->SetOnHitEvent(m_strPlayerX + "_StrikeHit");

			// TODO: Absolute garbage way to handle this, but I'm not diving deep into the physics to change it
			ComponentKartController* pKart = static_cast<ComponentKartController*>(GetGameObject()->GetParent()->GetParent()->GetComponent("GOC_KartController"));
			ComponentSimplePhysics* pPhysics = static_cast<ComponentSimplePhysics*>(pStrike->GetComponent("GOC_SimplePhysics"));
			pPhysics->SetDirection(pKart->GetDirection());
		}
	}

	void ComponentStrikeAbility::OnHitCallback(const HeatStroke::Event* p_pEvent)
	{
		std::string strTargetGUID;
		p_pEvent->GetRequiredGameObjectParameter("Target", strTargetGUID);

		HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
		pEvent->SetStringParameter("Originator", m_strPlayerX);
		pEvent->SetStringParameter("Target", strTargetGUID);
		pEvent->SetStringParameter("Ability", "Strike");
		pEvent->SetStringParameter("Effect", "SpinOut");
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentStrikeAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strProjectileXML)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_StrikeAbility") == 0);

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