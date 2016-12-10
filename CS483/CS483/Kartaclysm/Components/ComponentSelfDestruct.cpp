//----------------------------------------------------------------------------
// ComponentSelfDestruct.cpp
// Author: Bradley Cooper
//
// Component that destroys the game object after a time period or on collision.
//----------------------------------------------------------------------------

#include "ComponentSelfDestruct.h"

namespace Kartaclysm
{
	ComponentSelfDestruct::ComponentSelfDestruct(
		HeatStroke::GameObject* p_pGameObject,
		float p_fTimer,
		const std::vector<std::string>& p_vCollisionTags)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fTimer(p_fTimer),
		m_vCollisionTags(p_vCollisionTags),
		m_bDestroy(false),
		m_pCollisionDelegate(nullptr)
	{
		if (!m_vCollisionTags.empty())
		{
			m_pCollisionDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentSelfDestruct::HandleCollisionEvent, this, std::placeholders::_1));
			HeatStroke::EventManager::Instance()->AddListener("Collision", m_pCollisionDelegate);
		}
	}

	ComponentSelfDestruct::~ComponentSelfDestruct()
	{
		if (m_pCollisionDelegate != nullptr)
		{
			HeatStroke::EventManager::Instance()->RemoveListener("Collision", m_pCollisionDelegate);
			delete m_pCollisionDelegate;
			m_pCollisionDelegate = nullptr;
		}
	}

	HeatStroke::Component* ComponentSelfDestruct::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fTimer = 0.0f;
		std::vector<std::string> vCollisionTags;

		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fTimer, vCollisionTags);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fTimer, vCollisionTags);
		}

		// Check that we got everything we needed.
		assert(fTimer >= 0.0f || !vCollisionTags.empty());

		return new ComponentSelfDestruct(
			p_pGameObject,
			fTimer,
			vCollisionTags
			);
	}

	void ComponentSelfDestruct::Update(const float p_fDelta)
	{
		if (m_bDestroy)
		{
			GetGameObject()->GetManager()->DestroyGameObject(GetGameObject());
		}
		else if (m_fTimer >= 0.0f)
		{
			m_fTimer -= p_fDelta;

			if (m_fTimer <= 0.0f)
			{
				SelfDestruct();
			}
		}
	}

	void ComponentSelfDestruct::HandleCollisionEvent(const HeatStroke::Event* p_pEvent)
	{
		std::string strGUID1, strGUID2;
		p_pEvent->GetRequiredGameObjectParameter("Object1GUID", strGUID1);
		p_pEvent->GetRequiredGameObjectParameter("Object2GUID", strGUID2);

		HeatStroke::GameObject* pOther = nullptr;
		if (strGUID1.compare(m_pGameObject->GetGUID()) == 0)
		{
			pOther = m_pGameObject->GetManager()->GetGameObject(strGUID2);
		}
		else if (strGUID2.compare(m_pGameObject->GetGUID()) == 0)
		{
			pOther = m_pGameObject->GetManager()->GetGameObject(strGUID1);
		}

		if (pOther != nullptr)
		{
			for (unsigned int i = 0; i < m_vCollisionTags.size(); i++)
			{
				if (pOther->HasTag(m_vCollisionTags.at(i)))
				{
					SelfDestruct();
					break;
				}
			}
		}
	}

	void ComponentSelfDestruct::SelfDestruct()
	{
		if (!m_bDestroy)
		{
			HeatStroke::EventManager::Instance()->TriggerEvent(new HeatStroke::Event(GetGameObject()->GetGUID() + "_SelfDestruct"));
			m_bDestroy = true;
		}
	}

	void ComponentSelfDestruct::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fTimer,
		std::vector<std::string>& p_vCollisionTags)
	{
		assert(p_pNode != nullptr);
		assert(strcmp(p_pNode->Value(), "GOC_SelfDestruct") == 0);

		for (tinyxml2::XMLElement* pChildElement = p_pNode->FirstChildElement();
			pChildElement != nullptr;
			pChildElement = pChildElement->NextSiblingElement())
		{
			const char* szNodeName = pChildElement->Value();

			if (strcmp(szNodeName, "Timer") == 0)
			{
				HeatStroke::EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fTimer);
			}
			else if (strcmp(szNodeName, "Collision") == 0)
			{
				std::string strTags;
				HeatStroke::EasyXML::GetRequiredStringAttribute(pChildElement, "tags", strTags);
				boost::split(p_vCollisionTags, strTags, boost::is_any_of(","));
			}
		}
	}
}