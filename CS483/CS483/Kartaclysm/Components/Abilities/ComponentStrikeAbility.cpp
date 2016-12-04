//----------------------------------------------------------------------------
// ComponentStrikeAbility.h
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
	}

	ComponentStrikeAbility::~ComponentStrikeAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
	}

	HeatStroke::Component* ComponentStrikeAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		std::string strProjectileXML("");

		// All parameters are optional.
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

		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentStrikeAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(GetGameObject()->GetGUID(), m_pAbilityDelegate);
	}

	void ComponentStrikeAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			// TODO: Move projectile and assign which event to send after colliding with racer
			HeatStroke::GameObject* pProjectile = GetGameObject()->GetManager()->CreateGameObject(m_strProjectileXML);
			pProjectile->GetTransform().SetTranslation(GetGameObject()->GetTransform().GetTranslation());
		}
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