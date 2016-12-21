#include "ComponentTrack.h"

//#include "KeyboardInputBuffer.h"
#include "ComponentTrackPiece.h"
//#include <iostream>

namespace Kartaclysm
{
	ComponentTrack::ComponentTrack(HeatStroke::GameObject* p_pGameObject, const std::string& p_strTrackName)
		:
		Component(p_pGameObject),
		m_strTrackName(p_strTrackName),
		m_vTrackPieces(),
		m_fRaceTime(0.0f)
	{
		m_pRacerTrackPieceUpdatedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::OnRacerTrackPieceCollision, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerTrackPieceUpdated", m_pRacerTrackPieceUpdatedDelegate);

		m_pRegisterDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::RegisterForTrackHeight, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("TrackHeightRegister", m_pRegisterDelegate);
	}

	ComponentTrack::~ComponentTrack()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RacerTrackPieceUpdated", m_pRacerTrackPieceUpdatedDelegate);
		delete m_pRacerTrackPieceUpdatedDelegate;

		HeatStroke::EventManager::Instance()->RemoveListener("TrackHeightRegister", m_pRegisterDelegate);
		delete m_pRegisterDelegate;
	}

	HeatStroke::Component* ComponentTrack::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
	{
		std::string strTrackName = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(p_pBaseNode->FirstChildElement("Name"), "value", strTrackName);

		return new ComponentTrack(p_pGameObject, strTrackName);
	}

	void ComponentTrack::Init()
	{
		// generate list of track pieces
		for (HeatStroke::GameObject* pChildGameObject : m_pGameObject->GetChildren())
		{
			for (const std::string& strTag : pChildGameObject->GetTagList())
			{
				if (strcmp(strTag.c_str(), "Trackpiece") == 0)
				{
					m_vTrackPieces.push_back(pChildGameObject);
					break;
				}
			}
		}
	}

	void ComponentTrack::Update(const float p_fDelta)
	{
		m_fRaceTime += p_fDelta;

		// Iterate through track pieces to find which karts are located on them
		std::vector<bool> racersOnTrackPieces(m_vRacers.size(), false);
		for (unsigned int i = 0; i < m_vTrackPieces.size(); ++i)
		{
			ComponentTrackPiece* trackComponent = (ComponentTrackPiece*)m_vTrackPieces[i]->GetComponent("GOC_TrackPiece");

			if (trackComponent != nullptr)
			{
				// racer collision
				for (unsigned int j = 0; j < m_vRacers.size(); ++j)
				{
					HeatStroke::GameObject* pRacerObject = m_vRacers[j]->GetGameObject();

					if (trackComponent->CheckInBounds(pRacerObject->GetTransform().GetTranslation()))
					{
						HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
						pEvent->SetStringParameter("racerId", pRacerObject->GetGUID());
						pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[i]->GetGUID());
						HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
						racersOnTrackPieces[j] = true;
					}
				}

				// physics object heights
				auto it = m_vPhysicsObjects.begin(), end = m_vPhysicsObjects.end();
				for (; it != end; it++)
				{
					HeatStroke::GameObject* pPhysicsObject = it->second->GetGameObject();

					if (trackComponent->CheckInBounds(pPhysicsObject->GetTransform().GetTranslation()))
					{
						ComponentTrackPiece* trackPiece = static_cast<ComponentTrackPiece*>(m_vTrackPieces[i]->GetComponent("GOC_TrackPiece"));
						it->second->UpdateTrackHeight(trackPiece->HeightAtPosition(pPhysicsObject->GetTransform().GetTranslation()));
					}
				}
			}
		}

		for (int i = 0; i < racersOnTrackPieces.size(); ++i)
		{
			if (!racersOnTrackPieces[i])
			{
				ResetRacerPosition(m_vRacers[i]);
			}
		}

		UpdateRacerPositions();
		CheckRacerFacingForward();
	}

	void ComponentTrack::RegisterRacer(ComponentRacer* p_pRacer)
	{
		p_pRacer->SetCurrentLap(0);
		p_pRacer->SetCurrentTrackPiece(m_vTrackPieces.size() - 1);
		p_pRacer->SetFurthestTrackPiece(m_vTrackPieces.size() - 1);

		m_vRacers.push_back(p_pRacer);
	}

	void ComponentTrack::OnRacerTrackPieceCollision(const HeatStroke::Event* p_pEvent)
	{
		// parse event data
		std::string strRacerId = "";
		std::string strTrackPieceId = "";
		p_pEvent->GetRequiredStringParameter("racerId", strRacerId);
		p_pEvent->GetRequiredStringParameter("TrackPieceId", strTrackPieceId);

		// get racer and track piece
		int iRacerIndex = GetRacerIndex(strRacerId);
		unsigned int iTrackPieceIndex = GetTrackPieceIndex(strTrackPieceId);
		if (iTrackPieceIndex == UINT_MAX || iTrackPieceIndex >= m_vTrackPieces.size() || iRacerIndex < 0)
		{
			return;
		}

		// update lap/track piece information
		int iRacerFurthestTrackPiece = m_vRacers[iRacerIndex]->GetFurthestTrackPiece();
		int iRacerCurrentTrackPiece = m_vRacers[iRacerIndex]->GetCurrentTrackPiece();
		if (iTrackPieceIndex == 0 && iRacerFurthestTrackPiece == m_vTrackPieces.size() - 1)
		{
			m_vRacers[iRacerIndex]->SetFurthestTrackPiece(0);
			std::string strRacerId = m_vRacers[iRacerIndex]->GetGameObject()->GetGUID();
			TriggerRacerCompletedLapEvent(strRacerId);
			if (m_vRacers[iRacerIndex]->GetCurrentLap() > 3)
			{
				TriggerRacerFinishedRaceEvent(strRacerId);
			}
		}
		else if (iTrackPieceIndex == iRacerFurthestTrackPiece + 1)
		{
			m_vRacers[iRacerIndex]->SetFurthestTrackPiece(iTrackPieceIndex);
		}
		m_vRacers[iRacerIndex]->SetCurrentTrackPiece(iTrackPieceIndex);

		// update track height for racer
		ComponentKartController* kartController = (ComponentKartController*)m_vRacers[iRacerIndex]->GetGameObject()->GetComponent("GOC_KartController");
		ComponentTrackPiece* trackPiece = (ComponentTrackPiece*)m_pGameObject->GetManager()->GetGameObject(strTrackPieceId)->GetComponent("GOC_TrackPiece");
		if (kartController != nullptr && trackPiece != nullptr)
		{
			kartController->UpdateTrackHeight(trackPiece->HeightAtPosition(kartController->GetGameObject()->GetTransform().GetTranslation()));
		}
	}

	void ComponentTrack::RegisterForTrackHeight(const HeatStroke::Event* p_pEvent)
	{
		std::string strRegister, strUnregister;
		p_pEvent->GetOptionalGameObjectParameter("Register", strRegister, "");
		p_pEvent->GetOptionalGameObjectParameter("Unregister", strUnregister, "");

		if (strRegister != "")
		{
			HeatStroke::GameObject* pGO = GetGameObject()->GetManager()->GetGameObject(strRegister);
			ComponentSimplePhysics* pPhysics = static_cast<ComponentSimplePhysics*>(pGO->GetComponent("GOC_SimplePhysics"));
			m_vPhysicsObjects[strRegister] = pPhysics;
		}
		else if (strUnregister != "")
		{
			m_vPhysicsObjects.erase(m_vPhysicsObjects.find(strUnregister));
		}
	}

	int ComponentTrack::GetTrackPieceIndex(const std::string& p_strTrackPieceId)
	{
		const char* strTrackPieceId = p_strTrackPieceId.c_str();
		for (unsigned int i = 0; i < m_vTrackPieces.size(); ++i)
		{
			if (strcmp(strTrackPieceId, m_vTrackPieces[i]->GetGUID().c_str()) == 0)
			{
				return i;
			}
		}

		return UINT_MAX;
	}

	int ComponentTrack::GetNextTrackPieceIndex(int p_iCurrentTrackPieceIndex)
	{
		unsigned int iNextTrackPieceIndex = p_iCurrentTrackPieceIndex + 1;
		return iNextTrackPieceIndex >= m_vTrackPieces.size() ? 0 : iNextTrackPieceIndex;
	}

	int ComponentTrack::GetRacerIndex(const std::string& p_strRacerId)
	{
		const char* strRacerId = p_strRacerId.c_str();
		for (unsigned int i = 0; i < m_vRacers.size(); ++i)
		{
			if (strcmp(strRacerId, m_vRacers[i]->GetGameObject()->GetGUID().c_str()) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	void ComponentTrack::UpdateRacerPositions()
	{
		// TODO: optimize
		// Matt: This is currently O(n^2).  While n will always be small, and it likely won't be a problem,
		//			this can still be improved.  Something to pick up on a rainy day.
		bool bRaceStandingsUpdate = false;
		for (unsigned int i = 1; i < m_vRacers.size(); ++i)
		{
			bool bRacerPositionUpdated = false;
			ComponentRacer* racerA = m_vRacers[i];
			for (int j = i - 1; j >= 0; --j)
			{
				ComponentRacer* racerB = m_vRacers[j];
				if (IsAhead(racerA, racerB))
				{
					m_vRacers[i] = racerB;
					m_vRacers[j] = racerA;
					bRacerPositionUpdated = true;
					bRaceStandingsUpdate = true;
					TriggerRacerPositionUpdateEvent(racerB->GetGameObject()->GetGUID());
				}
			}

			if (bRacerPositionUpdated)
			{
				TriggerRacerPositionUpdateEvent(racerA->GetGameObject()->GetGUID());
			}
		}

		if (bRaceStandingsUpdate)
		{
			TriggerRaceStandingsUpdateEvent();
		}
	}

	void ComponentTrack::ResetRacerPosition(ComponentRacer* p_pRacer)
	{
		HeatStroke::GameObject* pRacerGameObject = p_pRacer->GetGameObject();
		glm::vec3 vResetPosition = m_vTrackPieces[p_pRacer->GetFurthestTrackPiece()]->GetTransform().GetTranslation();
		p_pRacer->GetGameObject()->GetTransform().SetTranslation(vResetPosition);
		static_cast<ComponentKartController*>(pRacerGameObject->GetComponent("GOC_KartController"))->SetSpeed(0.0f);
	}

	// TODO: move to component of racer object once that is created
	void ComponentTrack::CheckRacerFacingForward()
	{
		for (ComponentRacer* pRacer : m_vRacers)
		{
			glm::vec3 trackForwardDirection = DetermineTrackForwardDirection(pRacer->GetCurrentTrackPiece());
			glm::vec3 racerForwardDirection = DetermineRacerForwardDirection(pRacer->GetGameObject()->GetGUID());

			HeatStroke::Event* pEvent = new HeatStroke::Event(pRacer->GetGameObject()->GetGUID() + "_HUD_WrongWay");
			if (glm::dot(trackForwardDirection, racerForwardDirection) < 0.0f)
			{
				pEvent->SetIntParameter("Display", 1);
			}
			else
			{
				pEvent->SetIntParameter("Display", 0);
			}
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
	}

	glm::vec3 ComponentTrack::DetermineTrackForwardDirection(int p_iTrackPieceIndex)
	{
		int iNextTrackPieceIndex = GetNextTrackPieceIndex(p_iTrackPieceIndex);

		glm::vec3 vNextTrackPiecePos = m_vTrackPieces[iNextTrackPieceIndex]->GetTransform().GetTranslation();
		glm::vec3 vCurrentTrackPiecePos = m_vTrackPieces[p_iTrackPieceIndex]->GetTransform().GetTranslation();

		return vNextTrackPiecePos - vCurrentTrackPiecePos;
	}

	glm::vec3 ComponentTrack::DetermineRacerForwardDirection(const std::string& p_strRacerId)
	{
		HeatStroke::GameObject* pGameObject = m_pGameObject->GetManager()->GetGameObject(p_strRacerId);
		HeatStroke::Transform pRacerTransform = pGameObject->GetTransform();

		glm::vec3 vRacerForwardPosition = pRacerTransform.GetTranslation() + (pRacerTransform.GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));

		return vRacerForwardPosition - pRacerTransform.GetTranslation();
	}

	// TODO: clean this up
	// Matt: I don't really like the implementation of this function, but it will work for now
	bool ComponentTrack::IsAhead(ComponentRacer* p_pRacerA, ComponentRacer* p_pRacerB)
	{
		// ahead by lap
		if (p_pRacerA->GetCurrentLap() > p_pRacerB->GetCurrentLap())
		{
			return true;
		}
		else if (p_pRacerA->GetCurrentLap() < p_pRacerB->GetCurrentLap())
		{
			return false;
		}
		else
		{
			// ahead by track piece
			if (p_pRacerA->GetCurrentTrackPiece() > p_pRacerB->GetCurrentTrackPiece())
			{
				return true;
			}
			else if (p_pRacerA->GetCurrentTrackPiece() < p_pRacerB->GetCurrentTrackPiece())
			{
				return false;
			}
			else
			{
				// ahead by distance
				//TODO: this mathod for determining ahead by distance is not accurate in all cases.
				int iNextPieceIndex = GetNextTrackPieceIndex(p_pRacerA->GetCurrentTrackPiece());

				glm::vec3 vNextTrackPiecePosition = m_vTrackPieces[iNextPieceIndex]->GetTransform().GetTranslation();
				glm::vec3 vRacerPosition = p_pRacerA->GetGameObject()->GetTransform().GetTranslation();
				glm::vec3 vOpponentPosition = p_pRacerB->GetGameObject()->GetTransform().GetTranslation();

				return glm::distance(vNextTrackPiecePosition, vRacerPosition) < glm::distance(vNextTrackPiecePosition, vOpponentPosition);
			}
		}
	}

	void ComponentTrack::TriggerRacerPositionUpdateEvent(const std::string& p_strRacerId)
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RacerPositionUpdate");
		pEvent->SetStringParameter("racerId", p_strRacerId);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentTrack::TriggerRacerCompletedLapEvent(const std::string& p_strRacerId)
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RacerCompletedLap");
		pEvent->SetStringParameter("racerId", p_strRacerId);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentTrack::TriggerRacerFinishedRaceEvent(const std::string& p_strRacerId)
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RacerFinishedRace");
		pEvent->SetStringParameter("racerId", p_strRacerId);
		pEvent->SetFloatParameter("raceTime", m_fRaceTime);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentTrack::TriggerRaceStandingsUpdateEvent()
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RaceStandingsUpdate");
		for (unsigned int i = 0; i < m_vRacers.size(); ++i)
		{
			std::string strGUID = m_vRacers[i]->GetGameObject()->GetGUID();
			pEvent->SetIntParameter(strGUID, i);
			pEvent->SetStringParameter(std::to_string(i), strGUID);
		}
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}
}
