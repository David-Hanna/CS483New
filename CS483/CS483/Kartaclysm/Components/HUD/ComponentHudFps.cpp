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
		const std::string& p_strFontFilePath,
		float p_fWidth,
		float p_fHeight
		) :
		ComponentRenderable(p_pGameObject),
		m_mFont(p_strFontFilePath),
		m_mTextBox(&m_mFont, "0", p_fWidth, p_fHeight),
		m_fFPS(0.0f)
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
		float fWidth(0.0f);
		float fHeight(0.0f);

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fWidth, fHeight);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fWidth, fHeight);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(fWidth != 0.0f);
		assert(fHeight != 0.0f);

		// Now we can create and return the Component.
		return new ComponentHudFps(
			p_pGameObject,
			strFontFilePath,
			fWidth,
			fHeight
			);
	}


	void ComponentHudFps::Update(const float p_fDelta)
	{
		// smooth out fps over number of frames
		m_fFPS = (m_fFPS * 0.8f) + (0.2f / p_fDelta);
		m_mTextBox.SetText(std::to_string(static_cast<int>(m_fFPS)));
	}

	void ComponentHudFps::SyncTransform()
	{
		m_mTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	}

	void ComponentHudFps::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		float& p_fWidth,
		float& p_fHeight)
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
			else if (strcmp(szNodeName, "Width") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fWidth);
			}
			else if (strcmp(szNodeName, "Height") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fHeight);
			}
		}
	}
}