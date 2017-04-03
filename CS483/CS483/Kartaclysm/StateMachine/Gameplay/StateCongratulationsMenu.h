//------------------------------------------------------------------------
// StateCongratulationsMenu
// Author:	Bradley Cooper
//	
// Shows a congratulations message after a tournament.
// Continues to main menu.
//------------------------------------------------------------------------

#ifndef STATE_CONGRATULATIONS_MENU_H
#define STATE_CONGRATULATIONS_MENU_H

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentTextbox.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "EasyXML.h"
#include "AudioPlayer.h"
#include "TimeFormat.h"

namespace Kartaclysm
{
	class StateCongratulationsMenu : public Kartaclysm::GameplayState
	{
	public:
		StateCongratulationsMenu();
		virtual ~StateCongratulationsMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState) { m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState) { m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;

	private:
		struct Medalist {
			int m_iPosition;
			std::string m_strRacerId;

			Medalist(int p_iPosition, const std::string& p_strRacerId) :
				m_iPosition(p_iPosition), m_strRacerId(p_strRacerId) {}
		};

		void DetermineHumanRacersEarningMedals(const std::map<std::string, std::string>& p_mRaceResults, std::vector<Medalist>* p_pHumanMedalists) const;
		unsigned int CreateMedalsForWinners(const std::vector<Medalist>& p_vHumanMedalists);
		void SetTextBeneathMedal(HeatStroke::GameObject* p_pMedal, const std::string& p_strText) const;
		void PositionMedalRenderables(const std::vector<HeatStroke::GameObject*>& p_vMedalGOs);
	};
}

#endif

