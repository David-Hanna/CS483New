//----------------------------------------------------------------------------
// ComponentAbilityIcon.h
// Author: Bradley Cooper
//
// Component that handles the conditions for activating an ability.
//----------------------------------------------------------------------------

#include "ComponentAbilityIcon.h"

namespace Kartaclysm
{
	ComponentAbilityIcon::ComponentAbilityIcon(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strAbility,
		const std::string& p_strActiveMTLFileName,
		const std::string& p_strActiveMaterialName,
		const std::string& p_strInactiveMTLFileName,
		const std::string& p_strInactiveMaterialName)
		:
		Component(p_pGameObject),
		m_strAbility(p_strAbility),
		m_strActiveMaterialName(p_strActiveMaterialName),
		m_strActiveMTLFileName(p_strActiveMTLFileName),
		m_strInactiveMaterialName(p_strInactiveMaterialName),
		m_strInactiveMTLFileName(p_strInactiveMTLFileName)
	{
	}

	ComponentAbilityIcon::~ComponentAbilityIcon()
	{
	}

	HeatStroke::Component* ComponentAbilityIcon::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults (some for testing purposes only)
		std::string strAbility("");
		std::string strActiveMTLFileName("");
		std::string strActiveMaterialName("");
		std::string strInactiveMTLFileName("");
		std::string strInactiveMaterialName("");

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strAbility, strActiveMTLFileName, strActiveMaterialName, strInactiveMTLFileName, strInactiveMaterialName);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strAbility, strActiveMTLFileName, strActiveMaterialName, strInactiveMTLFileName, strInactiveMaterialName);
		}

		// Check that we got everything we needed.
		assert(strAbility != "");
		assert(strActiveMTLFileName != "");
		assert(strActiveMaterialName != "");
		assert(strInactiveMTLFileName != "");
		assert(strInactiveMaterialName != "");

		return new ComponentAbilityIcon(
			p_pGameObject,
			strAbility,
			strActiveMTLFileName,
			strActiveMaterialName,
			strInactiveMTLFileName,
			strInactiveMaterialName
			);
	}

	void ComponentAbilityIcon::Init()
	{
		//GUID follows format of "Player0": needs to be "Player0_HUD_KartAbility1_Icon"
		std::string strEventName = GetGameObject()->GetParent()->GetParent()->GetGUID();
		strEventName = strEventName + "_HUD_" + m_strAbility + "_Icon";

		HeatStroke::Event* pEvent = new HeatStroke::Event(strEventName);
		pEvent->SetStringParameter("ActiveMTLFile", m_strActiveMaterialName);
		pEvent->SetStringParameter("ActiveMaterialName", m_strActiveMTLFileName);
		pEvent->SetStringParameter("InactiveMTLFile", m_strInactiveMaterialName);
		pEvent->SetStringParameter("InactiveMaterialName", m_strInactiveMTLFileName);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentAbilityIcon::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strAbility,
		std::string& p_strActiveMTLFileName,
		std::string& p_strActiveMaterialName,
		std::string& p_strInactiveMTLFileName,
		std::string& p_strInactiveMaterialName)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_AbilityIcon") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Ability") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "value", p_strAbility);
			}
			else if (strcmp(szNodeName, "ActiveMTLFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strActiveMTLFileName);
			}
			else if (strcmp(szNodeName, "ActiveMaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strActiveMaterialName);
			}
			else if (strcmp(szNodeName, "InactiveMTLFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strInactiveMTLFileName);
			}
			else if (strcmp(szNodeName, "InactiveMaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strInactiveMaterialName);
			}
		}
	}
}
