//------------------------------------------------------------------------
// ComponentTrack
//
// Author:	Matthew White
//	
// Serves as the core for all track data
//------------------------------------------------------------------------

#ifndef COMPONENT_TRACK_H
#define COMPONENT_TRACK_H

#include "Component.h"
#include "GameObject.h"
#include "Event.h"

namespace Kartaclysm
{
	class ComponentTrack : public HeatStroke::Component
	{
		public:
			~ComponentTrack();

			virtual const std::string FamilyID() const override { return "GOC_Track"; }
			virtual const std::string ComponentID() const override { return "GOC_Track"; }

			static HeatStroke::Component* CreateComponent(
				HeatStroke::GameObject* p_pGameObject,
				tinyxml2::XMLNode* p_pBaseNode,
				tinyxml2::XMLNode* p_pOverrideNode);

			virtual void Init() override;
			virtual void Update(const float p_fDelta) override;

			void RacerTrackPieceUpdatedDelegate(const HeatStroke::Event* p_pEvent);

			const std::string& GetTrackName() const { return m_strTrackName; }

		protected:
			ComponentTrack(HeatStroke::GameObject* p_pGameObject, const std::string& p_strTrackName);

		private:
			//TODO: remove this
			//Matt: using this to simulate a racer going around the track
			struct RacerMock
			{
				std::string id;
				int currentTrackPiece;
				int currentLap;
			};

			std::string m_strTrackName;
			std::vector<HeatStroke::GameObject*> m_vTrackPieces;
			std::vector<RacerMock> m_vRacers;

			int GetTrackPieceIndex(const std::string& p_strTrackPieceId);
			int GetRacerIndex(const std::string& p_strRacerId);

			void UpdateRacerPosition(int p_iRacerIndex);
			void TriggerRacerPositionUpdateEvent(const std::string& p_strRacerId);

	};
}

#endif