//----------------------------------------------------------------------------
// ComponentRacer
// Author: Matthew White
//
// Component that handles racer logic
//----------------------------------------------------------------------------

#include "ComponentRacer.h"

namespace Kartaclysm
{
	ComponentRacer::ComponentRacer(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pDriver(nullptr),
		m_pKart(nullptr),
		m_iCurrentLap(0),
		m_iCurrentPosition(1),
		m_iCurrentTrackPiece(0)
	{
		m_pLapCompleteDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentRacer::FinishLap, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerCompletedLap", m_pLapCompleteDelegate);

		m_pRaceFinishedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentRacer::FinishRace, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerFinishedRace", m_pRaceFinishedDelegate);
	}

	ComponentRacer::~ComponentRacer()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RacerCompletedLap", m_pLapCompleteDelegate);
		delete m_pLapCompleteDelegate;
		m_pLapCompleteDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener("RacerFinishedRace", m_pRaceFinishedDelegate);
		delete m_pRaceFinishedDelegate;
		m_pRaceFinishedDelegate = nullptr;
	}

	HeatStroke::Component* ComponentRacer::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		return new ComponentRacer(p_pGameObject);
	}

	void ComponentRacer::Update(const float p_fDelta)
	{
	}

	void ComponentRacer::FinishLap(const HeatStroke::Event* p_pEvent)
	{
		std::string strRacerId = "";
		p_pEvent->GetRequiredStringParameter("racerId", strRacerId);
		if (strRacerId == GetGameObject()->GetGUID())
		{
			HeatStroke::Event* pEvent = new HeatStroke::Event(strRacerId + "_HUD_Lap");
			pEvent->SetIntParameter("Current", ++m_iCurrentLap);
			pEvent->SetIntParameter("Total", 3);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	void ComponentRacer::FinishRace(const HeatStroke::Event* p_pEvent)
	{
		std::string strRacerId = "";
		float fRaceTime = 0.0f;
		p_pEvent->GetRequiredStringParameter("racerId", strRacerId);
		p_pEvent->GetRequiredFloatParameter("raceTime", fRaceTime);
		if (strRacerId == GetGameObject()->GetGUID())
		{
			printf("finished the race in %f seconds!\n", fRaceTime);
			HeatStroke::Event* pEvent = new HeatStroke::Event("RacerFinishedRace2");
			HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
		}
	}
}
