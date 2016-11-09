//----------------------------------------------------------------------------
// ComponentHudAbility.cpp
// Author: Bradley Cooper
//
// Component that handles an ability icon on a player's HUD.
//----------------------------------------------------------------------------

#include "ComponentHudAbility.h"

namespace Kartaclysm
{
	ComponentHudAbility::ComponentHudAbility(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strMTLFileName,
		const std::string& p_strMaterialName,
		const std::string& p_strAbility
		) :
		HeatStroke::ComponentRenderable(p_pGameObject),
		m_mSprite(p_strMTLFileName, p_strMaterialName),
		m_strEventName("Player0_" + p_strAbility + "_HUD"),
		m_bReady(false)
	{
		HeatStroke::SceneManager::Instance()->AddSprite(&m_mSprite);

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	ComponentHudAbility::~ComponentHudAbility()
	{
		HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mSprite);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strMTLFileName("");
		std::string strMaterialName("");
		std::string strAbility("");

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strMTLFileName, strMaterialName, strAbility);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strMTLFileName, strMaterialName, strAbility);
		}

		// Check that we got everything we needed.
		assert(strMTLFileName != "");
		assert(strMaterialName != "");
		assert(strAbility != "");

		// Now we can create and return the Component.
		return new ComponentHudAbility(
			p_pGameObject,
			strMTLFileName,
			strMaterialName,
			strAbility
			);
	}

	void ComponentHudAbility::AbilityCallback(const HeatStroke::Event* p_pEvent)
	{
		int iCharges = 0;
		float fCooldown = 0.0f;

		p_pEvent->GetRequiredFloatParameter("Cooldown", fCooldown);
		p_pEvent->GetRequiredIntParameter("Charges", iCharges);

		if (fCooldown <= 0.0f && iCharges != 0)
		{
			if (!m_bReady)
			{
				m_bReady = true;
				printf("add sprite\n");
				HeatStroke::SceneManager::Instance()->AddSprite(&m_mSprite);
			}
		}
		else
		{
			if (m_bReady)
			{
				m_bReady = false;
				printf("remove sprite\n");
				HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mSprite);
			}
		}
	}

	void ComponentHudAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strMTLFileName,
		std::string& p_strMaterialName,
		std::string& p_strAbility)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_Hud_Ability") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			// skip comments
			if (pChildElement->ToComment() != nullptr)
				continue;

			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "MTLFileName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strMTLFileName);
			}
			else if (strcmp(szNodeName, "MaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strMaterialName);
			}
			else if (strcmp(szNodeName, "Ability") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strAbility);
			}
		}
	}
}