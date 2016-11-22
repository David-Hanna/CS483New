//----------------------------------------------------------------------------
// ComponentHudPosition.cpp
// Author: Bradley Cooper
//
// Component that handles an ability icon on a player's HUD.
//----------------------------------------------------------------------------

#include "ComponentHudPosition.h"

namespace Kartaclysm
{
	ComponentHudPosition::ComponentHudPosition(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strPositionFilePrefix
		) :
		HeatStroke::ComponentRenderable(p_pGameObject),
		m_strPositionFilePrefix(p_strPositionFilePrefix),
		m_mSprite(m_strPositionFilePrefix + "position_1.mtl", "position_1")
	{
		HeatStroke::SceneManager::Instance()->AddSprite(&m_mSprite);
	}

	ComponentHudPosition::~ComponentHudPosition()
	{
		HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mSprite);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudPosition::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strPositionFilePrefix("");

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strPositionFilePrefix);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strPositionFilePrefix);
		}

		// Check that we got everything we needed.
		assert(strPositionFilePrefix != "");

		// Now we can create and return the Component.
		return new ComponentHudPosition(
			p_pGameObject,
			strPositionFilePrefix
			);
	}

	void ComponentHudPosition::Init()
	{
		// event name follows "Player0_HUD_Lap" format
		assert(GetGameObject()->GetParent() != nullptr && "HUD hierarchy error");
		m_strEventName = GetGameObject()->GetParent()->GetGUID() + "_Position";

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudPosition::PositionCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	void ComponentHudPosition::SyncTransform()
	{
		m_mSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	};

	void ComponentHudPosition::PositionCallback(const HeatStroke::Event* p_pEvent)
	{
		int iPosition;
		p_pEvent->GetRequiredIntParameter("Position", iPosition);
		std::string strPosition = "position_" + std::to_string(iPosition);

		// TO DO, not very efficient I imagine. Should have them memo-ized or stored for quick switching
		HeatStroke::SceneManager::Instance()->RemoveSprite(&m_mSprite);
		m_mSprite = HeatStroke::Sprite(m_strPositionFilePrefix + strPosition + ".mtl", strPosition);
		HeatStroke::SceneManager::Instance()->AddSprite(&m_mSprite);
	}

	void ComponentHudPosition::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& strPositionFilePrefix)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Position") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			// skip comments
			if (pChildElement->ToComment() != nullptr)
				continue;

			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "PositionFilePrefix") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", strPositionFilePrefix);
			}
		}
	}
}