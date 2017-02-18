//----------------------------------------------------------------------------
// ComponentAIDriver.cpp
// Author: David MacIntosh
//
// Component that uses AI to control a ComponentKartController
//----------------------------------------------------------------------------

#include "ComponentAIDriver.h"

namespace Kartaclysm
{
	ComponentAIDriver::ComponentAIDriver(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject)
	{
	}

	ComponentAIDriver::~ComponentAIDriver()
	{
	}

	HeatStroke::Component* ComponentAIDriver::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		return new ComponentAIDriver(
			p_pGameObject
			);
	}

	void ComponentAIDriver::Update(const float p_fDelta)
	{
	}

	void ComponentAIDriver::QueryPlayerMovement(
		const int p_iPlayerNum,
		int& p_iAccelerate,
		int& p_iBrake,
		int& p_iSlide,
		float& p_fTurn) const
	{
		p_iAccelerate = 1;
	}
}