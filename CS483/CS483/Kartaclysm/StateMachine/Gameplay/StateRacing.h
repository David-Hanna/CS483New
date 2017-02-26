//------------------------------------------------------------------------
// StateRacing
// Author:	Bradley Cooper
//	
// Gameplay state for racing.
//------------------------------------------------------------------------

#ifndef STATE_RACING_H
#define STATE_RACING_H

#include <functional>

#include "Component3DModel.h"
#include "ComponentAmbientLight.h"
#include "ComponentDirectionalLight.h"
#include "ComponentPointLight.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentCameraController.h"
#include "ComponentKartController.h"
#include "ComponentTrack.h"
#include "ComponentTrackPiece.h"
#include "ComponentSprite.h"
#include "ComponentTextBox.h"
#include "ComponentOrthographicCamera.h"
#include "ComponentKartStats.h"
#include "ComponentDriverStatMods.h"
#include "ComponentAbilityConditions.h"
#include "ComponentAbilityIcon.h"
#include "ComponentBoostAbility.h"
#include "ComponentWheelieAbility.h"
#include "ComponentArmorPlateAbility.h"
#include "ComponentMaintainAbility.h"
#include "ComponentStrikeAbility.h"
#include "ComponentTurkeyAbility.h"
#include "ComponentClockAbility.h"
#include "ComponentTinkerAbility.h"
#include "ComponentBedazzleAbility.h"
#include "ComponentRainAbility.h"
#include "ComponentProjectile.h"
#include "ComponentSelfDestruct.h"
#include "ComponentSimplePhysics.h"
#include "ComponentSphereCollider.h"
#include "ComponentWallCollider.h"
#include "ComponentHudAbility.h"
#include "ComponentHudRaceTimer.h"
#include "ComponentHudPosition.h"
#include "ComponentHudLapCount.h"
#include "ComponentHudCountdown.h"
#include "ComponentHudFps.h"
#include "ComponentHudPopup.h"

#include "Common.h"
#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "AudioPlayer.h"

#include "ComponentRacer.h"

namespace Kartaclysm
{
	class StateRacing : public Kartaclysm::GameplayState
	{
	public:
		StateRacing();
		virtual ~StateRacing();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState);
		void Unsuspend(const int p_iPrevState);
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		struct RacerResults
		{
			ComponentRacer* m_pRacerComponent;
			std::vector<float> m_vLapTimes;
			float m_fRaceTime;
			unsigned int m_uiPosition;
		};
		std::map<std::string, RacerResults> m_mRaceResults;

		std::function<void(const HeatStroke::Event*)>* m_pPauseDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRacerFinishedLapDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRacerFinishedRaceDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRaceFinishedDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRaceRestartDelegate;

		std::map<std::string, std::string> m_mContextParams;
		unsigned int m_uiNumRacers;
		bool m_bCountdown;

		void CreateHUDForRacer(const std::string& p_strGuid);

		void SendRaceInfoEvent();
		void BeginRace();
		HeatStroke::GameObject* GenerateRacer(
			const std::string& p_strKartDefinitionFile, 
			const std::string& p_strDriverDefinitionFile, 
			const std::string& p_strCameraDefinitionFile, 
			const std::string& p_strGuid = ""
		);

		void PauseGame(const HeatStroke::Event* p_pEvent);
		void RacerFinishedLap(const HeatStroke::Event* p_pEvent);
		void RacerFinishedRace(const HeatStroke::Event* p_pEvent);
		void FinishRace(const HeatStroke::Event* p_pEvent);
		void RestartRace(const HeatStroke::Event* p_pEvent) { BeginRace(); }

		std::map<std::string, std::string> GenerateRaceResults() const;
		void GetDriverNameAndKartName(ComponentRacer* p_pRacerComponent, std::string& p_strDriver, std::string& p_strKart) const;
	};
}

#endif

