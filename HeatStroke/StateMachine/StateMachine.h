//------------------------------------------------------------------------
// StateMachine
// Author:	David Hanna
//	
// State machine that manages state transitions.
//------------------------------------------------------------------------

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <map>
#include <vector>
#include "State.h"

#include <assert.h>

namespace HeatStroke
{
	class StateMachine
	{
		typedef std::map<int, State*> StateMap;
		typedef std::vector<std::pair<int, State*>> StateStack;
		typedef const std::map<std::string, std::string>& ContextParameters;

	public:
		StateMachine(void);
		~StateMachine(void);

		void RegisterState(int p_iState, State* p_pstate);
		void Push(int p_iState, ContextParameters p_mContextParameters);
		void Pop();
		void PopAndPush(int p_iState, ContextParameters p_mContextParameters);
		void PopToTop();
		void PopToTopAndPush(int p_iState, ContextParameters p_mContextParameters);
		void PopAllAndPush(int p_iState, ContextParameters p_mContextParameters);

		void Update(const float p_fDelta);

		int GetCurrentState()					{ return m_iCurrentState; }
		float GetCurrentStateTime()				{ return m_fCurrentStateTime; }
		StateStack::const_iterator StackBegin()	{ return m_mStateStack.begin(); }
		StateStack::const_iterator StackEnd()	{ return m_mStateStack.end(); }

		// A way for states in a state machine to reference back to their owner
		void SetStateMachineOwner(void *p_pOwner)	{ m_pOwner = p_pOwner; }
		void* GetStateMachineOwner()				{ return m_pOwner; }

	private:
		// Map of state Ids to state instances
		StateMap m_mStateMap;

		// Stack of current states
		StateStack m_mStateStack;

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

