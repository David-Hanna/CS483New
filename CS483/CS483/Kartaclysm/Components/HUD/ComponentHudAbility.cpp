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
		const std::string& p_strFontFilePath,
		const std::string& p_strMTLFileName,
		const std::string& p_strMaterialName,
		const std::string& p_strAbility
		) :
		HeatStroke::ComponentRenderable(p_pGameObject),
		m_mFont(p_strFontFilePath),
		m_mTextBox(&m_mFont, ""),
		m_mActiveSprite(p_strMTLFileName, p_strMaterialName),
		m_mInactiveSprite(p_strMTLFileName.substr(0, p_strMTLFileName.find('.')) + "_inactive.mtl", p_strMaterialName + "_inactive"),
		m_strEventName("Player0_" + p_strAbility + "_HUD"),
		m_bReady(false),
		m_bHasCharges(false)
	{
		HeatStroke::SceneManager::Instance()->AddSprite(&m_mInactiveSprite);

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	ComponentHudAbility::~ComponentHudAbility()
	{
		HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mActiveSprite);
		HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mInactiveSprite);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);

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
		std::string strFontFilePath("");
		std::string strMTLFileName("");
		std::string strMaterialName("");
		std::string strAbility("");

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, strMTLFileName, strMaterialName, strAbility);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strFontFilePath, strMTLFileName, strMaterialName, strAbility);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(strMTLFileName != "");
		assert(strMaterialName != "");
		assert(strAbility != "");

		// Now we can create and return the Component.
		return new ComponentHudAbility(
			p_pGameObject,
			strFontFilePath,
			strMTLFileName,
			strMaterialName,
			strAbility
			);
	}

	void ComponentHudAbility::SyncTransform()
	{
		if (m_bReady)
		{
			m_mActiveSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
		}
		else
		{
			m_mInactiveSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
		}
	}

	void ComponentHudAbility::AbilityCallback(const HeatStroke::Event* p_pEvent)
	{
		int iCharges, iMaxCharges;
		float fCooldown;

		p_pEvent->GetRequiredFloatParameter("Cooldown", fCooldown);
		p_pEvent->GetOptionalIntParameter("Charges", iCharges, -1);
		p_pEvent->GetOptionalIntParameter("MaxCharges", iMaxCharges, -1);

		// Ability icon
		if (fCooldown <= 0.0f && iCharges != 0)
		{
			if (!m_bReady)
			{
				m_bReady = true;
				HeatStroke::SceneManager::Instance()->AddSprite(&m_mActiveSprite);
				HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mInactiveSprite);
			}
		}
		else if (m_bReady)
		{
			m_bReady = false;
			HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mActiveSprite);
			HeatStroke::SceneManager::Instance()->AddSprite(&m_mInactiveSprite);
		}

		// Charge textbox
		if (iCharges > 0)
		{
			if (!m_bHasCharges)
			{
				m_bHasCharges = true;
				HeatStroke::SceneManager::Instance()->AddTextBox(&m_mTextBox);
			}

			if (iCharges == iMaxCharges)
			{
				m_mTextBox.SetColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}
			else
			{
				m_mTextBox.SetColour(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			}

			m_mTextBox.SetText(std::to_string(iCharges));
			m_mTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
				glm::scale(glm::vec3(0.05f, 0.05f, 0.0f)) *
				glm::translate(glm::vec3(10.0f, 32.0f, 0.0f)));
		}
		else if (m_bHasCharges)
		{
			m_bHasCharges = false;
			HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);
		}
	}

	void ComponentHudAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		std::string& p_strMTLFileName,
		std::string& p_strMaterialName,
		std::string& p_strAbility)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Ability") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			// skip comments
			if (pChildElement->ToComment() != nullptr)
				continue;

			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FontFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
			}
			else if (strcmp(szNodeName, "MTLFileName") == 0)
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