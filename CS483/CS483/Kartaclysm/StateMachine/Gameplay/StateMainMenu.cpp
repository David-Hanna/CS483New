//------------------------------------------------------------------------
// StateMainMenu
// Author:	Bradley Cooper
//	
// Gameplay state first viewed when the game loads.
//------------------------------------------------------------------------

#include "StateMainMenu.h"

//------------------------------------------------------------------------------
// Method:    StateMainMenu
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
Kartaclysm::StateMainMenu::StateMainMenu()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true)
{
}

//------------------------------------------------------------------------------
// Method:    ~StateMainMenu
// Returns:   
// 
// Destructor.
//------------------------------------------------------------------------------
Kartaclysm::StateMainMenu::~StateMainMenu()
{
	Exit();
}

//------------------------------------------------------------------------------
// Method:		Enter
// Parameter:	std::map<std::string, std::string> p_mContextParameters - parameters for state
// 
// Called when this state is pushed onto the stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateMainMenu::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	m_pStateMachine->Pop();
	m_pStateMachine->Push(0, p_mContextParameters);
}

//------------------------------------------------------------------------------
// Method:		Suspend
// Parameter:	const int p_iNewState - index of new state being pushed
// 
// Called when this state is pushed down in the stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateMainMenu::Suspend(const int p_iNewState)
{
	m_bSuspended = true;
}

//------------------------------------------------------------------------------
// Method:		Unsuspend
// Parameter:	const int p_iPrevState - index of previous state popped
// 
// Called when this state is popped back to top of stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateMainMenu::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;
}

//------------------------------------------------------------------------------
// Method:    Update
// Parameter: const float p_fDelta
// 
// Called each from when this state is active.
//------------------------------------------------------------------------------
void Kartaclysm::StateMainMenu::Update(const float p_fDelta)
{
	// Do not update when suspended
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);
	}
}

//------------------------------------------------------------------------------
// Method:    PreRender
// 
// Called before rendering occurs.
//------------------------------------------------------------------------------
void Kartaclysm::StateMainMenu::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

//------------------------------------------------------------------------------
// Method:    Exit
// 
// Called when this state is popped off the stack.
//------------------------------------------------------------------------------
void Kartaclysm::StateMainMenu::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}