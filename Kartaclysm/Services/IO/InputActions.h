//====================================================================================
// InputActions
// Author: Bradley Cooper
//
// Provides enums for racer actions and input types, and defines XBox controls.
//====================================================================================

#ifndef INPUT_ACTIONS_H
#define INPUT_ACTIONS_H

// TO DO, test if these are accurate
#define XBOX_A 0
#define XBOX_B 1
#define XBOX_X 2
#define XBOX_Y 3
#define XBOX_RT 4 // right trigger
#define XBOX_LT 5 // left trigger
#define XBOX_RB 6 // right bumper
#define XBOX_LB 7 // left bumper
#define XBOX_UP 8
#define XBOX_DOWN 9
#define XBOX_LEFT 10
#define XBOX_RIGHT 11
#define XBOX_START 12
#define XBOX_BACK 13
#define XBOX_L3 14 // not the right name, but I'm not putting LEFT_ANALOG_DOWN
#define XBOX_R3 15 // ditto

#define XBOX_LEFT_ANALOG_X 0
#define XBOX_LEFT_ANALOG_Y 1
#define XBOX_RIGHT_ANALOG_X 2
#define XBOX_RIGHT_ANALOG_Y 3

namespace Kartaclysm
{
	namespace Racer
	{
		enum Action
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
			eAnalogStick // Analog stick movement, not its press-down button
		};
	}

	namespace Input
	{
		enum Type
		{
			eKeyboard = 0,
			eJoystick
		};
	}
}

#endif // INPUT_ACTIONS_H