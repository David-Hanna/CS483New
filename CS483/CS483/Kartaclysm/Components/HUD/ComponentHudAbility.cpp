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
		const std::string& p_strAbility
		) :
		HeatStroke::ComponentRenderable(p_pGameObject),
		m_mFont(p_strFontFilePath),
		m_mTextBox(&m_mFont, ""),
		m_pActiveSprite(nullptr),
		m_pInactiveSprite(new HeatStroke::Sprite("Assets/Hud/Abilities/null.mtl", "null_ability")),
		m_strEventName(p_strAbility),
		m_bReady(false),
		m_bHasCharges(false)
	{
		HeatStroke::SceneManager::Instance()->AddSprite(m_pInactiveSprite);
	}

	ComponentHudAbility::~ComponentHudAbility()
	{
		HeatStroke::SceneManager::Instance()->RemoveSprite(m_pActiveSprite);
		HeatStroke::SceneManager::Instance()->RemoveSprite(m_pInactiveSprite);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);

		if (m_pInactiveSprite != nullptr)
		{
			delete m_pInactiveSprite;
			m_pInactiveSprite = nullptr;
		}

		if (m_pActiveSprite != nullptr)
		{
			delete m_pActiveSprite;
			m_pActiveSprite = nullptr;
		}

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName + "_Icon", m_pIconDelegate);
		delete m_pIconDelegate;
		m_pIconDelegate = nullptr;
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
		std::string strAbility("");

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, strAbility);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strFontFilePath, strAbility);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(strAbility != "");

		// Now we can create and return the Component.
		return new ComponentHudAbility(
			p_pGameObject,
			strFontFilePath,
			strAbility
			);
	}

	void ComponentHudAbility::Init()
	{
		// event name follows "Player0_HUD_KartAbility1" format
		assert(GetGameObject()->GetParent() != nullptr && "HUD hierarchy error");
		m_strEventName = GetGameObject()->GetParent()->GetGUID() + "_" + m_strEventName;

		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pAbilityDelegate);

		m_pIconDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudAbility::IconCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName + "_Icon", m_pIconDelegate);
	}

	void ComponentHudAbility::SyncTransform()
	{
		if (m_bReady)
		{
			m_pActiveSprite->SetTransform(this->GetGameObject()->GetTransform().GetTransform());
		}
		else
		{
			m_pInactiveSprite->SetTransform(this->GetGameObject()->GetTransform().GetTransform());
		}
	}

	void ComponentHudAbility::AbilityCallback(const HeatStroke::Event* p_pEvent)
	{
		int iCharges, iMaxCharges, iSpecial;
		float fCooldown;

		p_pEvent->GetRequiredFloatParameter("Cooldown", fCooldown);
		//p_pEvent->GetRequiredFloatParameter("MaxCooldown", fMaxCooldown);
		p_pEvent->GetRequiredIntParameter("Charges", iCharges);
		p_pEvent->GetRequiredIntParameter("MaxCharges", iMaxCharges);
		p_pEvent->GetRequiredIntParameter("Special", iSpecial);

		// Ability icon
		if (fCooldown <= 0.0f && iCharges != 0 && iSpecial == 1)
		{
			if (!m_bReady)
			{
				m_bReady = true;
				HeatStroke::SceneManager::Instance()->RemoveSprite(m_pInactiveSprite);
				HeatStroke::SceneManager::Instance()->AddSprite(m_pActiveSprite);
			}
		}
		else if (m_bReady)
		{
			m_bReady = false;
			HeatStroke::SceneManager::Instance()->RemoveSprite(m_pActiveSprite);
			HeatStroke::SceneManager::Instance()->AddSprite(m_pInactiveSprite);
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

	void ComponentHudAbility::IconCallback(const HeatStroke::Event* p_pEvent)
	{
		std::string strActiveMTLFileName, strActiveMaterialName;
		std::string strInactiveMTLFileName, strInactiveMaterialName;
		p_pEvent->GetRequiredStringParameter("ActiveMTLFile", strActiveMaterialName);
		p_pEvent->GetRequiredStringParameter("ActiveMaterialName", strActiveMTLFileName);
		p_pEvent->GetRequiredStringParameter("InactiveMTLFile", strInactiveMaterialName);
		p_pEvent->GetRequiredStringParameter("InactiveMaterialName", strInactiveMTLFileName);

		if (m_pActiveSprite != nullptr)
		{
			HeatStroke::SceneManager::Instance()->RemoveSprite(m_pActiveSprite);
			delete m_pActiveSprite;
			m_pActiveSprite = nullptr;
		}
		if (m_pInactiveSprite != nullptr)
		{
			HeatStroke::SceneManager::Instance()->RemoveSprite(m_pInactiveSprite);
			delete m_pInactiveSprite;
			m_pInactiveSprite = nullptr;
		}

		m_pActiveSprite = new HeatStroke::Sprite(strActiveMTLFileName, strActiveMaterialName);
		m_pInactiveSprite = new HeatStroke::Sprite(strInactiveMTLFileName, strInactiveMaterialName);
		HeatStroke::SceneManager::Instance()->AddSprite(m_pInactiveSprite);
	}

	void ComponentHudAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
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
			else if (strcmp(szNodeName, "Ability") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strAbility);
			}
		}
	}
}