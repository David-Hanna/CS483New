//----------------------------------------------------------------------------
// ComponentMenuSlider.cpp
// Author: Bradley Cooper
//
// Component for menu slider ranging from [0,100].
//----------------------------------------------------------------------------

#include "ComponentMenuSlider.h"

namespace Kartaclysm
{
	ComponentMenuSlider::ComponentMenuSlider(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath,
		const std::string& p_strLabelMessage,
		const std::string& p_strOutlineMTLFileName,
		const std::string& p_strOutlineMaterialName,
		const std::string& p_strSliderMTLFileName,
		const std::string& p_strSliderMaterialName,
		float p_fSliderOffset,
		int p_iChangeFactor,
		int p_iCurrent
		) :
		ComponentRenderable(p_pGameObject),
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_mLabelTextBox(m_pFont, p_strLabelMessage),
		m_mOutline(HeatStroke::SpriteManager::Instance()->GetOrCreateSprite(p_strOutlineMTLFileName, p_strOutlineMaterialName)),
		m_mSlider(HeatStroke::SpriteManager::Instance()->GetOrCreateSprite(p_strSliderMTLFileName, p_strSliderMaterialName)),
		m_fSliderOffset(p_fSliderOffset),
		m_iChangeFactor(p_iChangeFactor),
		m_iCurrent(p_iCurrent)
	{
		m_mLabelTextBox.SetTransform(GetGameObject()->GetTransform().GetTransform());
		m_mOutline.SetTransform(GetGameObject()->GetTransform().GetTransform() *
								glm::translate(glm::vec3(p_fSliderOffset, -4.0f, 0.0f)) *
								glm::scale(glm::vec3(50.0f, 10.0f, 1.0f)));
		UpdateSlider(); // Sets transform for Slider

		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->AddSpriteInstance(&m_mOutline);
		HeatStroke::SceneManager::Instance()->AddSpriteInstance(&m_mSlider);
	}

	ComponentMenuSlider::~ComponentMenuSlider()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(&m_mOutline);
		HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(&m_mSlider);
	}

	HeatStroke::Component* ComponentMenuSlider::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strFontFilePath("");
		std::string strLabelMessage("");
		std::string strOutlineMTLFileName("");
		std::string strOutlineMaterialName("");
		std::string strSliderMTLFileName("");
		std::string strSliderMaterialName("");
		float fSliderOffset(0.0f);
		int iChangeFactor(-1);
		int iCurrent(-1);

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, strLabelMessage, strOutlineMTLFileName, strOutlineMaterialName,
				strSliderMTLFileName, strSliderMaterialName, fSliderOffset, iChangeFactor, iCurrent);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, strLabelMessage, strOutlineMTLFileName, strOutlineMaterialName,
				strSliderMTLFileName, strSliderMaterialName, fSliderOffset, iChangeFactor, iCurrent);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(strLabelMessage != "");
		assert(strOutlineMTLFileName != "");
		assert(strOutlineMaterialName != "");
		assert(strSliderMTLFileName != "");
		assert(strSliderMaterialName != "");
		assert(iChangeFactor > 0 && iChangeFactor <= 100);
		assert(iCurrent >= 0 && iCurrent <= 100);

		// Now we can create and return the Component.
		return new ComponentMenuSlider(
			p_pGameObject,
			strFontFilePath,
			strLabelMessage,
			strOutlineMTLFileName,
			strOutlineMaterialName,
			strSliderMTLFileName,
			strSliderMaterialName,
			fSliderOffset,
			iChangeFactor,
			iCurrent
			);
	}

	bool ComponentMenuSlider::IncreaseSlider()
	{
		if (m_iCurrent < 100)
		{
			m_iCurrent += m_iChangeFactor;
			if (m_iCurrent > 100) m_iCurrent = 100;
			UpdateSlider();
			return true;
		}
		return false;
	}

	bool ComponentMenuSlider::DecreaseSlider()
	{
		if (m_iCurrent > 0)
		{
			m_iCurrent -= m_iChangeFactor;
			if (m_iCurrent < 0) m_iCurrent = 0;
			UpdateSlider();
			return true;
		}
		return false;
	}

	void ComponentMenuSlider::UpdateSlider()
	{
		float fScaleFactor = m_iCurrent / 2.0f;
		float fScaleOffset = 50.0f - fScaleFactor;

		m_mSlider.SetTransform(GetGameObject()->GetTransform().GetTransform()  *
			glm::translate(glm::vec3(m_fSliderOffset - fScaleOffset, -4.0f, -0.1f)) *
			glm::scale(glm::vec3(fScaleFactor, 10.0f, 1.0f)));
	}

	void ComponentMenuSlider::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		std::string& p_strLabelMessage,
		std::string& p_strOutlineMTLFileName,
		std::string& p_strOutlineMaterialName,
		std::string& p_strSliderMTLFileName,
		std::string& p_strSliderMaterialName,
		float& p_fSliderOffset,
		int& p_iChangeFactor,
		int& p_iCurrent)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_MenuSlider") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FontFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
			}
			else if (strcmp(szNodeName, "Label") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "message", p_strLabelMessage);
			}
			else if (strcmp(szNodeName, "OutlineMTLFileName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strOutlineMTLFileName);
			}
			else if (strcmp(szNodeName, "OutlineMaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strOutlineMaterialName);
			}
			else if (strcmp(szNodeName, "SliderMTLFileName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strSliderMTLFileName);
			}
			else if (strcmp(szNodeName, "SliderMaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strSliderMaterialName);
			}
			else if (strcmp(szNodeName, "SliderOffset") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fSliderOffset);
			}
			else if (strcmp(szNodeName, "ChangeFactor") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iChangeFactor);
			}
			else if (strcmp(szNodeName, "StartingValue") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iCurrent);
			}
		}
	}
}