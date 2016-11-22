//----------------------------------------------------------------------------
// ComponentSampleAbility.h
// Author: Bradley Cooper
//
// Component that handles the conditions for activating an ability.
//----------------------------------------------------------------------------

#include "ComponentSampleAbility.h"

namespace Kartaclysm
{
	ComponentSampleAbility::ComponentSampleAbility(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strName,
		const std::string& p_strDescription,
		const std::string& p_strIdentifier)
		:
		ComponentSampleAbility::ComponentAbility(
			p_pGameObject,
			p_strName,
			p_strDescription,
			p_strIdentifier),
		m_fChargeCooldown(3.0f)
	{
		// TO DO, hard coded for first player and first ability (but this is only a sample)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentSampleAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strIdentifier, m_pAbilityDelegate);
	}

	ComponentSampleAbility::~ComponentSampleAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strIdentifier, m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;
	}

	HeatStroke::Component* ComponentSampleAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults
		std::string strName = "Sample Ability";
		std::string strDescription = "A very long, convoluted description that goes on for what seems like forever.";
		std::string strIdentifier = "Player0_KartAbility1";

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			//ParseNode(p_pBaseNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}
		if (p_pOverrideNode != nullptr)
		{
			//ParseNode(p_pOverrideNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}

		return new ComponentSampleAbility(
			p_pGameObject,
			strName,
			strDescription,
			strIdentifier
			);
	}

	void ComponentSampleAbility::Init()
	{
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");
	}

	void ComponentSampleAbility::Update(const float p_fDelta)
	{
		if (m_fChargeCooldown > 0.0f)
		{
			m_fChargeCooldown -= p_fDelta;
		}
		else
		{
			m_fChargeCooldown = 3.0f;
			m_pConditions->AddCharge();
		}
	}

	void ComponentSampleAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();
			m_pConditions->RemoveCharge();
			m_fChargeCooldown = 6.0f;
		}
	}
}