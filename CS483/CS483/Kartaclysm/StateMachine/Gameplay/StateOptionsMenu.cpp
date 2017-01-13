//------------------------------------------------------------------------
// StateOptionsMenu
// Author:	Bradley Cooper
//	
// Allows player to change and save game options.
// Accessed by and returns to main menu or pause menu.
// Passing OptionsXML parameter loads options from XML and calls Pop().
//------------------------------------------------------------------------

#include "StateOptionsMenu.h"

Kartaclysm::StateOptionsMenu::StateOptionsMenu()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_strXmlFilePath("")
{
}

Kartaclysm::StateOptionsMenu::~StateOptionsMenu()
{
}

void Kartaclysm::StateOptionsMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	// If OptionsXML is specified, load options from XML and pop to previous state
	auto find = p_mContextParameters.find("OptionsXML");
	if (find != p_mContextParameters.end())
	{
		LoadOptionsFromXml(find->second);
		m_pStateMachine->Pop();
		return;
	}

	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/menu_camera.xml");

	printf("Entering Options Menu State.\n");
}

void Kartaclysm::StateOptionsMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		// TODO: Stubbed to not have any functionaliy besides initial XML loading
		m_pStateMachine->Pop();
		return;

		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

	}
}

void Kartaclysm::StateOptionsMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateOptionsMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	printf("Exiting Player Selection Menu state.\n");
}

void Kartaclysm::StateOptionsMenu::LoadOptionsFromXml(const std::string& p_strXmlFilePath)
{
	m_strXmlFilePath = p_strXmlFilePath;

	tinyxml2::XMLDocument mPreloadDoc;
	if (mPreloadDoc.LoadFile(p_strXmlFilePath.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		printf("StateOptionsMenu: No options XML file found. Default values assumed");
		return;
	}

	tinyxml2::XMLElement* pOptionsElement = mPreloadDoc.RootElement();
	assert(pOptionsElement != nullptr);

	// Audio volume levels
	tinyxml2::XMLElement* pAudioElement = pOptionsElement->FirstChildElement("Audio");
	if (pAudioElement != nullptr)
	{
		HeatStroke::AudioPlayer* pAudioPlayer = HeatStroke::AudioPlayer::Instance();
		float fMusicVolume = pAudioPlayer->GetMusicVolume();
		float fSFXVolume = pAudioPlayer->GetSoundEffectsVolume();

		HeatStroke::EasyXML::GetOptionalFloatAttribute(pAudioElement->FirstChildElement("Music"), "volume", fMusicVolume, fMusicVolume);
		HeatStroke::EasyXML::GetOptionalFloatAttribute(pAudioElement->FirstChildElement("SFX"), "volume", fSFXVolume, fSFXVolume);

		pAudioPlayer->SetMusicVolume(fMusicVolume);
		pAudioPlayer->SetSoundEffectsVolume(fSFXVolume);
	}
}