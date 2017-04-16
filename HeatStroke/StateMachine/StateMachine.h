//------------------------------------------------------------------------
// StateMachine
// Author:	David Hanna
//	
// State machine that manages state transitions.
//------------------------------------------------------------------------

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <map>
#include <vector>
#include "State.h"

#include <assert.h>

namespace HeatStroke
{
	class StateMachine
	{
	public:
		// convenient typedefs
		typedef std::map<int, State*> StateMap;
		typedef std::pair<int, State*> StatePair;
		typedef std::vector<StatePair> StateStack;
		typedef std::map<std::string, std::string> ContextParameters;

		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		StateMachine();
		~StateMachine();

		void RegisterState(int p_iState, State* p_pstate);

		// Stack-based mechanics
		void Push(int p_iState, const ContextParameters& p_mContextParameters = ContextParameters());
		StatePair Pop();
		const StatePair& Peek() const				{ return m_mCurrentState; }
		StateStack::const_iterator begin()			{ return m_mStateStack.begin(); }
		StateStack::const_iterator end()			{ return m_mStateStack.end(); }
		bool empty() const							{ return m_mStateStack.empty(); }
		size_t size() const							{ return m_mStateStack.size(); }

		void Update(const float p_fDelta, const bool m_bUpdateStack = false);

		float GetCurrentStateTime()					{ return m_fCurrentStateTime; }

		// A way for states in a state machine to reference back to their owner
		void SetStateMachineOwner(void *p_pOwner)	{ m_pOwner = p_pOwner; }
		void* GetStateMachineOwner()				{ return m_pOwner; }

	private:
		// Map of state Ids to state instances
		StateMap m_mStateMap;

		// Stack of current states
		StateStack m_mStateStack;

		// Current state
		StatePair m_mCurrentState;

		// State timer
		float m_fCurrentStateTime;

		// State machine owner
		void* m_pOwner;
	};
} // namespace HeatStroke

#endif // STATE_MACHINE_H

