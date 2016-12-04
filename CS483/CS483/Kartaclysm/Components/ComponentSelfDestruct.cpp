//----------------------------------------------------------------------------
// ComponentSelfDestruct.cpp
// Author: Bradley Cooper
//
// Component that destroys the game object after a time period.
//----------------------------------------------------------------------------

#include "ComponentSelfDestruct.h"

namespace Kartaclysm
{
	ComponentSelfDestruct::ComponentSelfDestruct(
		HeatStroke::GameObject* p_pGameObject,
		float p_fTimer)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fTimer(p_fTimer)
	{
	}

	ComponentSelfDestruct::~ComponentSelfDestruct()
	{
	}

	HeatStroke::Component* ComponentSelfDestruct::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		float fTimer = 0.0f;

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, fTimer);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, fTimer);
		}

		// Check that we got everything we needed.
		assert(fTimer > 0.0f);

		return new ComponentSelfDestruct(
			p_pGameObject,
			fTimer
			);
	}

	void ComponentSelfDestruct::Update(const float p_fDelta)
	{
		m_fTimer -= p_fDelta;

		if (m_fTimer <= 0.0f)
		{
			HeatStroke::EventManager::Instance()->TriggerEvent(new HeatStroke::Event(GetGameObject()->GetGUID() + "_SelfDestruct"));
			GetGameObject()->GetManager()->DestroyGameObject(GetGameObject());
		}
	}

	void ComponentSelfDestruct::ParseNode(
		tinyxml2::XMLNode* p_pNode,
		float& p_fTimer)
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
		}
	}
}