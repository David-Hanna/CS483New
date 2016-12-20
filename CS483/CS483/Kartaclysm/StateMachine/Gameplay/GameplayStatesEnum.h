//========================================================================
// GameplayStatesEnum
// Author: David Hanna
//
// An enumeration to replace the int values in the state machine.
//========================================================================

#ifndef GAMEPLAY_STATES_ENUM
#define GAMEPLAY_STATES_ENUM

namespace Kartaclysm
{
	enum GameplayStates
	{
		STATE_MAIN_MENU = 0,
		STATE_PLAYER_SELECTION_MENU,
		STATE_TRACK_SELECTION_MENU,
		STATE_RACING,
		STATE_PAUSED,
		STATE_RACE_COMPLETE_MENU
	};
}

#endif