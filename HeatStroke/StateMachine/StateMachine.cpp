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
	// Call exit on every state in stack
	PopToTop();
	Pop();
}


void HeatStroke::StateMachine::RegisterState(int p_iState, State* p_pState)
{
	p_pState->SetStateMachineMembership(this);
	m_mStateMap.insert(std::pair<int, State*>(p_iState, p_pState));
}


void HeatStroke::StateMachine::Push(int p_iState, ContextParameters p_mContextParameters)
{
	// Prevent double entering current state
	if (p_iState == m_iCurrentState)
	{
		return;
	}

	// Find state mapped to index
	State* pState = nullptr;
	StateMap::iterator it = m_mStateMap.find(p_iState);
	if (it != m_mStateMap.end())
	{
		pState = static_cast<State*>(it->second);
	}
	assert(pState != nullptr);

	// Call Suspend() on current state if it exists
	if (m_pCurrentState != nullptr)
	{
		m_pCurrentState->Suspend();
	}

	// Push and enter new state
	m_mStateStack.push_back(std::pair<int, State*>(p_iState, pState));
	pState->Enter(p_mContextParameters);

	// Set the new current state info
	m_iCurrentState = p_iState;
	m_pCurrentState = pState;
	m_fCurrentStateTime = 0.0f;
}

void HeatStroke::StateMachine::Pop()
{
	if (!m_mStateStack.empty())
	{
		// Exit and pop current state
		m_pCurrentState->Exit();
		m_mStateStack.pop_back();

		// Set the new current state info and call Unsuspend() on new state if it exists
		m_fCurrentStateTime = 0.0f;
		if (m_mStateStack.empty())
		{
			m_iCurrentState = -1;
			m_pCurrentState = nullptr;
		}
		else
		{
			m_iCurrentState = m_mStateStack.back().first;
			m_pCurrentState = static_cast<State*>(m_mStateStack.back().second);
			m_pCurrentState->Unsuspend();
		}
	}
}


void HeatStroke::StateMachine::PopAndPush(int p_iState, ContextParameters p_mContextParameters)
{
	Pop();
	Push(p_iState, p_mContextParameters);
}


void HeatStroke::StateMachine::PopToTop()
{
	while (m_mStateStack.size() > 1)
	{
		Pop();
	}
}


void HeatStroke::StateMachine::PopToTopAndPush(int p_iState, ContextParameters p_mContextParameters)
{
	PopToTop();
	Push(p_iState, p_mContextParameters);
}


void HeatStroke::StateMachine::PopAllAndPush(int p_iState, ContextParameters p_mContextParameters)
{
	PopToTop();
	PopAndPush(p_iState, p_mContextParameters);
}


void HeatStroke::StateMachine::Update(float p_fDelta)
{
	m_fCurrentStateTime += p_fDelta;
	if (m_pCurrentState)
	{
		m_pCurrentState->Update(p_fDelta);
	}
}