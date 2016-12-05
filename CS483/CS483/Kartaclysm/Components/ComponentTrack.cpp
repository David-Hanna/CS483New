#include "ComponentTrack.h"

//#include "KeyboardInputBuffer.h"
#include "EventManager.h"
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
		m_pRacerTrackDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::OnRacerTrackPieceCollision, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerTrackPieceUpdated", m_pRacerTrackDelegate);

		m_pRegisterDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::RegisterForTrackHeight, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("TrackHeightRegister", m_pRegisterDelegate);
	}

	ComponentTrack::~ComponentTrack()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("RacerTrackPieceUpdated", m_pRacerTrackDelegate);
		delete m_pRacerTrackDelegate;
		m_pRacerTrackDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener("TrackHeightRegister", m_pRegisterDelegate);
		delete m_pRegisterDelegate;
		m_pRegisterDelegate = nullptr;
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

		// generate list of racers
		// TODO: generate based on actual racers
		int iNumTrackPieces = m_vTrackPieces.size() - 1;
		m_vRacers = {
			{"Kart", iNumTrackPieces, 0},
			{"Opponent", iNumTrackPieces, 0}
		};
	}

	void ComponentTrack::Update(const float p_fDelta)
	{
		// Iterate through track pieces to find which karts are located on them
		for (unsigned int i = 0; i < m_vTrackPieces.size(); ++i)
		{
			ComponentTrackPiece* trackComponent = (ComponentTrackPiece*)m_vTrackPieces[i]->GetComponent("GOC_TrackPiece");

			if (trackComponent != nullptr)
			{
				// racer collision
				for (unsigned int j = 0; j < m_vRacers.size(); ++j)
				{
					HeatStroke::GameObject* racerObject = m_pGameObject->GetManager()->GetGameObject(m_vRacers[j].id);

					if (trackComponent->CheckInBounds(racerObject->GetTransform().GetTranslation()))
					{
						HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
						pEvent->SetStringParameter("racerId", m_vRacers[j].id);
						pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[i]->GetGUID());
						HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
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

		// TODO: remove
		// Matt: just here to simulate an opponent going around the track
		/*if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDownOnce(GLFW_KEY_O))
		{
			int iRacerIndex = GetRacerIndex("Opponent");
			int iNextTrackIndex = GetNextTrackPieceIndex(m_vRacers[iRacerIndex].currentTrackPiece);

			m_pGameObject->GetManager()->GetGameObject("Opponent")->GetTransform().SetTranslation(m_vTrackPieces[iNextTrackIndex]->GetTransform().GetTranslation());

			HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
			pEvent->SetStringParameter("racerId", m_vRacers[iRacerIndex].id);
			pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[iNextTrackIndex]->GetGUID());
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}*/

		UpdateRacerPositions();
		CheckRacerFacingForward();

		//PrintRacerPositions();
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
		int iTrackPieceIndex = GetTrackPieceIndex(strTrackPieceId);
		if (iTrackPieceIndex < 0 || iTrackPieceIndex >= m_vTrackPieces.size() || iRacerIndex < 0)
		{
			return;
		}

		// update lap/track piece information
		if (iTrackPieceIndex == 0 && m_vRacers[iRacerIndex].currentTrackPiece == m_vTrackPieces.size() - 1)
		{
			m_vRacers[iRacerIndex].currentTrackPiece = 0;
			m_vRacers[iRacerIndex].currentLap++;
		}
		else if (iTrackPieceIndex == m_vRacers[iRacerIndex].currentTrackPiece + 1)
		{
			m_vRacers[iRacerIndex].currentTrackPiece = iTrackPieceIndex;
		}

		// update track height for racer
		ComponentKartController* kartController = (ComponentKartController*)m_pGameObject->GetManager()->GetGameObject(strRacerId)->GetComponent("GOC_KartController");
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

		return -1;
	}

	int ComponentTrack::GetNextTrackPieceIndex(int p_iCurrentTrackPieceIndex)
	{
		int iNextTrackPieceIndex = p_iCurrentTrackPieceIndex + 1;
		return iNextTrackPieceIndex >= m_vTrackPieces.size() ? 0 : iNextTrackPieceIndex;
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
		// TODO: optimize
		// Matt: This is currently O(n^2).  While n will always be small, and it likely won't be a problem,
		//			this can still be improved.  Something to pick up on a rainy day.
		for (int i = 1; i < m_vRacers.size(); ++i)
		{
			RacerData racerA = m_vRacers[i];
			bool bRacerPositionUpdated = false;
			for (int j = i - 1; j >= 0; --j)
			{
				RacerData racerB = m_vRacers[j];
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

	// TODO: move to component of racer object once that is created
	void ComponentTrack::CheckRacerFacingForward()
	{
		for (RacerData racer : m_vRacers)
		{
			glm::vec3 trackForwardDirection = DetermineTrackForwardDirection(racer.currentTrackPiece);
			glm::vec3 racerForwardDirection = DetermineRacerForwardDirection(racer.id);

			// TO DO, handle event better (toggle it, don't send every frame) and don't hardcode it Player0, Player1, etc.
			int iRacer = (racer.id == "Opponent" ? 1 : 0);
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
	bool ComponentTrack::IsAhead(const RacerData& p_RacerA, const RacerData& p_RacerB)
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
				int iNextPieceIndex = GetNextTrackPieceIndex(p_RacerA.currentTrackPiece);

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
			RacerData racer = m_vRacers[i];
			printf("position: %i\tid: %s\tlap: %i\tpiece: %i\n", i + 1, racer.id.c_str(), racer.currentLap, racer.currentTrackPiece);
		}
	}
}