//====================================================================================
// InputActionMapping
// Author: Bradley Cooper
//
// Map inputs to an action for a player.
//====================================================================================

#ifndef INPUT_ACTION_MAPPING_H
#define INPUT_ACTION_MAPPING_H

#include <GLFW/glfw3.h>
#include <map>
#include "KeyboardInputBuffer.h"
#include "JoystickInputBuffer.h"

namespace Kartaclysm
{
	class InputActionMapping
	{
	private:
		enum RacerAction
		{
			eAccelerate = 0,
			eBrake,
			eLeft,
			eRight,
			eSlide,
			eDriverAbility1,
			eDriverAbility2,
			eKartAbility1,
			eKartAbility2,
			ePause,
			eJoystick
		};

		// Convenient typedefs
		typedef std::map<int, int> PlayerMap;
		typedef std::map<RacerAction, int> InputMap;

	public:
		//-------------------------------------------------------------
		// Public methods
		//-------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(const std::string& p_strUserConfigFilePath);
		static void DestroyInstance();
		static InputActionMapping* Instance();

		// Game Loop Method
		void Update(const float p_fDelta);

	private:
		//-------------------------------------------------------------
		// Private members
		//-------------------------------------------------------------
		// Static singleton instance
		static InputActionMapping* s_pInputActionMappingInstance;

		// Player map (to input)
		PlayerMap* m_pPlayers;

		// Input maps
		// TO DO, all joysticks currently share one control binding
		InputMap* m_pKeyboardMap;
		InputMap* m_pJoystickMap;

		// Number of joysticks connected
		int m_iJoysticks;

		//-------------------------------------------------------------
		// Private methods
		//-------------------------------------------------------------
		InputActionMapping(const std::string& p_strUserConfigFilePath);
		InputActionMapping(const InputActionMapping&) = delete;
		InputActionMapping& operator=(const InputActionMapping&) = delete;
		~InputActionMapping();

		void Init(const std::string& p_strUserConfigFilePath);

		// Load user control bindings, setting missing controls to default
		void LoadUserControlBindings(const std::string& p_strUserConfigFilePath);

		// Joystick connect/disconnect callback
		void JoystickCallback(const int p_iGLFWJoystick, const int p_iJoystickEvent);
	};
}

#endif // INPUT_ACTION_MAPPING_H