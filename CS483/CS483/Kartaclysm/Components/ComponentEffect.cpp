//------------------------------------------------------------------------
// ComponentEffect.cpp
// Author:	David MacIntosh
//	
// Holds a particle effect.
//------------------------------------------------------------------------

#include "ComponentEffect.h"

#include "ComponentParticleEffect.h"

namespace Kartaclysm
{
	ComponentEffect::ComponentEffect(
		HeatStroke::GameObject* p_pGameObject,
		std::string p_strEffectId,
		float p_fDuration)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fDuration(p_fDuration),
		m_bStarted(false),
		m_strEffectId(p_strEffectId)
	{
	}

	ComponentEffect::~ComponentEffect()
	{
	}

	HeatStroke::Component* ComponentEffect::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		std::string strEffectId = "";
		HeatStroke::EasyXML::GetRequiredStringAttribute(p_pBaseNode->FirstChildElement("Effect"), "id", strEffectId);

		float fDuration = 0.0f;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Effect"), "duration", fDuration);

		return new ComponentEffect(
			p_pGameObject,
			strEffectId,
			fDuration
			);
	}

	void ComponentEffect::Update(const float p_fDelta)
	{
		m_fDuration -= p_fDelta;

		if (m_fDuration <= 0.0f)
		{
			m_pGameObject->GetManager()->DestroyGameObject(m_pGameObject);
		}

		if (!m_bStarted)
		{
			HeatStroke::ComponentParticleEffect* pComponentParticleEffect = static_cast<HeatStroke::ComponentParticleEffect*>(m_pGameObject->GetComponent("GOC_ParticleEffect"));
			if (pComponentParticleEffect)
			{
				HeatStroke::Effect* pCollisionParticleEffect = pComponentParticleEffect->GetEffect(m_strEffectId);
				if (pCollisionParticleEffect != nullptr)
				{
					pCollisionParticleEffect->Start();
					m_bStarted = true;
				}
			}
		}
	}
}