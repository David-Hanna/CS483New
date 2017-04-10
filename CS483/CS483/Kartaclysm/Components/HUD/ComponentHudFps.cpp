//----------------------------------------------------------------------------
// ComponentHudFps.cpp
// Author: Bradley Cooper
//
// Component that displays Frames per Second (FPS) on the HUD.
//----------------------------------------------------------------------------

#include "ComponentHudFps.h"

namespace Kartaclysm
{
	ComponentHudFps::ComponentHudFps(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath
		) :
		ComponentRenderable(p_pGameObject),
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_mTextBox(m_pFont, "0"),
		m_fFPS(0.0f),
		m_iFrameCounter(0),
		m_fTimeSinceLastShown(0.0f),
		m_lFrameSpeeds(std::vector<float>(90))
	{
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_mTextBox);
	}

	ComponentHudFps::~ComponentHudFps()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_mTextBox);
	}

	HeatStroke::Component* ComponentHudFps::CreateComponent(
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
		return new ComponentHudFps(
			p_pGameObject,
			strFontFilePath
			);
	}

	void ComponentHudFps::Update(const float p_fDelta)
	{
		// calculate FPS using a simple moving average (SMA) algorithm
		m_fFPS -= m_lFrameSpeeds[m_iFrameCounter];
		m_fFPS += 1.0f / p_fDelta;
		m_lFrameSpeeds[m_iFrameCounter] = 1.0f / p_fDelta;
		if (++m_iFrameCounter == m_lFrameSpeeds.size())
		{
			m_iFrameCounter = 0;
		}

		m_fTimeSinceLastShown += p_fDelta;
		if (m_fTimeSinceLastShown >= 0.2f)
		{
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << m_fFPS / m_lFrameSpeeds.size();
			m_mTextBox.SetText(stream.str());
			m_fTimeSinceLastShown = 0.0f;
		}
	}

	void ComponentHudFps::SyncTransform()
	{
		m_mTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	}

	void ComponentHudFps::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Fps") == 0);

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