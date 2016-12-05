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
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_strOriginator(""),
		m_strOnHitEvent("")
	{
		m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentProjectile::HandleCollisionEvent, this, std::placeholders::_1));
		HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);
	}

	ComponentProjectile::~ComponentProjectile()
	{
		HeatStroke::EventManager::Instance()->RemoveListener("Collision", m_pCollisionDelegate);
		delete m_pCollisionDelegate;
		m_pCollisionDelegate = nullptr;
	}

	HeatStroke::Component* ComponentProjectile::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		/*
		// Defaults
		float fStrength = 0.0f;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fStrength);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fStrength);
		}

		// Check that we got everything we needed.
		assert(fStrength != 0.0f);
		*/

		return new ComponentProjectile(
			p_pGameObject
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
			if (pOther->HasTag("Racer"))
			{
				HeatStroke::Event* pEvent = new HeatStroke::Event(m_strOnHitEvent);
				pEvent->SetGameObjectParameter("Originator", m_strOriginator);
				pEvent->SetGameObjectParameter("Target", pOther->GetGUID());
				HeatStroke::EventManager::Instance()->TriggerEvent(pEvent);

				GetGameObject()->GetManager()->DestroyGameObject(GetGameObject());
			}
			else if (pOther->HasTag("Wall") || pOther->HasTag("Projectile"))
			{
				GetGameObject()->GetManager()->DestroyGameObject(GetGameObject());
			}
		}

	}

	void ComponentProjectile::ParseNode(
		tinyxml2::XMLNode* p_pNode)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_Projectile") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			/*if (strcmp(szNodeName, "Strength") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fStrength);
			}*/
		}
	}
}