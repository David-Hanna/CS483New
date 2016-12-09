//------------------------------------------------------------------------
// ComponentTrack
//
// Author:	Matthew White
//	
// Serves as the core for all track data
//------------------------------------------------------------------------

#ifndef COMPONENT_TRACK_H
#define COMPONENT_TRACK_H

#include "ComponentKartController.h"
#include "ComponentRacer.h"
#include "ComponentSimplePhysics.h"
#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"

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

			void RegisterRacer(HeatStroke::GameObject* p_pRacer) { RegisterRacer(dynamic_cast<ComponentRacer*>(p_pRacer->GetComponent("GOC_Racer"))); }
			void RegisterRacer(ComponentRacer* p_pRacer);

			void OnRacerTrackPieceCollision(const HeatStroke::Event* p_pEvent);
			void RegisterForTrackHeight(const HeatStroke::Event* p_pEvent);

			const std::string& GetTrackName() const { return m_strTrackName; }

		protected:
			ComponentTrack(HeatStroke::GameObject* p_pGameObject, const std::string& p_strTrackName);

		private:
			std::string m_strTrackName;
			std::vector<HeatStroke::GameObject*> m_vTrackPieces;
			//NOTE: components are stored instead of objects, as it's faster to access objects from components than components from objects
			std::vector<ComponentRacer*> m_vRacers;
			std::map<std::string, ComponentSimplePhysics*> m_vPhysicsObjects;
			std::function<void(const HeatStroke::Event*)>* m_pRacerTrackPieceUpdatedDelegate;
			std::function<void(const HeatStroke::Event*)>* m_pRegisterDelegate;

			int GetTrackPieceIndex(const std::string& p_strTrackPieceId);
			int GetNextTrackPieceIndex(int p_iCurrentTrackPieceIndex);
			int GetRacerIndex(const std::string& p_strRacerId);

			void UpdateRacerPositions();
			void CheckRacerFacingForward();
			bool IsAhead(ComponentRacer* p_RacerA, ComponentRacer* p_RacerB);

			void TriggerRacerPositionUpdateEvent(const std::string& p_strRacerId);
			void TriggerRacerCompletedLapEvent(const std::string& p_strRacerId);
			void TriggerRacerFinishedRaceEvent(const std::string& p_strRacerId);
			void TriggerRaceStandingsUpdateEvent();

			glm::vec3 DetermineTrackForwardDirection(int p_iTrackPieceIndex);
			glm::vec3 DetermineRacerForwardDirection(const std::string& p_strRacerId);

			float m_fRaceTime;

	};
}

#endif
