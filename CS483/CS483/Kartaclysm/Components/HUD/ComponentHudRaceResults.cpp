#include "ComponentHudRaceResults.h"

Kartaclysm::ComponentHudRaceResults::ComponentHudRaceResults(HeatStroke::GameObject* p_pGameObject, const std::string& p_strFontFilePath)
	:
	ComponentRenderable(p_pGameObject),
	m_pFont(HeatStroke::FontManager::Instance()->GetOrCreateFont(p_strFontFilePath)),
	m_TextBox(m_pFont, "")
{
	std::string strParentGuid = p_pGameObject->GetParent()->GetGUID();
	int iRacerGuidEndIndex = strParentGuid.find("_HUD");
	m_strRacerId = strParentGuid.substr(0, iRacerGuidEndIndex);

	m_pRaceFinishedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHudRaceResults::RaceFinishedCallback, this, std::placeholders::_1));
	HeatStroke::EventManager::Instance()->AddListener("RacerFinishedRace", m_pRaceFinishedDelegate);
}

Kartaclysm::ComponentHudRaceResults::~ComponentHudRaceResults()
{
	HeatStroke::SceneManager::Instance()->RemoveTextBox(&m_TextBox);

	HeatStroke::EventManager::Instance()->RemoveListener("RacerFinishedRace", m_pRaceFinishedDelegate);
	delete m_pRaceFinishedDelegate;
	m_pRaceFinishedDelegate = nullptr;
}

HeatStroke::Component* Kartaclysm::ComponentHudRaceResults::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	assert(p_pGameObject != nullptr);

	std::string strFontFilePath = "";
	tinyxml2::XMLElement* pFontFileElement = p_pBaseNode->FirstChildElement("FontFile");
	HeatStroke::EasyXML::GetRequiredStringAttribute(pFontFileElement, "path", strFontFilePath);

	return new ComponentHudRaceResults(
		p_pGameObject,
		strFontFilePath
	);
}

void Kartaclysm::ComponentHudRaceResults::Update(const float p_fDelta)
{
}

void Kartaclysm::ComponentHudRaceResults::SyncTransform()
{
	m_TextBox.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
}

void Kartaclysm::ComponentHudRaceResults::RaceFinishedCallback(const HeatStroke::Event * p_pEvent)
{
	std::string strRacerId = "";
	p_pEvent->GetRequiredStringParameter("racerId", strRacerId);

	float fRacerTime = -1.0f;
	p_pEvent->GetRequiredFloatParameter("racerTime", fRacerTime);
	std::string strRacerTime = Common::TimeStringFromFloat(fRacerTime);

	m_TextBox.SetText(m_TextBox.GetText() + "\n" + strRacerId + "   " + strRacerTime);

	if (std::strcmp(strRacerId.c_str(), m_strRacerId.c_str()) == 0)
	{
		HeatStroke::SceneManager::Instance()->AddTextBox(&m_TextBox);
	}
}