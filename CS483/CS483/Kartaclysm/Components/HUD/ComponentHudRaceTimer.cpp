//----------------------------------------------------------------------------
// ComponentHudRaceTimer.cpp
// Author: Bradley Cooper
//
// Component that displays race time on the HUD.
//----------------------------------------------------------------------------

#include "ComponentHudRaceTimer.h"

namespace Kartaclysm
{
	ComponentHudRaceTimer::ComponentHudRaceTimer(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath,
		float p_fWidth,
		float p_fHeight,
		float p_fLabelOffset
		) :
		ComponentRenderable(p_pGameObject),
		m_mFont(p_strFontFilePath),
		m_mLabelTextBox(&m_mFont, "TIME", p_fWidth, p_fHeight),
		m_mTimerTextBox(&m_mFont, "00:00.000", p_fWidth, p_fHeight),
		m_fLabelOffset(p_fLabelOffset),
		m_fTime(0.0f)
	{
		m_mLabelTextBox.SetColour(glm::vec4(1.0, 0.5, 0.0, 1.0)); // orange
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mTimerTextBox);
	}

	ComponentHudRaceTimer::~ComponentHudRaceTimer()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mLabelTextBox);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTimerTextBox);
	}

	HeatStroke::Component* ComponentHudRaceTimer::CreateComponent(
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
		return new ComponentHudRaceTimer(
			p_pGameObject,
			strFontFilePath,
			fWidth,
			fHeight,
			fLabelOffset
			);
	}


	void ComponentHudRaceTimer::Update(const float p_fDelta)
	{
		// increase time up to the maximum one hour
		m_fTime += p_fDelta;
		if (m_fTime >= 3599.999f)
		{
			m_fTime = 3599.999f;
		}

		// minutes
		unsigned int uiMinutes = static_cast<unsigned int>(m_fTime) / 60;
		std::string strMinutes = (uiMinutes < 10 ? "0" + std::to_string(uiMinutes) : std::to_string(uiMinutes));

		// seconds
		unsigned int uiSeconds = static_cast<unsigned int>(m_fTime) % 60;
		std::string strSeconds = (uiSeconds < 10 ? "0" + std::to_string(uiSeconds) : std::to_string(uiSeconds));

		m_mTimerTextBox.SetText(strMinutes + ":" + strSeconds);

		/*
		// milliseconds (decreases frame rate with many render calls)
		unsigned int uiMilli = static_cast<unsigned int>((m_fTime - static_cast<unsigned int>(m_fTime)) * 1000);
		std::string strMilli =	(uiMilli < 10 ? "00" + std::to_string(uiMilli) : 
								(uiMilli < 100 ? "0" + std::to_string(uiMilli) :
								std::to_string(uiMilli)));

		m_mTimerTextBox.SetText(strMinutes + ":" + strSeconds + "." + strMilli);
		*/
	}

	void ComponentHudRaceTimer::SyncTransform()
	{
		m_mLabelTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
			glm::translate(glm::vec3(m_fLabelOffset, 0.0f, 0.0f)));
		m_mTimerTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform()); 
	}

	void ComponentHudRaceTimer::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		float& p_fWidth,
		float& p_fHeight,
		float& p_fLabelOffset)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_RaceTimer") == 0);

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