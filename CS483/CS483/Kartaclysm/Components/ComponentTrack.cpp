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

		//TEMP: used to test tracking racer progress
		m_vRacers = {
			{"racer1", 7, 0},
			{"racer2", 7, 0},
			{"racer3", 7, 0},
		};
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
		//TODO: change method for adding track pieces
		//Matt: current method assumes GameObject::GetChildren() returns them in the correct order, which probably isn't great. Good enough for now though
		std::vector<HeatStroke::GameObject*> vGameObjectChildren = m_pGameObject->GetChildren();

		//TODO: check that child is actually track pice
		//Matt: once tags are implemented, will check that each child has the 'TrackPiece' tag before adding

		m_vTrackPieces = vGameObjectChildren;
	}

	void ComponentTrack::Update(const float p_fDelta)
	{
		//TODO: remove
		//Matt: just here to simulate a racer going around the track
		static int iCurrentTrackPieceIndex = 7;
		static bool wasPressed = false;
		bool isPressed = HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_SPACE);
		if (isPressed && !wasPressed)
		{
			++iCurrentTrackPieceIndex;
			if (iCurrentTrackPieceIndex >= m_vTrackPieces.size())
			{
				iCurrentTrackPieceIndex = 0;
			}

			HeatStroke::Event* pEvent = new HeatStroke::Event("RacerTrackPieceUpdated");
			pEvent->SetStringParameter("racerId", m_vRacers[0].id);
			pEvent->SetStringParameter("TrackPieceId", m_vTrackPieces[iCurrentTrackPieceIndex]->GetGUID());
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
		}
		wasPressed = isPressed;
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

		//TEMP
		//Matt: just using to test
		printf("current lap: %i\n", m_vRacers[iRacerIndex].currentLap);
		printf("lap progress: %i/%i\n", m_vRacers[iRacerIndex].currentTrackPiece, m_vTrackPieces.size() - 1);
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
}