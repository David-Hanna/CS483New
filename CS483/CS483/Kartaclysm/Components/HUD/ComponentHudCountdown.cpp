//----------------------------------------------------------------------------
// ComponentHudCountdown.cpp
// Author: Bradley Cooper
//
// Component that displays beginning race countdown.
//----------------------------------------------------------------------------

#include "ComponentHudCountdown.h"

namespace Kartaclysm
{
	ComponentHudCountdown::ComponentHudCountdown(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath
		) :
		ComponentRenderable(p_pGameObject),
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_mTextBox(m_pFont, "")
	{
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mTextBox);

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudCountdown::CountdownCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Countdown_HUD", m_pDelegate);
	}

	ComponentHudCountdown::~ComponentHudCountdown()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);

		HeatStroke::EventManager::Instance()->RemoveListener("Countdown_HUD", m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudCountdown::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strFontFilePath("");

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");

		// Now we can create and return the Component.
		return new ComponentHudCountdown(
			p_pGameObject,
			strFontFilePath
			);
	}

	void ComponentHudCountdown::SyncTransform()
	{
		m_mTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	}

	void ComponentHudCountdown::CountdownCallback(const HeatStroke::Event* p_pEvent)
	{
		/*HeatStroke::Event* pHudEvent = new HeatStroke::Event("Countdown_HUD");
		pHudEvent->SetIntParameter("Countdown", static_cast<int>(ceilf(m_fTimer)));
		HeatStroke::EventManager::Instance()->TriggerEvent(pHudEvent);*/

		int iCountdown;
		p_pEvent->GetRequiredIntParameter("Countdown", iCountdown);

		if (iCountdown > 0)
		{
			m_mTextBox.SetText(std::to_string(iCountdown));
		}
		else
		{
			HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);
		}
	}

	void ComponentHudCountdown::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Countdown") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FontFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
			}
		}
	}
}