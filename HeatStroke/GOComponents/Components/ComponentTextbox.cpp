//------------------------------------------------------------------------
// ComponentTextBox
// Author:	Bradley Cooper
//	
// Provides 2D rendering of a textbox.
//------------------------------------------------------------------------

#include "ComponentTextBox.h"

HeatStroke::ComponentTextBox::ComponentTextBox(GameObject* p_pGameObject,
	const std::string& p_strFontFilePath,
	const std::string& p_strMessage,
	float p_fWidth,
	float p_fHeight)
	:
	ComponentRenderable(p_pGameObject),
	m_mFont(p_strFontFilePath),
	m_mTextBox(&m_mFont, p_strMessage, p_fWidth, p_fHeight)
{
	SceneManager::Instance()->AddTextBox(&m_mTextBox);
}

HeatStroke::ComponentTextBox::~ComponentTextBox()
{
	SceneManager::Instance()->RemoveTextBox(&m_mTextBox);
}

HeatStroke::Component* HeatStroke::ComponentTextBox::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	// Make some assertions on the parameters.
	assert(p_pGameObject != nullptr);

	// The values we need to fill by the end of parsing.
	std::string strFontFilePath("");
	std::string strMessage("");
	float fWidth(0.0f);
	float fHeight(0.0f);

	// Parse the elements of the base node.
	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, strFontFilePath, strMessage, fWidth, fHeight);
	}
	// Then override with the Override node.
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pBaseNode, strFontFilePath, strMessage, fWidth, fHeight);
	}

	// Check that we got everything we needed.
	assert(strFontFilePath != "");
	assert(strMessage != "");
	assert(fWidth != 0.0f);
	assert(fHeight != 0.0f);

	// Now we can create and return the Component.
	return new ComponentTextBox(
		p_pGameObject,
		strFontFilePath,
		strMessage,
		fWidth,
		fHeight
		);
}

void HeatStroke::ComponentTextBox::SyncTransform()
{
	m_mTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
}

void HeatStroke::ComponentTextBox::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	std::string& p_strFontFilePath,
	std::string& p_strMessage,
	float& p_fWidth,
	float& p_fHeight)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_TextBox") == 0);

	for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
		pChildElement != nullptr;
		pChildElement = pChildElement->NextSiblingElement())
	{
		const char* szNodeName = pChildElement->Value();

		if (strcmp(szNodeName, "FontFile") == 0)
		{
			HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
		}
		else if (strcmp(szNodeName, "Message") == 0)
		{
			HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "value", p_strMessage);
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