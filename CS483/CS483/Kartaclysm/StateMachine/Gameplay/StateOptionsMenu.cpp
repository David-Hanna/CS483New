//------------------------------------------------------------------------
// StateOptionsMenu
// Author:	Bradley Cooper
//	
// Allows player to change and save game options.
// Accessed by and returns to main menu or pause menu.
// Constructed with path to options XML file.
//------------------------------------------------------------------------

#include "StateOptionsMenu.h"

Kartaclysm::StateOptionsMenu::StateOptionsMenu(const std::string& p_strXmlFilePath)
	:
	GameplayState("Options State"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_strXmlFilePath(p_strXmlFilePath),
	m_iPlayer(0),
	m_pCurrentHighlight(nullptr),
	m_pCurrentSlider(nullptr)
{
	LoadOptionsFromXml(m_strXmlFilePath);
}

Kartaclysm::StateOptionsMenu::~StateOptionsMenu()
{
}

void Kartaclysm::StateOptionsMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_iOptionSelection = 0;

	// Get player controlling menu (Player 0 by default, but might change if accessed from pause menu)
	auto find = p_mContextParameters.find("Player");
	m_iPlayer = (find == p_mContextParameters.end() ? 0 : atoi(find->second.c_str()));
	
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_MenuSlider", ComponentMenuSlider::CreateComponent);

	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_overlay.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/OptionsMenu/options_selection.xml");
	m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/OptionsMenu/options_highlight_music.xml");

	// Set slider initial values
	if (ComponentMenuSlider* pSlider = dynamic_cast<ComponentMenuSlider*>(m_pGameObjectManager->GetGameObject("music_slider")->GetComponent("GOC_Renderable")))
	{
		pSlider->SetSliderValue(static_cast<int>(HeatStroke::AudioPlayer::Instance()->GetMusicVolume()));
	}
	else
	{
		assert(false && "Failed to find music slider");
	}

	if (ComponentMenuSlider* pSlider = dynamic_cast<ComponentMenuSlider*>(m_pGameObjectManager->GetGameObject("sfx_slider")->GetComponent("GOC_Renderable")))
	{
		pSlider->SetSliderValue(static_cast<int>(HeatStroke::AudioPlayer::Instance()->GetSoundEffectsVolume()));
	}
	else
	{
		assert(false && "Failed to find sound effects slider");
	}

	// TODO: What music plays here? Or does it continue the previous music?
}

void Kartaclysm::StateOptionsMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(m_iPlayer, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (bConfirm)
		{
			if (m_iOptionSelection == 2)
			{
				m_pStateMachine->Pop();
				// TODO: Save changes to file, and maybe add that file to the .gitignore?
			}
		}
		else if (bCancel)
		{
			m_pStateMachine->Pop(); // TODO: Cancelling out of the menu reverts changes?
		}
		else if (bUp)
		{
			switch (m_iOptionSelection)
			{
			case 1:
				m_iOptionSelection = 0;
				m_pCurrentSlider = nullptr;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/OptionsMenu/options_highlight_music.xml");
				break;
			case 2:
				m_iOptionSelection = 1;
				m_pCurrentSlider = nullptr;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/OptionsMenu/options_highlight_sfx.xml");
				break;
			}
		}
		else if (bDown)
		{
			switch (m_iOptionSelection)
			{
			case 0:
				m_iOptionSelection = 1;
				m_pCurrentSlider = nullptr;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/OptionsMenu/options_highlight_sfx.xml");
				break;
			case 1:
				m_iOptionSelection = 2;
				m_pCurrentSlider = nullptr;
				m_pGameObjectManager->DestroyGameObject(m_pCurrentHighlight);
				m_pCurrentHighlight = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/OptionsMenu/options_highlight_return.xml");
				break;
			}
		}
		else if (bRight || bLeft)
		{
			if (m_iOptionSelection == 2) return;

			if (m_pCurrentSlider == nullptr)
			{
				switch (m_iOptionSelection)
				{
				case 0:
					m_pCurrentSlider = dynamic_cast<ComponentMenuSlider*>(m_pGameObjectManager->GetGameObject("music_slider")->GetComponent("GOC_Renderable"));
					break;
				case 1:
					m_pCurrentSlider = dynamic_cast<ComponentMenuSlider*>(m_pGameObjectManager->GetGameObject("sfx_slider")->GetComponent("GOC_Renderable"));
					break;
				}
			}
			assert(m_pCurrentSlider != nullptr);

			bool bUpdate = false;
			if (bRight)
			{
				bUpdate = m_pCurrentSlider->IncreaseSlider();
			}
			else
			{
				bUpdate = m_pCurrentSlider->DecreaseSlider();
			}

			if (bUpdate)
			{
				switch (m_iOptionSelection)
				{
				case 0:
					HeatStroke::AudioPlayer::Instance()->SetMusicVolume(static_cast<float>(m_pCurrentSlider->GetSliderValue()));
					break;
				case 1:
					HeatStroke::AudioPlayer::Instance()->SetSoundEffectsVolume(static_cast<float>(m_pCurrentSlider->GetSliderValue()));
					// TODO: Play test sound effect to determine volume level
					break;
				}
			}
		}
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
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StateOptionsMenu::LoadOptionsFromXml(const std::string& p_strXmlFilePath)
{
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
