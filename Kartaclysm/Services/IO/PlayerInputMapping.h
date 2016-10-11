//====================================================================================
// PlayerInputMapping
// Author: Bradley Cooper
//
// Map players to an input that controls their racer.
//====================================================================================

#ifndef PLAYER_INPUT_MAPPING_H
#define PLAYER_INPUT_MAPPING_H

#include <GLFW/glfw3.h>
#include <map>
#include "InputActionMapping.h"

namespace Kartaclysm
{
	class PlayerInputMapping
	{
	private:
		// Convenient typedefs
		typedef std::map<int, int> PlayerMap;

	public:
		//-------------------------------------------------------------
		// Public methods
		//-------------------------------------------------------------
		// Singleton methods
		static void CreateInstance();
		static void DestroyInstance();
		static PlayerInputMapping* Instance();

		// Sets number of players to track when race mode is enabled
		bool SetSplitscreenPlayers(const int p_iNumPlayers);

		// Provide external control for mapping
		bool ManuallyMapPlayer(const int p_iPlayer, const int p_iGLFWJoystick);
		int GetFirstAvailableInput();

		int GetSplitscreenPlayers() const	{ return m_iPlayersRacing; }
		int GetPlayersConnected() const		{ return m_iPlayersConnected; }

		// When race mode is enabled, sends an event when a player's gamepad disconnects
		void EnableRaceMode()	{ m_bRaceMode = true; }
		void DisableRaceMode()	{ m_bRaceMode = false; }

		// Game Loop Method
		void Update(const float p_fDelta);

	private:
		//-------------------------------------------------------------
		// Private members
		//-------------------------------------------------------------
		// Static singleton instance
		static PlayerInputMapping* s_pPlayerInputMappingInstance;

		// Player map (to input)
		PlayerMap* m_pPlayers;

		// Number of players racing
		int m_iPlayersRacing;

		// Number of joysticks connected + 1 (keyboard)
		int m_iPlayersConnected;

		// Toggle race mode
		bool m_bRaceMode;

		//-------------------------------------------------------------
		// Private methods
		//-------------------------------------------------------------
		PlayerInputMapping();
		PlayerInputMapping(const PlayerInputMapping&) = delete;
		PlayerInputMapping& operator=(const PlayerInputMapping&) = delete;
		~PlayerInputMapping();

		void Init();

		// Joystick connect/disconnect callback
		void JoystickCallback(const int p_iGLFWJoystick, const int p_iJoystickEvent);
	};
}

#endif // PLAYER_INPUT_MAPPING_H