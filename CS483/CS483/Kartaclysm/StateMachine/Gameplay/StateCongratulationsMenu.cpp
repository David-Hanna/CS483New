//------------------------------------------------------------------------
// StateCongratulationsMenu
// Author:	Bradley Cooper
//	
// Shows a congratulations message after a tournament.
// Continues to main menu.
//------------------------------------------------------------------------

#include "StateCongratulationsMenu.h"

Kartaclysm::StateCongratulationsMenu::StateCongratulationsMenu()
	:
	GameplayState("Congratulations"),
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
{
}

Kartaclysm::StateCongratulationsMenu::~StateCongratulationsMenu()
{
}

void Kartaclysm::StateCongratulationsMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	std::map<std::string, std::string> mResults = p_mContextParameters;
	m_pGameObjectManager = new HeatStroke::GameObjectManager();

	m_pGameObjectManager->RegisterComponentFactory("GOC_OrthographicCamera", HeatStroke::ComponentOrthographicCamera::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_Sprite", HeatStroke::ComponentSprite::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_TextBox", HeatStroke::ComponentTextBox::CreateComponent);
	m_pGameObjectManager->RegisterComponentFactory("GOC_PerspectiveCamera", HeatStroke::ComponentPerspectiveCamera::CreateComponent);

	std::vector<Medalist> vHumanMedalists;
	DetermineHumanRacersEarningMedals(mResults, &vHumanMedalists);
	unsigned int uiMedalCount = CreateMedalsForWinners(vHumanMedalists);

	if (uiMedalCount == 0)
	{
		// TODO: Maybe a "Better luck next time!" kinda message
		m_pStateMachine->Pop();
		if (m_pStateMachine->empty())
		{
			m_pStateMachine->Push(STATE_MAIN_MENU);
		}
		return;
	}
	
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Camera/camera_menu.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/background.xml");
	m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/CongratulationsMenu/congratulations.xml");

	// TODO: More fanfare (cool title message, better music, fireworks, etc.)
	
	if (HeatStroke::AudioPlayer::Instance()->GetCurrentMusicFile() != "Assets/Music/FunkyChunk.ogg")
	{
		HeatStroke::AudioPlayer::Instance()->StopMusic();
		HeatStroke::AudioPlayer::Instance()->OpenMusicFromFile("Assets/Music/FunkyChunk.ogg");
		HeatStroke::AudioPlayer::Instance()->PlayMusic();
	}
}

void Kartaclysm::StateCongratulationsMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);

		bool bUp, bDown, bLeft, bRight, bConfirm, bCancel;
		PlayerInputMapping::Instance()->QueryPlayerMenuActions(0, bUp, bDown, bLeft, bRight, bConfirm, bCancel);

		if (bConfirm || bCancel)
		{
			m_pStateMachine->Pop();
			if (m_pStateMachine->empty())
			{
				m_pStateMachine->Push(STATE_MAIN_MENU);
			}
		}
	}
}

void Kartaclysm::StateCongratulationsMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateCongratulationsMenu::Exit()
{
	m_bSuspended = true;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}

void Kartaclysm::StateCongratulationsMenu::DetermineHumanRacersEarningMedals(
	const std::map<std::string, std::string>& p_mRaceResults,
	std::vector<Kartaclysm::StateCongratulationsMenu::Medalist>* p_pHumanMedalists) const
{
	int iNumRacers = std::stoi(p_mRaceResults.at("numRacers"));
	std::string strCutoff = std::to_string((iNumRacers > 4 ? 4 : iNumRacers) - 1);

	for (int i = 0; i < iNumRacers; ++i)
	{
		std::string strIndex = std::to_string(i);
		std::string strRacerPosition = p_mRaceResults.at("racerPosition" + strIndex);

		if (strRacerPosition == "dnf" || strRacerPosition > strCutoff)
		{
			continue;
		}

		int iPosition = std::stoi(strRacerPosition);
		std::string strRacerGuid = p_mRaceResults.at("racerId" + strIndex);

		// TODO: Better way to make sure they are human
		if (strRacerGuid.at(0) != 'P')
		{
			continue;
		}

		p_pHumanMedalists->push_back(Medalist(iPosition, strRacerGuid));
	}
}

unsigned int Kartaclysm::StateCongratulationsMenu::CreateMedalsForWinners(const std::vector<Kartaclysm::StateCongratulationsMenu::Medalist>& p_vHumanMedalists)
{
	if (p_vHumanMedalists.empty()) return 0;

	std::vector<HeatStroke::GameObject*> vMedalGOs;
	for (auto medalist : p_vHumanMedalists)
	{
		std::string strMedal = "";
		switch (medalist.m_iPosition)
		{
			case 1: strMedal = "gold_medal";	break;
			case 2: strMedal = "silver_medal";	break;
			case 3: strMedal = "bronze_medal";	break;
			default:
#ifdef _DEBUG
				assert(false && "No medal for other positions");
#endif
				continue;
		}

		HeatStroke::GameObject* pMedal = m_pGameObjectManager->CreateGameObject("CS483/CS483/Kartaclysm/Data/Menus/CongratulationsMenu/" + strMedal + ".xml");
		SetTextBeneathMedal(pMedal, medalist.m_strRacerId);
		vMedalGOs.push_back(pMedal);
	}

	PositionMedalRenderables(vMedalGOs);
	return vMedalGOs.size();
}

void Kartaclysm::StateCongratulationsMenu::SetTextBeneathMedal(HeatStroke::GameObject* p_pMedal, const std::string& p_strText) const
{
	auto vTextRenderables = p_pMedal->GetChildrenWithTag("Text");
	if (vTextRenderables.empty())
	{
#ifdef _DEBUG
		assert(false && "Missing child with \"Text\" tag");
#endif
		return;
	}

	if (HeatStroke::ComponentTextBox* pText = dynamic_cast<HeatStroke::ComponentTextBox*>(vTextRenderables.at(0)->GetComponent("GOC_Renderable")))
	{
		pText->SetMessage(p_strText);
	}
#ifdef _DEBUG
	else
	{
		assert(false && "Missing textbox renderable");
	}
#endif
}

void Kartaclysm::StateCongratulationsMenu::PositionMedalRenderables(const std::vector<HeatStroke::GameObject*>& p_vMedalGOs)
{
	if (p_vMedalGOs.empty()) return;

	float fHorizontalSpace = 200.0f / (p_vMedalGOs.size() + 1);
	for (unsigned int i = 1; i <= p_vMedalGOs.size(); ++i)
	{
		auto pTransform = &p_vMedalGOs.at(i-1)->GetTransform();
		pTransform->TranslateXYZ(-100.0f + (i * fHorizontalSpace), 0.0f, 0.0f);
	}
}