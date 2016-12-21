//----------------------------------------------------------------------------
// ComponentHudPopup.cpp
// Author: Bradley Cooper
//
// Component that displays Frames per Second (FPS) on the HUD.
//----------------------------------------------------------------------------

#include "ComponentHudPopup.h"

namespace Kartaclysm
{
	ComponentHudPopup::ComponentHudPopup(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strMTLFileName,
		const std::string& p_strMaterialName,
		const std::string& p_strEventName,
		float p_fDelay
		) :
		ComponentRenderable(p_pGameObject),
		m_mSprite(p_strMTLFileName, p_strMaterialName),
		m_strEventName(p_strEventName),
		m_bDisplaying(false),
		m_fDelay(p_fDelay),
		m_fWaitingToRender(0.0f)
	{	
	}

	ComponentHudPopup::~ComponentHudPopup()
	{
		HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(&m_mSprite);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudPopup::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strMTLFileName("");
		std::string strMaterialName("");
		std::string strEventName("");
		float fDelay = 0.0f;

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strMTLFileName, strMaterialName, strEventName, fDelay);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strMTLFileName, strMaterialName, strEventName, fDelay);
		}

		// Check that we got everything we needed.
		assert(strMTLFileName != "");
		assert(strMaterialName != "");
		assert(strEventName != "");

		// Now we can create and return the Component.
		return new ComponentHudPopup(
			p_pGameObject,
			strMTLFileName,
			strMaterialName,
			strEventName,
			fDelay
			);
	}

	void ComponentHudPopup::Init()
	{
		// Replace generic "PlayerX" found in XML to the proper player number ("Player0" as example)
		std::size_t find = m_strEventName.find("PlayerX");
		if (find != std::string::npos)
		{
			m_strEventName.replace(find, find + 8, GetGameObject()->GetParent()->GetGUID().substr(0, 8));
		}

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudPopup::ToggleCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	void ComponentHudPopup::Update(const float p_fDelta)
	{
		// Update time to render until it surpasses the delay (if delay == 0.0f, handled in ToggleCallback instead)
		if (m_bDisplaying && m_fWaitingToRender < m_fDelay)
		{
			m_fWaitingToRender += p_fDelta;

			if (m_fWaitingToRender >= m_fDelay)
			{
				HeatStroke::SceneManager::Instance()->AddSpriteInstance(&m_mSprite);
			}
		}
	}

	void ComponentHudPopup::SyncTransform()
	{
		m_mSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	}

	void ComponentHudPopup::ToggleCallback(const HeatStroke::Event* p_pEvent)
	{
		int iDisplay;
		p_pEvent->GetRequiredIntParameter("Display", iDisplay);
		
		if (iDisplay == 0)
		{
			m_bDisplaying = false;
			m_fWaitingToRender = 0.0f;
			HeatStroke::SceneManager::Instance()->RemoveSpriteInstance(&m_mSprite);
		}
		else
		{
			m_bDisplaying = true;

			if (m_fDelay == 0.0f)
			{
				HeatStroke::SceneManager::Instance()->AddSpriteInstance(&m_mSprite);
			}
		}
	}

	void ComponentHudPopup::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strMTLFileName,
		std::string& p_strMaterialName,
		std::string& p_strEventName,
		float& p_fDelay)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Popup") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "MTLFileName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strMTLFileName);
			}
			else if (strcmp(szNodeName, "MaterialName") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "name", p_strMaterialName);
			}
			else if (strcmp(szNodeName, "Event") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "value", p_strEventName);
			}
			else if (strcmp(szNodeName, "Delay") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fDelay);
			}
		}
	}
}