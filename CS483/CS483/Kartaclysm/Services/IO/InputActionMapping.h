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
#include "InputActions.h"
#include "KeyboardInputBuffer.h"
#include "JoystickInputBuffer.h"

namespace Kartaclysm
{
	class InputActionMapping
	{
		// Allow direct access to query information
		friend class PlayerInputMapping;

	public:
		//-------------------------------------------------------------
		// Public methods
		//-------------------------------------------------------------
		// Singleton methods
		static void CreateInstance(const std::string& p_strUserConfigFilePath);
		static void DestroyInstance();
		static InputActionMapping* Instance();

		// Manage user control bindings
		void ResetUserControlBindings(const bool p_bResetKeyboard = true, const bool p_bResetJoystick = true);
		void LoadUserControlBindings();

		// TO DO, the below methods
		// bool SaveUserControlBindings();
		// bool EditUserControlBindings(Input::Type, Racer::Action, const int p_iGLFWInputKey);

	private:
		//-------------------------------------------------------------
		// Private types
		//-------------------------------------------------------------
		typedef std::map<Racer::Action, int> ActionMap;
		typedef std::map<Input::Type, ActionMap> InputMap;

		//-------------------------------------------------------------
		// Private members
		//-------------------------------------------------------------
		// Static singleton instance
		static InputActionMapping* s_pInputActionMappingInstance;

		// Input maps
		InputMap* m_pInputMap;

		// File location for user control bindings
		std::string m_strUserConfigFilePath;

		//-------------------------------------------------------------
		// Private methods
		//-------------------------------------------------------------

		void SendInputEventForPlayer(const int p_iPlayer, const int p_iGLFWJoystick);
		bool GetButton(const int p_iGLFWJoystick, const Racer::Action p_eAction);
		float GetTurning(const int p_iGLFWJoystick);

		InputActionMapping(const std::string& p_strUserConfigFilePath);
		InputActionMapping(const InputActionMapping&) = delete;
		InputActionMapping& operator=(const InputActionMapping&) = delete;
		~InputActionMapping();

		void Init();
	};
}

#endif // INPUT_ACTION_MAPPING_H