//------------------------------------------------------------------------
// StateMachine
// Author:	David Hanna
//	
// State machine that manages state transitions.
//------------------------------------------------------------------------

#include "StateMachine.h"

//------------------------------------------------------------------------------
// Method:    StateMachine
// 
// Constructor
//------------------------------------------------------------------------------
HeatStroke::StateMachine::StateMachine()
	:
	m_mCurrentState(-1, nullptr),
	m_fCurrentStateTime(0.0f),
	m_pOwner(nullptr)
{
}

//------------------------------------------------------------------------------
// Method:    ~StateMachine
// 
// Destructor. Calls Exit() on stack and deletes all registered states.
//------------------------------------------------------------------------------
HeatStroke::StateMachine::~StateMachine()
{
	// Call Exit() on every state in stack
	while (!m_mStateStack.empty() && m_mCurrentState.second != nullptr)
	{
		Pop();
	}

	// Delete all registered states
	StateMap::iterator it = m_mStateMap.begin();
	while (it != m_mStateMap.end())
	{
		if (it->second != nullptr)
		{
			delete it->second;
			it->second = nullptr;
		}

		it = m_mStateMap.erase(it);
	}
}

//------------------------------------------------------------------------------
// Method:		RegisterState
// Parameter:	int p_iState - index to map state
//				State* p_pState - state to register to index
// 
// Register a state to an index. Ownership of the state now belongs to this state machine.
//------------------------------------------------------------------------------
void HeatStroke::StateMachine::RegisterState(int p_iState, State* p_pState)
{
	p_pState->SetStateMachineMembership(this);
	m_mStateMap.insert(StateMap::value_type(p_iState, p_pState));
}

//------------------------------------------------------------------------------
// Method:		Push
// Parameter:	int p_iState - integer of state to push
//				const ContextParameters& p_mContextParameters - Context parameters for State
// 
// Pushes state mapped to index on top of stack, and calls Enter() with context parameters
// Calls Suspend() on current state on top of the stack
// Asserts that the same state is not on the stack twice
//------------------------------------------------------------------------------
void HeatStroke::StateMachine::Push(int p_iState, const ContextParameters& p_mContextParameters)
{
	// Prevent a state from being on the stack twice
	StateStack::iterator it = m_mStateStack.begin(), end = m_mStateStack.end();
	for (; it != end; it++)
	{
		assert(it->first != p_iState);
	}

	// Find state mapped to index
	State* pState = nullptr;
	StateMap::iterator it2 = m_mStateMap.find(p_iState);
	if (it2 != m_mStateMap.end())
	{
		pState = static_cast<State*>(it2->second);
	}
	assert(pState != nullptr);

	// Call Suspend() on current state if it exists
	if (m_mCurrentState.second != nullptr)
	{
		m_mCurrentState.second->Suspend(it2->first);
	}

	// Push and enter new state
	m_mStateStack.push_back(std::pair<int, State*>(p_iState, pState));
	pState->Enter(p_mContextParameters);

	// Set the new current state info
	m_mCurrentState.first = p_iState;
	m_mCurrentState.second = pState;
	m_fCurrentStateTime = 0.0f;
}

//------------------------------------------------------------------------------
// Method:	Pop
// Returns:	StatePair
// 
// Calls Exit() on state, pops it off to return, and unsuspends next state on stack
//------------------------------------------------------------------------------
HeatStroke::StateMachine::StatePair HeatStroke::StateMachine::Pop()
{
	StatePair mReturn = m_mCurrentState;
	if (!m_mStateStack.empty())
	{
		// Exit and pop current state
		assert(m_mCurrentState.second != nullptr);
		m_mCurrentState.second->Exit();
		m_mStateStack.pop_back();

		// Set the new current state info and call Unsuspend() on new state if it exists
		m_fCurrentStateTime = 0.0f;
		if (m_mStateStack.empty())
		{
			m_mCurrentState.first = -1;
			m_mCurrentState.second = nullptr;
		}
		else
		{
			m_mCurrentState.first = m_mStateStack.back().first;
			m_mCurrentState.second = static_cast<State*>(m_mStateStack.back().second);
			m_mCurrentState.second->Unsuspend(mReturn.first);
		}
	}

	return mReturn;
}

//------------------------------------------------------------------------------
// Method:		Update
// Parameter:	const float p_fDelta
//				const bool m_bUpdateStack - update entire stack (true) or current state (false)
// 
// Update either the entire stack or just the top state on the stack
//------------------------------------------------------------------------------
void HeatStroke::StateMachine::Update(const float p_fDelta, const bool m_bUpdateStack)
{
	// TO DO, should each state have an independent tracker for how long it has been in the stack?
	m_fCurrentStateTime += p_fDelta;
	if (m_bUpdateStack)
	{
		// Update entire stack from bottom to top
		StateStack::iterator it = m_mStateStack.begin(), end = m_mStateStack.end();
		for (; it != end; it++)
		{
			it->second->Update(p_fDelta);
		}
	}
	else
	{
		// Update only the top state
		if (m_mCurrentState.second != nullptr)
		{
			m_mCurrentState.second->Update(p_fDelta);
		}
	}
}