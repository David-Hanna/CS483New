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
		m_vTrackPieces()
	{
		m_pRacerTrackPieceUpdatedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::OnRacerTrackPieceCollision, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerTrackPieceUpdated", m_pRacerTrackPieceUpdatedDelegate);
	}

	ComponentTrack::~ComponentTrack()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RacerTrackPieceUpdated", m_pRacerTrackPieceUpdatedDelegate);
		delete m_pRacerTrackPieceUpdatedDelegate;
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
		// Iterate through track pieces to find which karts are located on them
		for (unsigned int i = 0; i < m_vTrackPieces.size(); ++i)
		{
			ComponentTrackPiece* trackComponent = (ComponentTrackPiece*)m_vTrackPieces[i]->GetComponent("GOC_TrackPiece");

			if (trackComponent != nullptr)
			{
				for (unsigned int j = 0; j < m_vRacers.size(); ++j)
				{
					HeatStroke::GameObject* pRacerObject = m_vRacers[j]->GetGameObject();

					if (trackComponent->CheckInBounds(pRacerObject->GetTransform().GetTranslation()))
					{
						HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
						pEvent->SetStringParameter("racerId", pRacerObject->GetGUID());
						pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[i]->GetGUID());
						HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
					}
				}
			}
		}

		UpdateRacerPositions();
		CheckRacerFacingForward();
	}

	void ComponentTrack::RegisterRacer(ComponentRacer* p_pRacer)
	{
		p_pRacer->SetCurrentLap(0);
		p_pRacer->SetCurrentTrackPiece(m_vTrackPieces.size() - 1);

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
		int iRacerCurrentTrackPiece = m_vRacers[iRacerIndex]->GetCurrentTrackPiece();
		if (iTrackPieceIndex == 0 && iRacerCurrentTrackPiece == m_vTrackPieces.size() - 1)
		{
			m_vRacers[iRacerIndex]->SetCurrentTrackPiece(0);
			TriggerRacerCompletedLapEvent(m_vRacers[iRacerIndex]->GetGameObject()->GetGUID());
		}
		else if (iTrackPieceIndex == iRacerCurrentTrackPiece + 1)
		{
			//TODO: fix issue where wrong way is relative to most forward piece, rather than current piece
			m_vRacers[iRacerIndex]->SetCurrentTrackPiece(iTrackPieceIndex);
		}

		// update track height for racer
		ComponentKartController* kartController = (ComponentKartController*)m_vRacers[iRacerIndex]->GetGameObject()->GetComponent("GOC_KartController");
		ComponentTrackPiece* trackPiece = (ComponentTrackPiece*)m_pGameObject->GetManager()->GetGameObject(strTrackPieceId)->GetComponent("GOC_TrackPiece");
		if (kartController != nullptr && trackPiece != nullptr)
		{
			kartController->UpdateTrackHeight(trackPiece->HeightAtPosition(kartController->GetGameObject()->GetTransform().GetTranslation()));
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
		for (unsigned int i = 1; i < m_vRacers.size(); ++i)
		{
			ComponentRacer* racerA = m_vRacers[i];
			bool bRacerPositionUpdated = false;
			for (int j = i - 1; j >= 0; --j)
			{
				ComponentRacer* racerB = m_vRacers[j];
				if (IsAhead(racerA, racerB))
				{
					m_vRacers[i] = racerB;
					m_vRacers[j] = racerA;
					bRacerPositionUpdated = true;
					TriggerRacerPositionUpdateEvent(racerB->GetGameObject()->GetGUID());
				}
			}

			if (bRacerPositionUpdated)
			{
				TriggerRacerPositionUpdateEvent(racerA->GetGameObject()->GetGUID());
			}
		}
	}

	// TODO: move to component of racer object once that is created
	void ComponentTrack::CheckRacerFacingForward()
	{
		for (ComponentRacer* pRacer : m_vRacers)
		{
			glm::vec3 trackForwardDirection = DetermineTrackForwardDirection(pRacer->GetCurrentTrackPiece());
			glm::vec3 racerForwardDirection = DetermineRacerForwardDirection(pRacer->GetGameObject()->GetGUID());

			// TO DO, handle event better (toggle it, don't send every frame) and don't hardcode it Player0, Player1, etc.
			int iRacer = (pRacer->GetGameObject()->GetGUID() == "Opponent" ? 1 : 0);
			HeatStroke::Event* pEvent = new HeatStroke::Event("Player" + std::to_string(iRacer) + "_HUD_WrongWay");
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
		HeatStroke::Event* pEvent = new HeatStroke::Event("RacerPositionUpdated");
		pEvent->SetStringParameter("racerId", p_strRacerId);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentTrack::TriggerRacerCompletedLapEvent(const std::string& p_strRacerId)
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RacerCompletedLap");
		pEvent->SetStringParameter("racerId", p_strRacerId);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}
}