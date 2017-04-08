//----------------------------------------------------------------------------
// ComponentHudRaceTimer.cpp
// Author: Bradley Cooper
//
// Component that displays race time on the HUD.
//----------------------------------------------------------------------------

#include "ComponentHudRaceTimer.h"

namespace Kartaclysm
{
	ComponentHudRaceTimer::ComponentHudRaceTimer(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath,
		float p_fLabelOffset
		) :
		ComponentRenderable(p_pGameObject),
		m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
		m_LabelTextBox(m_pFont, "TIME"),
		m_TimerTextBox(m_pFont, "00:00"),
		m_fTime(-3.0f), // beginning countdown
		m_fLapDisplayTimer(0.0f)
	{
		m_LabelTextBox.SetColour(glm::vec4(1.0, 0.5, 0.0, 1.0)); // orange

		m_LabelTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
			glm::translate(glm::vec3(p_fLabelOffset, 0.0f, 0.0f)));

		HeatStroke::SceneManager::Instance()->AddTextBox(&m_LabelTextBox);
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_TimerTextBox);
	}

	ComponentHudRaceTimer::~ComponentHudRaceTimer()
	{
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_LabelTextBox);
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_TimerTextBox);

		HeatStroke::EventManager::Instance()->RemoveListener(m_strEventName, m_pDelegate);
		delete m_pDelegate;
		m_pDelegate = nullptr;
	}

	HeatStroke::Component* ComponentHudRaceTimer::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		// Make some assertions on the parameters.
		assert(p_pGameObject != nullptr);

		// The values we need to fill by the end of parsing.
		std::string strFontFilePath("");
		float fLabelOffset(999.999f);

		// Parse the elements of the base node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fLabelOffset);
		}
		// Then override with the Override node.
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pBaseNode, strFontFilePath, fLabelOffset);
		}

		// Check that we got everything we needed.
		assert(strFontFilePath != "");
		assert(fLabelOffset != 999.999f);

		// Now we can create and return the Component.
		return new ComponentHudRaceTimer(
			p_pGameObject,
			strFontFilePath,
			fLabelOffset
			);
	}

	void ComponentHudRaceTimer::Update(const float p_fDelta)
	{
		m_fTime += p_fDelta;
		if (m_fTime < 0.0f) return;

		if (m_fLapDisplayTimer > 0.0f)
		{
			m_fLapDisplayTimer -= p_fDelta;
			if (m_fLapDisplayTimer <= 0.0f)
			{
				DisableLapFlash();
			}
			else
			{
				FlashLapTimer(m_fLapDisplayTimer, 0.5f);
			}
		}
		else
		{
			m_TimerTextBox.SetText(Common::FormatHudTime(m_fTime));
		}
	}

	void ComponentHudRaceTimer::Init()
	{
		// event name follows "Player0_HUD_Lap" format
		assert(GetGameObject()->GetParent() != nullptr && "HUD hierarchy error");
		m_strEventName = GetGameObject()->GetParent()->GetGUID() + "_Lap";

		m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudRaceTimer::LapCountCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strEventName, m_pDelegate);
	}

	void ComponentHudRaceTimer::SyncTransform()
	{
		if (m_fLapDisplayTimer > 0.0f)
		{
			m_TimerTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform() *
				glm::translate(glm::vec3(-24.0f, 0.0f, 0.0f)));
		}
		else
		{
			m_TimerTextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
		}
	}

	void ComponentHudRaceTimer::LapCountCallback(const HeatStroke::Event* p_pEvent)
	{
		float fLapTime;
		p_pEvent->GetRequiredFloatParameter("LapTime", fLapTime);
		if (fLapTime <= 0.0f) return;

		m_fLapDisplayTimer = 3.0f;
		HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_LabelTextBox);
		m_TimerTextBox.SetText(Common::TimeStringFromFloat(fLapTime));
	}

	void ComponentHudRaceTimer::FlashLapTimer(const float p_fLapDisplayTimer, const float p_fInterval)
	{
		assert(p_fInterval > 0.0f && "Must be a positive interval");
		int iDivisor = static_cast<int>(p_fLapDisplayTimer / p_fInterval);

		if (iDivisor % 2 == 0)
		{
			m_TimerTextBox.SetColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));	// white
		}
		else
		{
			m_TimerTextBox.SetColour(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));	// light blue
		}
	}

	void ComponentHudRaceTimer::DisableLapFlash()
	{
		m_fLapDisplayTimer = 0.0f;
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_LabelTextBox);

		m_TimerTextBox.SetText(Common::FormatHudTime(m_fTime));
		m_TimerTextBox.SetColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));	// white
	}

	void ComponentHudRaceTimer::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strFontFilePath,
		float& p_fLabelOffset)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_HUD_RaceTimer") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FontFile") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strFontFilePath);
			}
			else if (strcmp(szNodeName, "LabelOffset") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fLabelOffset);
			}
		}
	}
}