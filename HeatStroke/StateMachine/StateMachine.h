//------------------------------------------------------------------------
// StateMachine
// Author:	David Hanna
//	
// State machine that manages state transitions.
//------------------------------------------------------------------------

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "State.h"

#include <assert.h>

namespace HeatStroke
{
	class StateMachine
	{
		typedef std::map<int, State*> StateMap;

	public:
		StateMachine(void);
		~StateMachine(void);

		void RegisterState(int p_iState, State* p_pstate);
		void GoToState(int p_iState, const std::map<std::string, std::string>& p_mContextParameters);

		void Update(float p_fDelta);

		int GetCurrentState()		{ return m_iCurrentState; }
		float GetCurrentStateTime() { return m_fCurrentStateTime; }

		// A way for states in a state machine to reference back to their owner, whether 
		// it's the Game class, a Character controller or an AI controller.
		void SetStateMachineOwner(void *p_pOwner)	{ m_pOwner = p_pOwner; }
		void* GetStateMachineOwner()				{ return m_pOwner; }

	private:
		// Map of state Ids to state instances
		StateMap m_mStateMap;

		// Current state
		int m_iCurrentState;
		State* m_pCurrentState;

		// State timer
		float m_fCurrentStateTime;

		// State machine owner
		void* m_pOwner;
	};
} // namespace HeatStroke

#endif // STATEMACHINE_H

