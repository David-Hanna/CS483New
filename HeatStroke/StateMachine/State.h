//------------------------------------------------------------------------
// State
// Author:	David Hanna
//	
// Base class for any states driven by StateMachine.
//------------------------------------------------------------------------

#ifndef STATE_H
#define STATE_H

#include <map>

namespace HeatStroke
{
	class State
	{
		friend class StateMachine;

	public:
		State() : m_pStateMachine(nullptr) {}
		virtual ~State() {}
			
		// Required interface for all subclasses.
		virtual void Enter(const std::map<std::string, std::string>& p_mContextParameters) = 0;
		virtual void Update(const float p_fDelta) = 0;
		virtual void Exit() = 0;

	private:
		void SetStateMachineMembership(StateMachine* p_pStateMachine) { m_pStateMachine = p_pStateMachine; }

	protected:
		// State machine that this state is registered with
		StateMachine* m_pStateMachine;
	};
} // namespace HeatStroke

#endif

