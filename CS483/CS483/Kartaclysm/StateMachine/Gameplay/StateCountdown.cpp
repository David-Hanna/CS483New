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

	m_iPlayerCount = PlayerInputMapping::Instance()->GetSplitscreenPlayers();
	for (int i = 0; i < m_iPlayerCount; i++)
	{
		m_vGainsBoost[i] = false;
		m_vAccelerating[i] = false;
	}
}

void Kartaclysm::StateCountdown::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		// Tell the HUD to update the countdown
		m_fTimer -= p_fDelta;
		HeatStroke::Event* pHudEvent = new HeatStroke::Event("Countdown_HUD");
		pHudEvent->SetIntParameter("Countdown", static_cast<int>(ceilf(m_fTimer)));
		HeatStroke::EventManager::Instance()->TriggerEvent(pHudEvent);

		// Check conditions for race start and boosts
		if (m_fTimer <= 0.0f)
		{
			// Enable kart movement and provide boosts
			HeatStroke::Event* pCountdownEvent = new HeatStroke::Event("KartCountdown");
			pCountdownEvent->SetIntParameter("Disable", 0);
			for (int i = 0; i < m_iPlayerCount; i++)
			{
				// FIXME - this causes a crash when running in debug in VS 2015
				pHudEvent->SetFloatParameter("Player" + std::to_string(i), (m_vGainsBoost[i] ? 1.3f : 0.0f));
			}
			HeatStroke::EventManager::Instance()->TriggerEvent(pCountdownEvent);

			m_pStateMachine->Pop();
		}
		else
		{
			// Get player accelerations to determine boost timing
			for (int i = 0; i < m_iPlayerCount; i++)
			{
				int iAccelerate, iBrake, iSlide;
				float fTurn;
				PlayerInputMapping::Instance()->QueryPlayerMovement(i, iAccelerate, iBrake, iSlide, fTurn);

				if (m_fTimer <= 0.25f)
				{
					// Final stretch: must continue accelerating
					if (iAccelerate == 0)
					{
						m_vGainsBoost[i] = false;
					}
				}
				else if (m_fTimer <= 0.5f)
				{
					// Sweet spot: Must begin acceleration in this phase
					if (iAccelerate && (!m_vAccelerating[i] || m_vGainsBoost[i]))
					{
						m_vGainsBoost[i] = true;
					}
					else
					{
						m_vGainsBoost[i] = false;
					}
				}

				// Update acceleration status for previous frame
				m_vAccelerating[i] = (iAccelerate != 0);
			}
		}
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