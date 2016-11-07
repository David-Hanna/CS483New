#include "ComponentTrack.h"

#include "KeyboardInputBuffer.h"
#include "EventManager.h"
#include <iostream>

namespace Kartaclysm
{
	ComponentTrack::ComponentTrack(HeatStroke::GameObject* p_pGameObject, const std::string& p_strTrackName)
		:
		Component(p_pGameObject),
		m_strTrackName(p_strTrackName),
		m_vTrackPieces()
	{
		std::function<void(const HeatStroke::Event*)>* func = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::RacerTrackPieceUpdatedDelegate, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerTrackPieceUpdated", func);
	}

	ComponentTrack::~ComponentTrack()
	{
	}

	HeatStroke::Component* ComponentTrack::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
	{
		printf("Created component!\n");

		return new ComponentTrack(p_pGameObject, "test track name");
	}

	void ComponentTrack::Init()
	{
		// generate list of track pieces and racers
		int iNumTrackPieces = m_vTrackPieces.size() - 1;
		for (HeatStroke::GameObject* pChildGameObject : m_pGameObject->GetChildren())
		{
			for (const std::string& strTag : pChildGameObject->GetTagList())
			{
				if (strcmp(strTag.c_str(), "Trackpiece") == 0)
				{
					m_vTrackPieces.push_back(pChildGameObject);
					break;
				}
				else if (strcmp(strTag.c_str(), "Racer") == 0)
				{
					m_vRacers.push_back({pChildGameObject->GetGUID(), iNumTrackPieces, 0});
				}
			}
		}
	}

	void ComponentTrack::Update(const float p_fDelta)
	{
		// TODO: remove
		// Matt: just here to simulate a racer going around the track
		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_P))
		{
			int iRacerIndex = GetRacerIndex("Kart");
			int iNextTrackIndex = m_vRacers[iRacerIndex].currentTrackPiece + 1;
			if (iNextTrackIndex >= m_vTrackPieces.size())
			{
				iNextTrackIndex = 0;
			}

			HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
			pEvent->SetStringParameter("racerId", m_vRacers[iRacerIndex].id);
			pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[iNextTrackIndex]->GetGUID());
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}

		// TODO: remove
		// Matt: just here to simulate an opponent going around the track
		if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_O))
		{
			int iRacerIndex = GetRacerIndex("Opponent");
			int iNextTrackIndex = m_vRacers[iRacerIndex].currentTrackPiece + 1;
			if (iNextTrackIndex >= m_vTrackPieces.size())
			{
				iNextTrackIndex = 0;
			}

			m_pGameObject->GetManager()->GetGameObject("Opponent")->GetTransform().SetTranslation(m_vTrackPieces[iNextTrackIndex]->GetTransform().GetTranslation());

			HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
			pEvent->SetStringParameter("racerId", m_vRacers[iRacerIndex].id);
			pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[iNextTrackIndex]->GetGUID());
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}

		UpdateRacerPositions();

		PrintRacerPositions();
	}

	void ComponentTrack::RacerTrackPieceUpdatedDelegate(const HeatStroke::Event* p_pEvent)
	{
		std::string strRacerId = "";
		std::string strTrackPieceId = "";
		p_pEvent->GetRequiredStringParameter("racerId", strRacerId);
		p_pEvent->GetRequiredStringParameter("TrackPieceId", strTrackPieceId);

		int iRacerIndex = GetRacerIndex(strRacerId);
		int iTrackPieceIndex = GetTrackPieceIndex(strTrackPieceId);
		if (iTrackPieceIndex < 0 || iRacerIndex < 0)
		{
			return;
		}

		if (iTrackPieceIndex == 0 && m_vRacers[iRacerIndex].currentTrackPiece == m_vTrackPieces.size() - 1)
		{
			m_vRacers[iRacerIndex].currentTrackPiece = 0;
			m_vRacers[iRacerIndex].currentLap++;
		}
		else if (iTrackPieceIndex == m_vRacers[iRacerIndex].currentTrackPiece + 1)
		{
			m_vRacers[iRacerIndex].currentTrackPiece = iTrackPieceIndex;
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

		return -1;
	}

	int ComponentTrack::GetRacerIndex(const std::string& p_strRacerId)
	{
		const char* strRacerId = p_strRacerId.c_str();
		for (unsigned int i = 0; i < m_vRacers.size(); ++i)
		{
			if (strcmp(strRacerId, m_vRacers[i].id.c_str()) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	void ComponentTrack::UpdateRacerPositions()
	{
		for (int i = 1; i < m_vRacers.size(); ++i)
		{
			RacerMock racerA = m_vRacers[i];
			bool bRacerPositionUpdated = false;
			for (int j = i - 1; j >= 0; --j)
			{
				RacerMock racerB = m_vRacers[j];
				if (IsAhead(racerA, racerB))
				{
					m_vRacers[i] = racerB;
					m_vRacers[j] = racerA;
					bRacerPositionUpdated = true;
					TriggerRacerPositionUpdateEvent(racerB.id);
				}
			}

			if (bRacerPositionUpdated)
			{
				TriggerRacerPositionUpdateEvent(racerA.id);
			}
		}
	}

	bool ComponentTrack::IsAhead(const RacerMock& p_RacerA, const RacerMock& p_RacerB)
	{
		// ahead by lap
		if (p_RacerA.currentLap > p_RacerB.currentLap)
		{
			return true;
		}
		else if (p_RacerA.currentLap < p_RacerB.currentLap)
		{
			return false;
		}
		else
		{
			// ahead by track piece
			if (p_RacerA.currentTrackPiece > p_RacerB.currentTrackPiece)
			{
				return true;
			}
			else if (p_RacerA.currentTrackPiece < p_RacerB.currentTrackPiece)
			{
				return false;
			}
			else
			{
				// ahead by distance
				int iNextPieceIndex = p_RacerA.currentTrackPiece + 1;
				if (iNextPieceIndex >= m_vTrackPieces.size())
				{
					iNextPieceIndex = 0;
				}

				glm::vec3 vNextTrackPiecePosition = m_vTrackPieces[iNextPieceIndex]->GetTransform().GetTranslation();
				glm::vec3 vRacerPosition = m_pGameObject->GetManager()->GetGameObject(p_RacerA.id)->GetTransform().GetTranslation();
				glm::vec3 vOpponentPosition = m_pGameObject->GetManager()->GetGameObject(p_RacerB.id)->GetTransform().GetTranslation();

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

	void ComponentTrack::PrintRacerPositions()
	{
		printf("**************************************************\n");
		for (int i = 0; i < m_vRacers.size(); ++i)
		{
			RacerMock racer = m_vRacers[i];
			printf("position: %i\tid: %s\tlap: %i\tpiece: %i\n", i + 1, racer.id.c_str(), racer.currentLap, racer.currentTrackPiece);
		}
	}
}