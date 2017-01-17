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
		m_mSprite(m_strPositionFilePrefix + "position_1.mtl", "position_1"),
		m_strPlayerX(GetGameObject()->GetParent()->GetGUID().substr(0, GetGameObject()->GetParent()->GetGUID().find('_'))),
		m_iCurrentPosition(0)
	{
		m_mSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
		HeatStroke::SceneManager::Instance()->AddSpriteInstance(&m_mSprite);

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudPosition::PositionCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RaceStandingsUpdate", m_pDelegate);
	}

	ComponentHudPosition::~ComponentHudPosition()
	{
		HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(&m_mSprite);

		HeatStroke::EventManager::Instance()->RemoveListener("RaceStandingsUpdate", m_pDelegate);
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
	}

	void ComponentHudPosition::SyncTransform()
	{
	};

	void ComponentHudPosition::PositionCallback(const HeatStroke::Event* p_pEvent)
	{
		int iPosition;
		p_pEvent->GetRequiredIntParameter(m_strPlayerX, iPosition);

		if (m_iCurrentPosition != ++iPosition)
		{
			m_iCurrentPosition = iPosition;
			std::string strPosition = "position_" + std::to_string(iPosition);

			HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(&m_mSprite);
			m_mSprite = HeatStroke::SpriteInstance(m_strPositionFilePrefix + strPosition + ".mtl", strPosition);
			m_mSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
			HeatStroke::SceneManager::Instance()->AddSpriteInstance(&m_mSprite);
		}
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