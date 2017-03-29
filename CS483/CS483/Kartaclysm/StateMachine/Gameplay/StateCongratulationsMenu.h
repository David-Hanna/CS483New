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
		bool m_bSkipToMainMenu;

		struct Medal {
			HeatStroke::ComponentSprite*	m_pSprite;
			HeatStroke::TextBox*			m_pText;

			Medal() : m_pSprite(nullptr), m_pText(nullptr) {}
		};
		std::vector<Medal> m_vMedals;
	};
}

#endif

