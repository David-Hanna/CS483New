//------------------------------------------------------------------------
// StateTrackSelect
// Author:	Matthew White
//	
// Menu state for selecting the track
//------------------------------------------------------------------------

#include "StateTrackSelect.h"

Kartaclysm::StateTrackSelect::StateTrackSelect()
	:
	m_pGameObjectManager(nullptr),
	m_bSuspended(true),
	m_strTrackDefinitionFile("")
{
}

Kartaclysm::StateTrackSelect::~StateTrackSelect()
{
	Exit();
}

void Kartaclysm::StateTrackSelect::Enter(const std::map<std::string, std::string>& p_mContextParameters)
{
	m_bSuspended = false;

	//TEMP hardcoding for now
	m_strTrackDefinitionFile = "CS483/CS483/Kartaclysm/Data/Tracks/up_and_over.xml";
	std::map<std::string, std::string> mContextParams = p_mContextParameters;
	mContextParams.insert(std::pair<std::string, std::string>("TrackDefinitionFile", m_strTrackDefinitionFile));

	m_pStateMachine->Pop();
	m_pStateMachine->Push(0, mContextParams);
}

void Kartaclysm::StateTrackSelect::Suspend(const int p_iNewState)
{
	m_bSuspended = true;
}

void Kartaclysm::StateTrackSelect::Unsuspend(const int p_iPrevState)
{
	m_bSuspended = false;
}

void Kartaclysm::StateTrackSelect::Update(const float p_fDelta)
{
	if (!m_bSuspended)
	{
		assert(m_pGameObjectManager != nullptr);
		m_pGameObjectManager->Update(p_fDelta);
	}
}

void Kartaclysm::StateTrackSelect::PreRender()
{
	// Render even when suspended
	assert(m_pGameObjectManager != nullptr);
	m_pGameObjectManager->PreRender();
}

void Kartaclysm::StateTrackSelect::Exit()
{
	m_bSuspended = false;

	if (m_pGameObjectManager != nullptr)
	{
		m_pGameObjectManager->DestroyAllGameObjects();
		delete m_pGameObjectManager;
		m_pGameObjectManager = nullptr;
	}
}