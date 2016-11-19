//----------------------------------------------------------------------------
// ComponentHudPopup.cpp
// Author: Bradley Cooper
//
// Component that displays Frames per Second (FPS) on the HUD.
//----------------------------------------------------------------------------

#include "ComponentHudPopup.h"

namespace Kartaclysm
{
	ComponentHudPopup::ComponentHudPopup(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath,
		const std::string& p_strEventName,
		const std::string& p_strMessage
		) :
		ComponentRenderable(p_pGameObject),
		m_mFont(p_strFontFilePath),
		m_mTextBox(&m_mFont, p_strMessage),
		m_strEventName(p_strEventName),
		m_bDisplaying(false)
	{
		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudPopup::ToggleCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	ComponentHudPopup::~ComponentHudPopup()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudPopup::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strFontFilePath("");
		std::string strEventName("");
		std::string strMessage("");

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, strEventName, strMessage);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, strEventName, strMessage);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(strEventName != "");
		assert(strMessage != "");

		// Now we can create and return the Component.
		return new ComponentHudPopup(
			p_pGameObject,
			strFontFilePath,
			strEventName,
			strMessage
			);
	}

	void ComponentHudPopup::SyncTransform()
	{
		m_mTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	}


	void ComponentHudPopup::ToggleCallback(const HeatStroke::Event* p_pEvent)
	{
		int iDisplay;
		p_pEvent->GetRequiredIntParameter("Display", iDisplay);
		
		if (iDisplay == 0)
		{
			m_bDisplaying = false;
			HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);
		}
		else if (!m_bDisplaying)
		{
			m_bDisplaying = true;
			HeatStroke::SceneManager::Instance()->AddTextBox(&m_mTextBox);
		}

	}

	void ComponentHudPopup::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		std::string& p_strEventName,
		std::string& p_strMessage)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Popup") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FontFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
			}
			else if (strcmp(szNodeName, "Event") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "value", p_strEventName);
			}
			else if (strcmp(szNodeName, "Message") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "value", p_strMessage);
			}
		}
	}
}