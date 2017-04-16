//----------------------------------------------------------------------------
// ComponentObjectSpawn.cpp
// Author: David MacIntosh
//
// Simple component that spawns an object when initialized
//----------------------------------------------------------------------------

#include "ComponentObjectSpawn.h"

namespace Kartaclysm
{
	ComponentObjectSpawn::ComponentObjectSpawn(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strObjectXML)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_strObjectXML(p_strObjectXML),
		m_bCreated(false)
	{
	}

	ComponentObjectSpawn::~ComponentObjectSpawn()
	{
	}

	void ComponentObjectSpawn::Update(const float p_fDelta)
	{
		if (!m_bCreated)
		{
			if (m_strObjectXML != "")
			{
				HeatStroke::GameObject* pObject = GetGameObject()->GetManager()->CreateGameObject(m_strObjectXML);
				pObject->GetTransform().SetTranslation(GetGameObject()->GetTransform().GetTranslation());
			}

			m_bCreated = true;
		}
	}

	HeatStroke::Component* ComponentObjectSpawn::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// Defaults
		std::string strObjectXML = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(p_pBaseNode->FirstChildElement("Object"), "xml", strObjectXML);

		return new ComponentObjectSpawn(
			p_pGameObject,
			strObjectXML
			);
	}
}