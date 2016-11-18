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
		float p_fWidth,
		float p_fHeight,
		float p_fLabelOffset
		) :
		ComponentRenderable(p_pGameObject),
		m_mFont(p_strFontFilePath),
		m_mLabelTextBox(&m_mFont, "LAP", p_fWidth, p_fHeight),
		m_mLapTextBox(&m_mFont, "1/3", p_fWidth, p_fHeight),
		m_fLabelOffset(p_fLabelOffset),
		m_strEventName("Player0_Lap_HUD")
	{
		m_mLabelTextBox.SetColour(glm::vec4(1.0, 0.5, 0.0, 1.0)); // orange
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mLapTextBox);

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudLapCount::LapCountCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
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
		float fWidth(0.0f);
		float fHeight(0.0f);
		float fLabelOffset(999.999f);

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fWidth, fHeight, fLabelOffset);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fWidth, fHeight, fLabelOffset);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(fWidth != 0.0f);
		assert(fHeight != 0.0f);
		assert(fLabelOffset != 999.999f);

		// Now we can create and return the Component.
		return new ComponentHudLapCount(
			p_pGameObject,
			strFontFilePath,
			fWidth,
			fHeight,
			fLabelOffset
			);
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

		m_mLapTextBox.SetText(std::to_string(iCurrent) + "/" + std::to_string(iTotal));
	}

	void ComponentHudLapCount::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		float& p_fWidth,
		float& p_fHeight,
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
			else if (strcmp(szNodeName, "Width") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fWidth);
			}
			else if (strcmp(szNodeName, "Height") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fHeight);
			}
			else if (strcmp(szNodeName, "LabelOffset") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fLabelOffset);
			}
		}
	}
}