#include "ComponentTrack.h"

#include "ComponentTrackPiece.h"
#include "ComponentAIDriver.h"

namespace Kartaclysm
{
	ComponentTrack::ComponentTrack(HeatStroke::GameObject* p_pGameObject, const std::string& p_strTrackName, std::vector<PathNode>& p_vNodes, std::vector<NodeTrigger>& p_vNodeTriggers)
		:
		Component(p_pGameObject),
		m_strTrackName(p_strTrackName),
		m_vTrackPieces(),
		m_fRaceTime(-3.0f), // beginning countdown
		m_iLapsToFinishTrack(3), // value of 0 can be used for testing
		m_bRacerIsOffroad(false),
		m_vPathfindingNodes(p_vNodes),
		m_vNodeTriggers(p_vNodeTriggers),
		m_iLeadHumanPosition(0),
		m_iRearHumanPosition(0),
		m_bHumanPositionsDirty(true)
	{
		m_pRacerTrackPieceUpdatedDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::OnRacerTrackPieceCollision, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("RacerTrackPieceUpdated", m_pRacerTrackPieceUpdatedDelegate);

		m_pRegisterDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentTrack::RegisterForTrackHeight, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("TrackHeightRegister", m_pRegisterDelegate);
	}

	ComponentTrack::~ComponentTrack()
	{
		HeatStroke::AudioPlayer::Instance()->StopSoundEffect("Assets/Sounds/drive_on_grass.wav");

		HeatStroke::EventManager::Instance()->RemoveListener("RacerTrackPieceUpdated", m_pRacerTrackPieceUpdatedDelegate);
		delete m_pRacerTrackPieceUpdatedDelegate;

		HeatStroke::EventManager::Instance()->RemoveListener("TrackHeightRegister", m_pRegisterDelegate);
		delete m_pRegisterDelegate;
	}

	HeatStroke::Component* ComponentTrack::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
	{
		std::string strTrackName = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(p_pBaseNode->FirstChildElement("Name"), "value", strTrackName);

		std::vector<NodeTrigger> vNodeTriggers;
		std::vector<PathNode> vNodes = ParsePathfindingNodes(p_pBaseNode, &vNodeTriggers);

		return new ComponentTrack(p_pGameObject, strTrackName, vNodes, vNodeTriggers);
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

		// Assume false before this is re-flagged during this update loop by "RacerTrackPieceUpdated" events.
		m_bRacerIsOffroad = false;

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

		/*for (unsigned int i = 0; i < racersOnTrackPieces.size(); ++i)
		{
			if (!racersOnTrackPieces[i])
			{
				ResetRacerPosition(m_vRacers[i]);
			}
		}*/

		if (m_bRacerIsOffroad)
		{
			// AudioPlayer will not start sfx over if it's already playing.
			HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/drive_on_grass.wav", true);
		}
		else
		{
			HeatStroke::AudioPlayer::Instance()->StopSoundEffect("Assets/Sounds/drive_on_grass.wav");
		}

		UpdateRacerPositions();
		CheckRacerFacingForward();

		if (m_bHumanPositionsDirty)
		{
			UpdateHumanPositions();
		}
	}

	void ComponentTrack::RegisterRacer(ComponentRacer* p_pRacer)
	{
		p_pRacer->SetCurrentLap(0);
		p_pRacer->SetCurrentTrackPiece(m_vTrackPieces.size() - 1);
		p_pRacer->SetFurthestTrackPiece(m_vTrackPieces.size() - 1);

		m_vRacers.push_back(p_pRacer);

		// Queue HUD event for beginning lap count
		HeatStroke::Event* pEvent = new HeatStroke::Event(p_pRacer->GetGameObject()->GetGUID() + "_HUD_Lap");
		pEvent->SetIntParameter("Current", 1);
		pEvent->SetIntParameter("Total", m_iLapsToFinishTrack);
		HeatStroke::EventManager::Instance()->QueueEvent(pEvent);
	}

	void ComponentTrack::RegisterAIRacer(ComponentRacer* p_pRacer)
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
			if (m_vRacers[iRacerIndex]->GetCurrentLap() > m_iLapsToFinishTrack && !m_vRacers[iRacerIndex]->HasFinishedRace())
			{
				TriggerRacerFinishedRaceEvent(strRacerId);
			}
		}
		else if (iTrackPieceIndex == iRacerFurthestTrackPiece + 1)
		{
			m_vRacers[iRacerIndex]->SetFurthestTrackPiece(iTrackPieceIndex);
		}
		m_vRacers[iRacerIndex]->SetCurrentTrackPiece(iTrackPieceIndex);

		// update track height for racer + offroad
		ComponentKartController* kartController = (ComponentKartController*)m_vRacers[iRacerIndex]->GetGameObject()->GetComponent("GOC_KartController");
		ComponentTrackPiece* trackPiece = (ComponentTrackPiece*)m_pGameObject->GetManager()->GetGameObject(strTrackPieceId)->GetComponent("GOC_TrackPiece");
		if (kartController != nullptr && trackPiece != nullptr)
		{
			glm::vec3 pos = kartController->GetGameObject()->GetTransform().GetTranslation();
			kartController->UpdateTrackHeight(trackPiece->HeightAtPosition(pos));
			bool bOffroad = trackPiece->IsOffroadAtPosition(pos);
			kartController->SetOffroad(bOffroad);
			if (bOffroad)
			{
				m_bRacerIsOffroad = true;
			}

			// Dealing with triggered AI nodes
			ComponentAIDriver* aiDriver = static_cast<ComponentAIDriver*>(m_vRacers[iRacerIndex]->GetGameObject()->GetComponent("GOC_AIDriver"));
			if (aiDriver != nullptr)
			{
				//if (!kartController->IsAirborne()) // This doesn't work because IsAirborne won't have updated yet
				if (kartController->GetGameObject()->GetTransform().GetTranslation().y - kartController->GetGroundHeight() <= 1.0f)
				{
					for (unsigned int i = 0; i < m_vNodeTriggers.size(); i++)
					{
						if (strTrackPieceId.compare(m_vNodeTriggers[i].trackPieceIndex) == 0)
						{
							aiDriver->SetNode(m_vNodeTriggers[i].node);
						}
					}
				}
			}
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

	int ComponentTrack::GetNextTrackPieceIndex(int p_iCurrentTrackPieceIndex) const
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
			ComponentRacer* pOriginalRacer = m_vRacers[i];
			if (m_vRacers[i]->HasFinishedRace())
			{
				continue;
			}

			bool bRacerPositionUpdated = false;
			for (int j = i - 1; j >= 0; --j)
			{
				if (IsAhead(m_vRacers[i], m_vRacers[j]))
				{
					ComponentRacer* pTemp = m_vRacers[j];
					m_vRacers[j] = m_vRacers[i];
					m_vRacers[i] = pTemp;
					bRacerPositionUpdated = true;
					bRaceStandingsUpdate = true;
					TriggerRacerPositionUpdateEvent(pTemp->GetGameObject()->GetGUID());
				}
			}

			if (bRacerPositionUpdated)
			{
				TriggerRacerPositionUpdateEvent(pOriginalRacer->GetGameObject()->GetGUID());
			}
		}

		if (bRaceStandingsUpdate)
		{
			TriggerRaceStandingsUpdateEvent();
			m_bHumanPositionsDirty = true;
		}
	}

	void ComponentTrack::UpdateHumanPositions()
	{
		m_iLeadHumanPosition = GetNumberOfRacers();
		m_iRearHumanPosition = 0;

		for (unsigned int i = 1; i < m_vRacers.size(); ++i)
		{
			ComponentRacer* pRacer = m_vRacers[i];
			if (m_vRacers[i]->HasFinishedRace())
			{
				continue;
			}

			HeatStroke::Component* pAIDriver = pRacer->GetGameObject()->GetComponent("GOC_AIDriver");
			if (pAIDriver == nullptr) // only the finest way to check if a racer is human
			{
				int iPosition = pRacer->GetCurrentPosition();

				if (iPosition < m_iLeadHumanPosition)
				{
					m_iLeadHumanPosition = iPosition;
				}

				if (iPosition > m_iRearHumanPosition)
				{
					m_iRearHumanPosition = iPosition;
				}
			}
		}

		m_bHumanPositionsDirty = false;
	}

	int ComponentTrack::GetLeadHumanPosition()
	{
		if (m_bHumanPositionsDirty)
		{
			UpdateHumanPositions();
		}

		return m_iLeadHumanPosition;
	}

	int ComponentTrack::GetRearHumanPosition()
	{
		if (m_bHumanPositionsDirty)
		{
			UpdateHumanPositions();
		}

		return m_iRearHumanPosition;
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
		int iCurrentLapA = p_pRacerA->GetCurrentLap();
		int iCurrentLapB = p_pRacerB->GetCurrentLap();
		if (iCurrentLapA > iCurrentLapB)
		{
			return true;
		}
		else if (iCurrentLapA < iCurrentLapB)
		{
			return false;
		}
		else
		{
			// ahead by track piece
			int iCurrentTrackPieceA = p_pRacerA->GetCurrentTrackPiece();
			int iCurrentTrackPieceB = p_pRacerB->GetCurrentTrackPiece();
			if (iCurrentTrackPieceA > iCurrentTrackPieceB)
			{
				return true;
			}
			else if (iCurrentTrackPieceA < iCurrentTrackPieceB)
			{
				return false;
			}
			else
			{
				// ahead by distance
				glm::vec3 vRacerPositionA = p_pRacerA->GetGameObject()->GetTransform().GetTranslation();
				glm::vec3 vRacerPositionB = p_pRacerB->GetGameObject()->GetTransform().GetTranslation();

				ComponentTrackPiece* pTrackComponent = static_cast<ComponentTrackPiece*>(m_vTrackPieces[iCurrentTrackPieceA]->GetComponent("GOC_TrackPiece"));
				return pTrackComponent->IsAhead(vRacerPositionA, vRacerPositionB);
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
		pEvent->SetIntParameter("totalLaps", m_iLapsToFinishTrack);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentTrack::TriggerRacerFinishedRaceEvent(const std::string& p_strRacerId)
	{
		HeatStroke::Event* pEvent = new HeatStroke::Event("RacerFinishedRace");
		pEvent->SetStringParameter("racerId", p_strRacerId);
		pEvent->SetFloatParameter("racerTime", m_fRaceTime);
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

	ComponentTrack::PathNode ComponentTrack::GetNextNode(int p_iCurrentNodeIndex)
	{
		if (p_iCurrentNodeIndex >= m_vPathfindingNodes.size() - 1)
		{
			return m_vPathfindingNodes[0];
		}
		return m_vPathfindingNodes[p_iCurrentNodeIndex + 1];
	}

	std::vector<ComponentTrack::PathNode> ComponentTrack::ParsePathfindingNodes(tinyxml2::XMLNode* p_pRootNode, std::vector<NodeTrigger>* p_vNodeTriggers)
	{
		std::vector<PathNode> vNodes;
		tinyxml2::XMLElement* pPathfindingNodesElement = p_pRootNode->FirstChildElement("PathfindingNodes");
		if (pPathfindingNodesElement)
		{
			tinyxml2::XMLElement* pNodeElement = pPathfindingNodesElement->FirstChildElement("Node");
			int iIndex = 0;
			while (pNodeElement)
			{
				PathNode node;

				HeatStroke::EasyXML::GetRequiredFloatAttribute(pNodeElement, "x", node.x);
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pNodeElement, "z", node.z);
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pNodeElement, "variation", node.variation);
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pNodeElement, "radius", node.radius);

				std::string sTriggerIndex;
				std::string sDefault = ""; // yup
				HeatStroke::EasyXML::GetOptionalStringAttribute(pNodeElement, "trigger", sTriggerIndex, sDefault);

				node.index = iIndex++;

				if (sTriggerIndex.compare(sDefault) != 0)
				{
					NodeTrigger trigger;

					trigger.node = node;
					trigger.trackPieceIndex = sTriggerIndex;

					p_vNodeTriggers->push_back(trigger);
				}

				vNodes.push_back(node);
				pNodeElement = pNodeElement->NextSiblingElement("Node");
			}
		}

		return vNodes;
	}
}
