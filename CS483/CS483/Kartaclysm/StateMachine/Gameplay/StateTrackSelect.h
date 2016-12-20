//------------------------------------------------------------------------
// StateTrackSelect
// Author:	Matthew White
//	
// Menu state for selecting the track
//------------------------------------------------------------------------

#ifndef STATE_TRACK_SELECT_H
#define STATE_TRACK_SELECT_H

#include "GameplayState.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class StateTrackSelect : public Kartaclysm::GameplayState
	{
		public:
			StateTrackSelect();
			virtual ~StateTrackSelect();

			void Enter(const std::map<std::string, std::string>& p_mContextParameters);
			void Suspend(const int p_iNewState);
			void Unsuspend(const int p_iPrevState);
			void Update(const float p_fDelta);
			void PreRender();
			void Exit();

		protected:
			HeatStroke::GameObjectManager* m_pGameObjectManager;
			bool m_bSuspended;

			std::string m_strTrackDefinitionFile;
	};
}

#endif

