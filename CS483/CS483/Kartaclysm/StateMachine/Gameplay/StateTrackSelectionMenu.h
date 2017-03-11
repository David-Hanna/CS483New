//------------------------------------------------------------------------
// StateTrackSelectionMenu
// Author:	David Hanna
//	
// Allows player one to select a track to race on.
// Continues to racing state.
//------------------------------------------------------------------------

#ifndef STATE_TRACK_SELECTION_MENU_H
#define STATE_TRACK_SELECTION_MENU_H

#include "ComponentOrthographicCamera.h"
#include "ComponentSprite.h"
#include "ComponentPerspectiveCamera.h"
#include "ComponentTextbox.h"

#include "GameplayState.h"
#include "EventManager.h"
#include "PlayerInputMapping.h"
#include "AudioPlayer.h"

namespace Kartaclysm
{
	class StateTrackSelectionMenu : public Kartaclysm::GameplayState
	{
	public:
		StateTrackSelectionMenu();
		virtual ~StateTrackSelectionMenu();

		void Enter(const std::map<std::string, std::string>& p_mContextParameters);
		void Suspend(const int p_iNewState)			{ m_bSuspended = true; }
		void Unsuspend(const int p_iPrevState)		{ m_bSuspended = false; }
		void Update(const float p_fDelta);
		void PreRender();
		void Exit();

	protected:
		struct Times {
			// TODO: Expand to include lap times
			std::string m_strLocalRace;
			std::string m_strGlobalRace;

			Times() : m_strLocalRace("--:--.--"), m_strGlobalRace("--:--.--") {}
		};
		std::map<std::string, Times> m_mTrackTimes;

		HeatStroke::GameObjectManager* m_pGameObjectManager;
		bool m_bSuspended;
		int m_iTrackSelection;
		HeatStroke::GameObject* m_pCurrentHighlight;
		std::map<std::string, std::string> m_mContextParameters;
		std::function<void(const HeatStroke::Event*)>* m_pTrackTimeDelegate;

		virtual void LoadLocalTrackTimesFromXml(const std::string& p_strFileName, const std::vector<HeatStroke::GameObject*>& p_vTracks);
		virtual void FillRaceTimeTextboxes(const std::vector<HeatStroke::GameObject*>& p_vTracks);
		virtual void TrackTimeCallback(const HeatStroke::Event* p_pEvent);

		// Another repeat of the same function from StateRaceComplete
		std::string FormatTime(const std::string& p_strUnformattedTime) const;
	};
}

#endif

