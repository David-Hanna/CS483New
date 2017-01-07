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
#include <functional>
#include "InputActionMapping.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class PlayerInputMapping
	{
	public:
		//-------------------------------------------------------------
		// Public methods
		//-------------------------------------------------------------
		// Singleton methods
		static void CreateInstance();
		static void DestroyInstance();
		static PlayerInputMapping* Instance();

		// Manually query a player's movement actions
		void QueryPlayerMovement(
			const int p_iPlayerNum,
			int& p_iAccelerate,
			int& p_iBrake,
			int& p_iSlide,
			float& p_fTurn) const;

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
		// Private types
		//-------------------------------------------------------------
		typedef std::map<int, int> PlayerMap;

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

		// Delegate function to register with EventManager for joystick connects and disconnects
		std::function<void(const HeatStroke::Event*)>* m_pJoystickDelegate;

		//-------------------------------------------------------------
		// Private methods
		//-------------------------------------------------------------
		PlayerInputMapping();
		PlayerInputMapping(const PlayerInputMapping&) = delete;
		PlayerInputMapping& operator=(const PlayerInputMapping&) = delete;
		~PlayerInputMapping();

		void Init();

		// Send event for HUD and other listeners
		void SendInputAssignmentEvent(const int p_iPlayer);
		std::string GetButtonString(Input::Type eType, const int p_iButton) const;

		// Joystick connect/disconnect callback
		void JoystickCallback(const HeatStroke::Event* p_pEvent);
	};
}

#endif // PLAYER_INPUT_MAPPING_H