//------------------------------------------------------------------------
// StateCountdown
// Author:	Bradley Cooper
//	
// Gameplay state for the countdown at start of races.
//------------------------------------------------------------------------

#include "StateCountdown.h"

Kartaclysm::StateCountdown::StateCountdown()
	:
	GameplayState("Countdown"),
	m_pGameObjectManager(nullptr),
	m_uiNumHumanRacers(0),
	m_fTimer(0.0f),
	m_mRacerBoostMap(),
	m_mPreviousFrameAccelerating(),
	m_bSuspended(true)
{
}

Kartaclysm::StateCountdown::~StateCountdown()
{
}

void Kartaclysm::StateCountdown::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;
	m_fTimer = 3.0f;

	SendKartDisableEvent();

	m_uiNumHumanRacers = std::stoi(p_mContextParameters.at("NumHumanRacers"));
	for (unsigned int i = 0; i < m_uiNumHumanRacers; ++i)
	{
		std::string strPlayerX = "Player" + std::to_string(i);
		m_mRacerBoostMap[strPlayerX] = false;
		m_mPreviousFrameAccelerating[i] = false;
	}

	unsigned int uiNumAIRacers = std::stoi(p_mContextParameters.at("NumAIRacers"));
	DetermineAIBoost(uiNumAIRacers);
}

void Kartaclysm::StateCountdown::Update(const float p_fDelta)
{
	if (m_bSuspended) return;

	m_fTimer -= p_fDelta;

	SendHudCountdownEvent(m_fTimer);

	if (m_fTimer <= 0.0f)
	{
		SendCountdownBoostEvent();
		m_pStateMachine->Pop();
	}
	else
	{
		UpdateHumanBoostValues();
	}
}

void Kartaclysm::StateCountdown::Exit()
{
	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}

	m_bSuspended = true;
}

void Kartaclysm::StateCountdown::SendKartDisableEvent()
{
	HeatStroke::Event* pEvent = new HeatStroke::Event("KartCountdown");
	pEvent->SetIntParameter("Disable", 1);
	HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
}

void Kartaclysm::StateCountdown::DetermineAIBoost(unsigned int p_uiNumAIRacers)
{
	std::bernoulli_distribution bool_chance(0.5f);
	for (unsigned int i = 0; i < p_uiNumAIRacers; ++i)
	{
		std::string strAIRacer = "AI_racer" + std::to_string(i);
		m_mRacerBoostMap[strAIRacer] = bool_chance(HeatStroke::Common::GetRNGesus());
	}
}

void Kartaclysm::StateCountdown::UpdateHumanBoostValues()
{
	for (int i = 0; i < m_uiNumHumanRacers; i++)
	{
		int iAccelerate, iBrake, iSlide;
		float fTurn;
		PlayerInputMapping::Instance()->QueryPlayerMovement(i, iAccelerate, iBrake, iSlide, fTurn);

		std::string strPlayerX = "Player" + std::to_string(i);
		bool bAccelerating = (iAccelerate != 0);

		DetermineHumanBoost(&m_mRacerBoostMap[strPlayerX], bAccelerating, m_mPreviousFrameAccelerating[i]);
		m_mPreviousFrameAccelerating[i] = bAccelerating;
	}
}

void Kartaclysm::StateCountdown::DetermineHumanBoost(bool* p_pCurrentBoostValue, const bool p_bAcceleratingThisFrame, const bool p_bAcceleratingLastFrame)
{
	if (m_fTimer <= 0.25f)
	{
		if (!p_bAcceleratingThisFrame)
		{
			*p_pCurrentBoostValue = false;
		}
	}
	else if (m_fTimer <= 0.5f)
	{
		if (p_bAcceleratingThisFrame)
		{
			bool bBeganAccelerationInThisPhase = (*p_pCurrentBoostValue || !p_bAcceleratingLastFrame);
			*p_pCurrentBoostValue = bBeganAccelerationInThisPhase;
		}
		else
		{
			*p_pCurrentBoostValue = false;
		}
	}
}

void Kartaclysm::StateCountdown::SendHudCountdownEvent(const float p_fTimer) const
{
	HeatStroke::Event* pEvent = new HeatStroke::Event("Countdown_HUD");
	pEvent->SetFloatParameter("Countdown", p_fTimer);
	HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
}

void Kartaclysm::StateCountdown::SendCountdownBoostEvent() const
{
	HeatStroke::Event* pEvent = new HeatStroke::Event("KartCountdown");
	pEvent->SetIntParameter("Disable", 0);
	for (auto boost : m_mRacerBoostMap)
	{
		pEvent->SetFloatParameter(boost.first, (boost.second ? 1.3f : 0.0f));
	}
	HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
}