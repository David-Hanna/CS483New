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
#include "ComponentAbilityConditions.h"
#include "ComponentAbilityIcon.h"
#include "ComponentBoostAbility.h"
#include "ComponentWheelieAbility.h"
#include "ComponentArmorPlateAbility.h"
#include "ComponentMaintainAbility.h"
#include "ComponentStrikeAbility.h"
#include "ComponentTurkeyAbility.h"
#include "ComponentProjectile.h"
#include "ComponentSelfDestruct.h"
#include "ComponentSimplePhysics.h"
#include "ComponentSphereCollider.h"
#include "ComponentWallCollider.h"
#include "ComponentHudAbility.h"
#include "ComponentHudRaceTimer.h"
#include "ComponentHudPosition.h"
#include "ComponentHudLapCount.h"
#include "ComponentHudFps.h"
#include "ComponentHudPopup.h"

#include "Common.h"
#include "GameplayState.h"
#include "EventManager.h"

#include "ComponentRacer.h"

namespace Kartaclysm
{
	class StateRacing : public Kartaclysm::GameplayState
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		StateRacing();
		virtual ~StateRacing();

		// Inherited
		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState);
		void Unsuspend(const int p_iPrevState);
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		// Inherited
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pPauseDelegate;
		std::function<void(const HeatStroke::Event*)>* m_pRaceFinishedDelegate;

		//TEMP used to reset race state
		//		can be removed once we return to menus after race
		std::map<std::string, std::string> m_mContextParams;

		HeatStroke::GameObject* GenerateRacer(
			const std::string& p_strKartDefinitionFile, 
			const std::string& p_strDriverDefinitionFile, 
			const std::string& p_strCameraDefinitionFile, 
			const std::string& p_strGuid = ""
		);

		void PauseGame(const HeatStroke::Event* p_pEvent);
		void FinishRace(const HeatStroke::Event* p_pEvent);
	};
} // namespace Kartaclysm

#endif

