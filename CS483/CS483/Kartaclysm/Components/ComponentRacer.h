//----------------------------------------------------------------------------
// ComponentRacer
// Author: Matthew White
//
// Component that handles racer logic
//----------------------------------------------------------------------------

#ifndef COMPONENT_RACER_H
#define COMPONENT_RACER_H

#include <tinyxml2.h>

#include "Component.h"
#include "ComponentKartController.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentRacer : public HeatStroke::Component
	{
	public:
		virtual const std::string FamilyID() const override {return "GOC_Racer";}
		virtual const std::string ComponentID() const override {return "GOC_Racer";}

		// Destruction.
		virtual ~ComponentRacer();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		HeatStroke::GameObject* GetKart() {return m_pKart;}
		void SetKart(HeatStroke::GameObject* p_pKart) { m_pKart = p_pKart; }
		HeatStroke::GameObject* GetDriver() {return m_pDriver;}
		void SetDriver(HeatStroke::GameObject* p_pDriver)  { m_pDriver = p_pDriver; }
		int GetCurrentTrackPiece() const { return m_iCurrentTrackPiece; }
		int GetCurrentTrackPieceForDistanceCheck() const { return m_iCurrentTrackPieceForDistanceCheck; }
		void SetCurrentTrackPiece(int p_iNewTrackPiece);
		int GetFurthestTrackPiece() const {return m_iFurthestTrackPiece;}
		void SetFurthestTrackPiece(int p_iNewTrackPiece) {m_iFurthestTrackPiece = p_iNewTrackPiece;}
		int GetCurrentLap() const {return m_iCurrentLap;}
		void SetCurrentLap(int p_iNewLap) {m_iCurrentLap = p_iNewLap;}
		int GetCurrentPosition() const {return m_iCurrentPosition;}
		void SetCurrentPosition(int p_iNewPosition) {m_iCurrentPosition = p_iNewPosition;}
		bool HasFinishedRace() const {return m_bHasFinishedRace;}
		void SetHumanPlayer(bool p_bHumanPlayer) { m_bHumanPlayer = p_bHumanPlayer; }
		bool IsHumanPlayer() const { return m_bHumanPlayer; }
		const std::vector<float> GetLapTimes() const { return m_vLapTimes; }

	protected:
		ComponentRacer(HeatStroke::GameObject* p_pGameObject);

		virtual void PositionCallback(const HeatStroke::Event* p_pEvent);

	private:
		HeatStroke::GameObject* m_pKart;
		HeatStroke::GameObject* m_pDriver;
		std::function<void(const HeatStroke::Event*)>* m_pLapCompleteDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRaceFinishedDelegate;

		int m_iCurrentTrackPiece;
		int m_iCurrentTrackPieceForDistanceCheck;
		int m_iFurthestTrackPiece;
		int m_iCurrentLap;
		int m_iCurrentPosition;
		bool m_bHasFinishedRace;
		bool m_bHumanPlayer;
		std::vector<float> m_vLapTimes;

		void FinishLap(const HeatStroke::Event* p_pEvent);
		void FinishRace(const HeatStroke::Event* p_pEvent);

		std::function<void(const HeatStroke::Event*)>* m_pStandingsUpdateDelegate;
	};
}

#endif // COMPONENT_RACER_H
