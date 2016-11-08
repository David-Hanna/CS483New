//------------------------------------------------------------------------
// ComponentSprite
// Author:	David Hanna
//	
// Provides 3D rendering of a .obj model to the screen for a GameObject.
//------------------------------------------------------------------------

#include "ComponentSprite.h"

HeatStroke::ComponentSprite::ComponentSprite(HeatStroke::GameObject* p_pGameObject, const std::string& p_strMTLFileName, const std::string& p_strMaterialName) :
ComponentRenderable(p_pGameObject),
m_mSprite(p_strMTLFileName, p_strMaterialName)
{
	SceneManager::Instance()->AddSprite(&m_mSprite);
}

HeatStroke::ComponentSprite::~ComponentSprite()
{
	SceneManager::Instance()->RemoveSprite(&m_mSprite);
}

HeatStroke::Component* HeatStroke::ComponentSprite::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	// Make some assertions on the parameters.
	assert(p_pGameObject != nullptr);

	// The values we need to fill by the end of parsing.
	std::string strMTLFileName("");
	std::string strMaterialName("");

	// Parse the elements of the base node.
	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, strMTLFileName, strMaterialName);
	}
	// Then override with the Override node.
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, strMTLFileName, strMaterialName);
	}

	// Check that we got everything we needed.
	assert(strMTLFileName != "");
	assert(strMaterialName != "");

	// Now we can create and return the Component.
	return new ComponentSprite(
		p_pGameObject,
		strMTLFileName,
		strMaterialName
		);
}

void HeatStroke::ComponentSprite::SyncTransform()
{
	m_mSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
}

void HeatStroke::ComponentSprite::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	std::string& p_strMTLFileName,
	std::string& p_strMaterialName)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_Sprite") == 0);

	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "MTLFileName") == 0)
		{
			HeatStroke::EasyXML::GetRequiredStringAttribute(pElement, "path", p_strMTLFileName);
		}
		else if (strcmp(szNodeName, "MaterialName") == 0)
		{
			HeatStroke::EasyXML::GetRequiredStringAttribute(pElement, "name", p_strMaterialName);
		}
	}
}