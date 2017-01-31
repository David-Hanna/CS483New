//----------------------------------------------------------------------------
// ComponentProjectile.cpp
// Author: Bradley Cooper
//
// Component that handles projectile collisions and events.
//----------------------------------------------------------------------------

#include "ComponentProjectile.h"

namespace Kartaclysm
{
	ComponentProjectile::ComponentProjectile(
		HeatStroke::GameObject* p_pGameObject,
		bool p_bFriendlyFire,
		const std::string& p_strBlastXML)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_strOriginator(""),
		m_strOnHitEvent(""),
		m_bFriendlyFire(p_bFriendlyFire),
		m_strBlastXML(p_strBlastXML),
		m_pSelfDestructDelegate(nullptr)
	{
		m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentProjectile::HandleCollisionEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);

		if (m_strBlastXML != "")
		{
			m_pSelfDestructDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentProjectile::HandleSelfDestructEvent, this, std::placeholders::_1));
			HeatStroke::EventManager::Instance()->AddListener(GetGameObject()->GetGUID() + "_SelfDestruct", m_pSelfDestructDelegate);
		}
	}

	ComponentProjectile::~ComponentProjectile()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Collision", m_pCollisionDelegate);
		delete m_pCollisionDelegate;
		m_pCollisionDelegate = nullptr;

		if (m_pSelfDestructDelegate != nullptr)
		{
			HeatStroke::EventManager::Instance()->RemoveListener(GetGameObject()->GetGUID() + "_SelfDestruct", m_pSelfDestructDelegate);
			delete m_pSelfDestructDelegate;
			m_pSelfDestructDelegate = nullptr;
		}
	}

	HeatStroke::Component* ComponentProjectile::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		bool bFriendlyFire = false;
		std::string strBlastXML = "";

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, bFriendlyFire, strBlastXML);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, bFriendlyFire, strBlastXML);
		}
		
		return new ComponentProjectile(
			p_pGameObject,
			bFriendlyFire,
			strBlastXML
			);
	}

	void ComponentProjectile::HandleCollisionEvent(const HeatStroke::Event* p_pEvent)
	{
		std::string guid1, guid2;
		p_pEvent->GetRequiredGameObjectParameter("Object1GUID", guid1);
		p_pEvent->GetRequiredGameObjectParameter("Object2GUID", guid2);

		HeatStroke::GameObject* pOther = nullptr;
		if (guid1.compare(m_pGameObject->GetGUID()) == 0)
		{
			pOther = GetGameObject()->GetManager()->GetGameObject(guid2);
		}
		else if (guid2.compare(m_pGameObject->GetGUID()) == 0)
		{
			pOther = GetGameObject()->GetManager()->GetGameObject(guid1);
		}

		if (pOther != nullptr)
		{
			if (!m_bFriendlyFire && pOther->GetGUID() == m_strOriginator)
			{
				return;
			}
			else if (pOther->HasTag("Racer"))
			{
				if (m_strBlastXML != "")
				{
					HeatStroke::EventManager::Instance()->TriggerEvent(new HeatStroke::Event(GetGameObject()->GetGUID() + "_SelfDestruct"));
				}
				else if (m_strOnHitEvent != "")
				{
					HeatStroke::Event* pEvent = new HeatStroke::Event(m_strOnHitEvent);
					pEvent->SetGameObjectParameter("Originator", m_strOriginator);
					pEvent->SetGameObjectParameter("Target", pOther->GetGUID());
					HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);
				}

				GetGameObject()->GetManager()->DestroyGameObject(GetGameObject());
			}
		}
	}

	void ComponentProjectile::HandleSelfDestructEvent(const HeatStroke::Event* p_pEvent)
	{
		if (m_strBlastXML != "")
		{
			HeatStroke::GameObject* pBlast = GetGameObject()->GetManager()->CreateGameObject(m_strBlastXML);
			pBlast->GetTransform().SetTranslation(GetGameObject()->GetTransform().GetTranslation());

			ComponentProjectile* pProjectile = static_cast<ComponentProjectile*>(pBlast->GetComponent("GOC_Projectile"));
			assert(pProjectile != nullptr);
			pProjectile->SetOriginator(m_strOriginator);
			pProjectile->SetOnHitEvent(m_strOnHitEvent);

			m_strBlastXML = "";
			m_strOnHitEvent = "";
		}
	}

	void ComponentProjectile::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		bool& p_bFriendlyFire,
		std::string& p_strBlastXML)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_Projectile") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "FriendlyFire") == 0)
			{
				HeatStroke::EasyXML::GetRequiredBoolAttribute(pChildElement, "value", p_bFriendlyFire);
			}
			else if (strcmp(szNodeName, "BlastXML") == 0)
			{
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "path", p_strBlastXML);
			}
		}
	}
}