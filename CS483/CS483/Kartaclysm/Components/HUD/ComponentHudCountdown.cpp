//----------------------------------------------------------------------------
// ComponentHudCountdown.cpp
// Author: Bradley Cooper
//
// Component that displays beginning race countdown.
//----------------------------------------------------------------------------

#include "ComponentHudCountdown.h"

namespace Kartaclysm
{
	ComponentHudCountdown::ComponentHudCountdown(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath
		) :
		ComponentRenderable(p_pGameObject),
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_TextBox(m_pFont, ""),
		m_OriginalTransform(),
		m_fDisplayRaceStartTimer(0.0f)
	{
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_TextBox);

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudCountdown::CountdownCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Countdown_HUD", m_pDelegate);
	}

	ComponentHudCountdown::~ComponentHudCountdown()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_TextBox);

		HeatStroke::EventManager::Instance()->RemoveListener("Countdown_HUD", m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	void ComponentHudCountdown::Init()
	{
		m_OriginalTransform = GetGameObject()->GetTransform().GetTransform();
	}

	void ComponentHudCountdown::Update(const float p_fDelta)
	{
		if (m_fDisplayRaceStartTimer > 0.0f)
		{
			m_fDisplayRaceStartTimer -= p_fDelta;

			if (m_fDisplayRaceStartTimer <= 0.0f)
			{
				GetGameObject()->GetManager()->DestroyGameObject(GetGameObject());
			}
		}
	}

	HeatStroke::Component* ComponentHudCountdown::CreateComponent(
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
		return new ComponentHudCountdown(
			p_pGameObject,
			strFontFilePath
			);
	}

	void ComponentHudCountdown::SyncTransform()
	{
		m_TextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
	}

	void ComponentHudCountdown::CountdownCallback(const HeatStroke::Event* p_pEvent)
	{
		float fCountdown;
		p_pEvent->GetRequiredFloatParameter("Countdown", fCountdown);

		if (fCountdown > 0.0f)
		{
			std::string strBefore = m_TextBox.GetText();
			SetCountdownMessage(fCountdown);
			std::string strAfter = m_TextBox.GetText();

			if (strAfter != strBefore)
			{
				HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/countdown_wait.flac");
			}
		}
		else
		{
			DisplayRaceStartMessage();
			HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/countdown_go.flac");
		}
	}

	void ComponentHudCountdown::SetCountdownMessage(const float p_fCountdown)
	{
		int iCountdown = static_cast<int>(ceilf(p_fCountdown));
		float fScale = ((1.0f - fabsf(iCountdown - p_fCountdown)) * 0.5f) + 0.5f;

		m_TextBox.SetText(std::to_string(iCountdown));
		GetGameObject()->GetTransform().SetTransform(m_OriginalTransform * glm::scale(glm::vec3(fScale, fScale, 1.0f)));

		switch (iCountdown)
		{
		case 3:	 m_TextBox.SetColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); break;		// red
		case 2:	 m_TextBox.SetColour(glm::vec4(0.5f, 0.25f, 0.0f, 1.0f)); break;	// orange
		case 1:	 m_TextBox.SetColour(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); break;		// yellow
		default: 
#ifdef _DEBUG
			printf("Countdown: %i\n", iCountdown);
			assert(false && "Should not have a countdown value outside the range of [1,3]");
#endif
			break;
		}
	}

	void ComponentHudCountdown::DisplayRaceStartMessage()
	{
		m_fDisplayRaceStartTimer = 1.0f;
		m_TextBox.SetText("GO!");
		GetGameObject()->GetTransform().SetTransform(m_OriginalTransform * glm::translate(glm::vec3(-8.0f, 0.0f, 0.0f)));
		m_TextBox.SetColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));	// green
	}

	void ComponentHudCountdown::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_Countdown") == 0);

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