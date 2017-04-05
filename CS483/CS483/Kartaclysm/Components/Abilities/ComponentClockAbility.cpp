//----------------------------------------------------------------------------
// ComponentClockAbility.cpp
// Author: Bradley Cooper
//
// Clockmaker's ability to throw an AoE lobbed bomb forward.
//----------------------------------------------------------------------------

#include "ComponentClockAbility.h"

namespace Kartaclysm
{
	ComponentClockAbility::ComponentClockAbility(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strProjectileXML,
		float p_fDuration)
		:
		ComponentAbility(p_pGameObject),
		m_strProjectileXML(p_strProjectileXML),
		m_strChargeEventName(m_strPlayerX + "_ClockCharges"),
		m_fDuration(p_fDuration)
	{
		// Listen to activation event ("Player0_KartAbility1" as example)
		m_pAbilityDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentClockAbility::AbilityCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_DriverAbility1", m_pAbilityDelegate);

		// Listen to on hit event
		m_pOnHitDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentClockAbility::OnHitCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strPlayerX + "_ClockHit", m_pOnHitDelegate);

		// Listen to any events that change the charge count
		m_pChargeDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentClockAbility::ChargeCallback, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener(m_strChargeEventName, m_pChargeDelegate);
	}

	ComponentClockAbility::~ComponentClockAbility()
	{
		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_DriverAbility1", m_pAbilityDelegate);
		delete m_pAbilityDelegate;
		m_pAbilityDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strPlayerX + "_ClockHit", m_pOnHitDelegate);
		delete m_pOnHitDelegate;
		m_pOnHitDelegate = nullptr;

		HeatStroke::EventManager::Instance()->RemoveListener(m_strChargeEventName, m_pChargeDelegate);
		delete m_pChargeDelegate;
		m_pChargeDelegate = nullptr;
	}

	HeatStroke::Component* ComponentClockAbility::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		std::string strProjectileXML = "";
		float fDuration;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, strProjectileXML, fDuration);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, strProjectileXML, fDuration);
		}

		// Check that we got everything we needed.
		assert(strProjectileXML != "");
		assert(fDuration > 0.0f);

		return new ComponentClockAbility(
			p_pGameObject,
			strProjectileXML,
			fDuration
			);
	}

	void ComponentClockAbility::Init()
	{
		// Find ability conditions component
		m_pConditions = static_cast<ComponentAbilityConditions*>(GetGameObject()->GetComponent("GOC_AbilityConditions"));
		assert(m_pConditions != nullptr && "Cannot find component.");

		// Register with AI (if applicable)
		HeatStroke::GameObject* pAIKartObject = GetGameObject()->GetManager()->GetGameObject(m_strPlayerX);
		ComponentAIDriver* pAIDriver = static_cast<ComponentAIDriver*>(pAIKartObject->GetComponent("GOC_AIDriver"));
		if (pAIDriver != nullptr)
		{
			pAIDriver->RegisterComponentAbility(this);
		}
	}

	void ComponentClockAbility::Activate()
	{
		if (m_pConditions->CanActivate())
		{
			m_pConditions->ResetCooldown();

			// Send event for charge change
			HeatStroke::Event* pEvent = new HeatStroke::Event(m_strChargeEventName);
			pEvent->SetIntParameter("Increase", 0);
			pEvent->SetIntParameter("Reset", 1);
			HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

			// Spawn projectile
			HeatStroke::GameObject* pStrike = GetGameObject()->GetManager()->CreateGameObject(m_strProjectileXML);
			pStrike->GetTransform().SetTranslation(GetGameObject()->GetTransform().GetTranslation());
			pStrike->GetTransform().SetRotation(GetGameObject()->GetTransform().GetRotation());

			ComponentProjectile* pProjectile = static_cast<ComponentProjectile*>(pStrike->GetComponent("GOC_Projectile"));
			assert(pProjectile != nullptr);
			pProjectile->SetOriginator(m_strPlayerX);
			pProjectile->SetOnHitEvent(m_strPlayerX + "_ClockHit");

			// TODO: Absolute garbage way to handle this, but I'm not diving deep into the physics to change it
			ComponentKartController* pKart = static_cast<ComponentKartController*>(GetGameObject()->GetParent()->GetParent()->GetComponent("GOC_KartController"));
			ComponentSimplePhysics* pPhysics = static_cast<ComponentSimplePhysics*>(pStrike->GetComponent("GOC_SimplePhysics"));
			pPhysics->SetDirection(pKart->GetDirection());
			pPhysics->SetTrackHeight(pKart->GetGroundHeight());

			HeatStroke::AudioPlayer::Instance()->PlaySoundEffect("Assets/Sounds/clockmaker_clock_bomb.ogg");
		}
	}

	void ComponentClockAbility::AICheckCondition(HeatStroke::Component* p_pAIDriver)
	{
		ComponentAIDriver* pAIDriver = static_cast<ComponentAIDriver*>(p_pAIDriver);
		if (pAIDriver != nullptr)
		{
			if (m_pConditions->CanActivate() &&
				pAIDriver->CurrentPosition() > 1 &&
				rand() % 8 == 0)
			{
				Activate();
			}
		}
	}

	void ComponentClockAbility::OnHitCallback(const HeatStroke::Event* p_pEvent)
	{
		std::string strTargetGUID;
		p_pEvent->GetRequiredGameObjectParameter("Target", strTargetGUID);

		HeatStroke::Event* pEvent = new HeatStroke::Event("AbilityUse");
		pEvent->SetStringParameter("Originator", m_strPlayerX);
		pEvent->SetStringParameter("Target", strTargetGUID);
		pEvent->SetStringParameter("Ability", "Clock");
		pEvent->SetStringParameter("Effect", "SpinOut");
		pEvent->SetFloatParameter("Duration", m_fDuration);
		HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
	}

	void ComponentClockAbility::ChargeCallback(const HeatStroke::Event* p_pEvent)
	{
		int iIncrease = 0;
		int iReset = 0;

		p_pEvent->GetOptionalIntParameter("Increase", iIncrease, 0);
		p_pEvent->GetOptionalIntParameter("Reset", iReset, 0);

		if (iIncrease == 1)
		{
			m_pConditions->AddCharge();
		}

		if (iReset == 1)
		{
			m_pConditions->ResetCharges();
		}

		m_pConditions->SetSpecialCondition(m_pConditions->GetCharges() == m_pConditions->GetMaxCharges());
	}

	void ComponentClockAbility::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		std::string& p_strProjectileXML,
		float& p_fDuration)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_ClockAbility") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "ProjectileXML") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strProjectileXML);
			}
			else if (strcmp(szNodeName, "Duration") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fDuration);
			}
		}
	}
}