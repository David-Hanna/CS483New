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
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject),
		m_fDuration(1.0f),
		m_bStarted(false)
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

		return new ComponentEffect(
			p_pGameObject
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
				HeatStroke::Effect* pCollisionParticleEffect = pComponentParticleEffect->GetEffect("collision");
				if (pCollisionParticleEffect != nullptr)
				{
					pCollisionParticleEffect->Start();
					m_bStarted = true;
				}
			}
		}
	}
}