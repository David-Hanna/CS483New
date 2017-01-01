//----------------------------------------------------------------------------
// ComponentHudLapCount.cpp
// Author: Bradley Cooper
//
// Component that handles player lap count.
//----------------------------------------------------------------------------

#include "ComponentHudLapCount.h"

namespace Kartaclysm
{
	ComponentHudLapCount::ComponentHudLapCount(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath,
		float p_fLabelOffset
		) :
		ComponentRenderable(p_pGameObject),
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_mLabelTextBox(m_pFont, "LAP"),
		m_mLapTextBox(m_pFont, "GO!"),
		m_fLabelOffset(p_fLabelOffset)
	{
		m_mLabelTextBox.SetColour(glm::vec4(1.0, 0.5, 0.0, 1.0)); // orange
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mLapTextBox);
	}

	ComponentHudLapCount::~ComponentHudLapCount()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mLapTextBox);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudLapCount::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strFontFilePath("");
		float fLabelOffset(999.999f);

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fLabelOffset);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fLabelOffset);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(fLabelOffset != 999.999f);

		// Now we can create and return the Component.
		return new ComponentHudLapCount(
			p_pGameObject,
			strFontFilePath,
			fLabelOffset
			);
	}

	void ComponentHudLapCount::Init()
	{
		// event name follows "Player0_HUD_Lap" format
		assert(GetGameObject()->GetParent() != nullptr && "HUD hierarchy error");
		m_strEventName = GetGameObject()->GetParent()->GetGUID() + "_Lap";

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudLapCount::LapCountCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	void ComponentHudLapCount::SyncTransform()
	{
		m_mLabelTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
			glm::translate(glm::vec3(m_fLabelOffset, 0.0f, 0.0f)));
		m_mLapTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	};

	void ComponentHudLapCount::LapCountCallback(const HeatStroke::Event* p_pEvent)
	{
		int iCurrent, iTotal;
		p_pEvent->GetRequiredIntParameter("Current", iCurrent);
		p_pEvent->GetRequiredIntParameter("Total", iTotal);

		if (iCurrent <= iTotal)
		{
			m_mLapTextBox.SetText(std::to_string(iCurrent) + "/" + std::to_string(iTotal));
		}
		else
		{
			m_mLapTextBox.SetText("FIN");
		}
	}

	void ComponentHudLapCount::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		float& p_fLabelOffset)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_LapCount") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FontFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
			}
			else if (strcmp(szNodeName, "LabelOffset") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fLabelOffset);
			}
		}
	}
}