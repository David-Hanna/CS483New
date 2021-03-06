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
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_mChargesTextBox(m_pFont, ""),
		m_mControlsTextBox(m_pFont, ""),
		m_pActiveSprite(nullptr),
		m_pInactiveSprite(new HeatStroke::SpriteInstance("Assets/Hud/Abilities/null_ability.mtl", "null_ability")),
		m_strEventName(p_strAbility),
		m_bReady(false),
		m_bHasCharges(false)
	{
		HeatStroke::SceneManager::Instance()->AddSpriteInstance(m_pInactiveSprite);

		m_pControlsDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudAbility::ControlsCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("PlayerInputMap", m_pControlsDelegate);
	}

	ComponentHudAbility::~ComponentHudAbility()
	{
		HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(m_pActiveSprite);
		HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(m_pInactiveSprite);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mChargesTextBox);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mControlsTextBox);

		DELETE_IF(m_pInactiveSprite);
		DELETE_IF(m_pActiveSprite);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName + "_Icon", m_pIconDelegate);
		delete m_pIconDelegate;
		m_pIconDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener("PlayerInputMap", m_pControlsDelegate);
		delete m_pControlsDelegate;
		m_pIconDelegate = m_pControlsDelegate;
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
				HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(m_pInactiveSprite);
				HeatStroke::SceneManager::Instance()->AddSpriteInstance(m_pActiveSprite);
			}
		}
		else if (m_bReady)
		{
			m_bReady = false;
			HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(m_pActiveSprite);
			HeatStroke::SceneManager::Instance()->AddSpriteInstance(m_pInactiveSprite);
		}

		// Charge textbox
		if (iCharges > 0)
		{
			if (!m_bHasCharges)
			{
				m_bHasCharges = true;
				HeatStroke::SceneManager::Instance()->AddTextBox(&m_mChargesTextBox);
			}

			if (iCharges == iMaxCharges)
			{
				m_mChargesTextBox.SetColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}
			else
			{
				m_mChargesTextBox.SetColour(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			}

			m_mChargesTextBox.SetText(std::to_string(iCharges));
			m_mChargesTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
				glm::scale(glm::vec3(0.05f, 0.05f, 1.0f)) *
				glm::translate(glm::vec3(10.0f, 32.0f, 0.0f)));
		}
		else if (m_bHasCharges)
		{
			m_bHasCharges = false;
			HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mChargesTextBox);
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
			HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(m_pActiveSprite);
			delete m_pActiveSprite;
			m_pActiveSprite = nullptr;
		}
		if (m_pInactiveSprite != nullptr)
		{
			HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(m_pInactiveSprite);
			delete m_pInactiveSprite;
			m_pInactiveSprite = nullptr;
		}

		m_pActiveSprite = new HeatStroke::SpriteInstance(strActiveMTLFileName, strActiveMaterialName);
		m_pInactiveSprite = new HeatStroke::SpriteInstance(strInactiveMTLFileName, strInactiveMaterialName);
		HeatStroke::SceneManager::Instance()->AddSpriteInstance(m_pInactiveSprite);
	}

	void ComponentHudAbility::ControlsCallback(const HeatStroke::Event* p_pEvent)
	{
		int iPlayer;
		std::string strControls = "";

		p_pEvent->GetRequiredIntParameter("Player", iPlayer);
		p_pEvent->GetOptionalStringParameter(m_strEventName.substr(m_strEventName.find_last_of('_') + 1), strControls, strControls);

		if (iPlayer == atoi(m_strEventName.substr(6, m_strEventName.find('_')).c_str()))
		{
			m_mControlsTextBox.SetText(strControls);
			m_mControlsTextBox.SetColour(glm::vec4(1.0, 0.5, 0.0, 1.0)); // orange
			m_mControlsTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
				glm::scale(glm::vec3(0.035f, 0.035f, 1.0f)) *
				glm::translate(glm::vec3(-20.0f, -30.0f, 0.0f)));
		}

		// Make sure textbox is added to scene only once
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mControlsTextBox);
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mControlsTextBox);
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