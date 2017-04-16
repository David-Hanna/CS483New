//------------------------------------------------------------------------
// StateMainMenu
// Author:	David Hanna
//	
// Main menu state shows title screen and continues with enter key.
//------------------------------------------------------------------------

#ifndef STATE_MAIN_MENU_H
#define STATE_MAIN_MENU_H

#include <thread>
#include <future>

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "AudioPlayer.h"
#include "KeyboardInputBuffer.h"
#include "PlayerInputMapping.h"
#include "ModelManager.h"
#include "SpriteManager.h"
#include "DatabaseManager.h"

namespace Kartaclysm
{
	class StateMainMenu : public Kartaclysm::GameplayState
	{
	public:
		StateMainMenu();
		virtual ~StateMainMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState);
		void Unsuspend(const int p_iPrevState);
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		HeatStroke::GameObjectManager* m_pGameObjectManager;
		std::set<Database::TrackPK> m_vTrackIds;
		bool m_bSuspended;
		bool m_bPreloadCalled;
		bool m_bRenderedOnce;
		bool m_bRunTrackTimeQuery;

		void SendTrackTimesEvent(const DatabaseManager::TrackTimes& p_mTrackTimes) const;
		std::future<DatabaseManager::TrackTimes> CreateTrackTimesThread() const;
		void EndTrackTimesThread(std::future<Kartaclysm::DatabaseManager::TrackTimes>& p_thrTrackTimeQuery) const;
	};
}

#endif

