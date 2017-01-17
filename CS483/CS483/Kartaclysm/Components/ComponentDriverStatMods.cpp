//----------------------------------------------------------------------------
// ComponentDriverStatMods
// Author: David MacIntosh
//
// Component for the stat modifiers of a driver
//----------------------------------------------------------------------------

#include "ComponentDriverStatMods.h"

#include "ComponentKartController.h"

namespace Kartaclysm
{
	ComponentDriverStatMods::ComponentDriverStatMods(
		HeatStroke::GameObject* p_pGameObject,
		int p_iMaxSpeed,
		int p_iAcceleration,
		int p_iHandling,
		int p_iDurability)
		:
		Component(p_pGameObject),
		m_iMaxSpeedStat(p_iMaxSpeed),
		m_iAccelerationStat(p_iAcceleration),
		m_iHandlingStat(p_iHandling),
		m_iDurabilityStat(p_iDurability)
	{
		ComponentKartController* kartController = (ComponentKartController*)m_pGameObject->GetParent()->GetComponent("GOC_KartController");

		kartController->SetDriverStatMods(m_iMaxSpeedStat, m_iAccelerationStat, m_iHandlingStat, m_iDurabilityStat);
	}

	ComponentDriverStatMods::~ComponentDriverStatMods()
	{
	}

	HeatStroke::Component* ComponentDriverStatMods::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Defaults
		int iMaxSpeed = 0;
		int iAcceleration = 0;
		int iHandling = 0;
		int iDurability = 0;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, iMaxSpeed, iAcceleration, iHandling, iDurability);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, iMaxSpeed, iAcceleration, iHandling, iDurability);
		}

		return new ComponentDriverStatMods(p_pGameObject, iMaxSpeed, iAcceleration, iHandling, iDurability);
	}

	void ComponentDriverStatMods::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		int& p_iMaxSpeed,
		int& p_iAcceleration,
		int& p_iHandling,
		int& p_iDurability)
	{
		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "MaxSpeed") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iMaxSpeed);
			}
			else if (strcmp(szNodeName, "Acceleration") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iAcceleration);
			}
			else if (strcmp(szNodeName, "Handling") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iHandling);
			}
			else if (strcmp(szNodeName, "Durability") == 0)
			{
				HeatStroke::EasyXML::GetRequiredIntAttribute(pChildElement, "value", p_iDurability);
			}
		}
	}
}