//----------------------------------------------------------------------------
// ComponentHudAbility.cpp
// Author: Bradley Cooper
//
// Component that handles an ability icon on a player's HUD.
//----------------------------------------------------------------------------

#include "ComponentHudAbility.h"

namespace Kartaclysm
{
	ComponentHudAbility::ComponentHudAbility(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strMTLFileName,
		const std::string& p_strMaterialName
		) :
		HeatStroke::ComponentSprite(p_pGameObject, p_strMTLFileName, p_strMaterialName)
	{
		
	}

	ComponentHudAbility::~ComponentHudAbility()
	{
	}

	HeatStroke::Component* ComponentHudAbility::CreateComponent(
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
		return new ComponentHudAbility(
			p_pGameObject,
			strMTLFileName,
			strMaterialName
			);
	}

	void ComponentHudAbility::AbilityCallback(const HeatStroke::Event* p_pEvent)
	{

	}

	void ComponentHudAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strMTLFileName,
		std::string& p_strMaterialName)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_Hud_Ability") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			// skip comments
			if (pChildElement->ToComment() != nullptr)
				continue;

			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "MTLFileName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strMTLFileName);
			}
			else if (strcmp(szNodeName, "MaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strMaterialName);
			}
		}
	}
}