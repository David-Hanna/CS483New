//====================================================================================
// InputActions
// Author: Bradley Cooper
//
// Provides enums for racer actions and input types, and defines XBox controls.
//====================================================================================

#ifndef INPUT_ACTIONS_H
#define INPUT_ACTIONS_H

#define XBOX_A 0
#define XBOX_B 1
#define XBOX_X 2
#define XBOX_Y 3
#define XBOX_LB 4
#define XBOX_RB 5
#define XBOX_BACK 6
#define XBOX_START 7
#define XBOX_L3 8
#define XBOX_R3 9
#define XBOX_UP 10
#define XBOX_RIGHT 11
#define XBOX_DOWN 12
#define XBOX_LEFT 13

// Right/up is positive, left/down is negative
#define XBOX_AXIS_DEAD_ZONE 0.1f
#define XBOX_AXIS_LEFT_X 0
#define XBOX_AXIS_LEFT_Y 1
#define XBOX_AXIS_TRIGGERS 2
#define XBOX_AXIS_RIGHT_Y 3
#define XBOX_AXIS_RIGHT_X 4	

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
			eKeyboard1 = 0,
			eKeyboard2,
			eKeyboard3,
			eKeyboard4,
			eJoystick
		};
	}
}

#endif // INPUT_ACTIONS_H