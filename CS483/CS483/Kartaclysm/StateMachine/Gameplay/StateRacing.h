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
#include "ComponentPerspectiveCamera.h"
#include "ComponentCameraController.h"
#include "ComponentKartController.h"
#include "ComponentSprite.h"
#include "ComponentOrthographicCamera.h"
#include "ComponentAbilityConditions.h"
#include "ComponentSampleAbility.h"
#include "ComponentHudAbility.h"

#include "Common.h"
#include "GameplayState.h"
#include "EventManager.h"
#include "LineDrawer.h"

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

		HeatStroke::LineDrawer *lineDrawer;

		// Inherited
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pPauseDelegate;

		void LoadLevel(const std::string& p_strLevelPath);
		void PauseGame(const HeatStroke::Event* p_pEvent);
	};
} // namespace Kartaclysm

#endif

