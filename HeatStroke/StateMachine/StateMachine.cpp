//------------------------------------------------------------------------
// StateMachine
// Author:	David Hanna
//	
// State machine that manages state transitions.
//------------------------------------------------------------------------

#include "StateMachine.h"

HeatStroke::StateMachine::StateMachine()
	:
	m_iCurrentState(-1),
	m_pCurrentState(nullptr),
	m_fCurrentStateTime(0.0f),
	m_pOwner(nullptr)
{
}


HeatStroke::StateMachine::~StateMachine()
{
}


void HeatStroke::StateMachine::RegisterState(int p_iState, State* p_pState)
{
	p_pState->SetStateMachineMembership(this);
	m_mStateMap.insert(std::pair<int, State*>(p_iState, p_pState));
}


void HeatStroke::StateMachine::GoToState(int p_iState, const std::map<std::string, std::string>& p_mContextParameters)
{
	// Prevent double entering current state
	if (p_iState == m_iCurrentState)
	{
		return;
	}

	State* pState = nullptr;
	StateMap::iterator it = m_mStateMap.find(p_iState);
	if (it != m_mStateMap.end())
	{
		pState = static_cast<State*>(it->second);
	}
	assert(pState != nullptr);

	// Call exit on current state
	if (m_pCurrentState)
	{
		m_pCurrentState->Exit();
	}

	// Call enter on the new state
	pState->Enter(p_mContextParameters);

	// Set the new current state.
	m_iCurrentState = p_iState;
	m_pCurrentState = pState;
	m_fCurrentStateTime = 0.0f;
}

void HeatStroke::StateMachine::Update(float p_fDelta)
{
	m_fCurrentStateTime += p_fDelta;
	if (m_pCurrentState)
	{
		m_pCurrentState->Update(p_fDelta);
	}
}