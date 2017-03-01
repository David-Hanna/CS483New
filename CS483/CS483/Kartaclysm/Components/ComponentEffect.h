//------------------------------------------------------------------------
// ComponentEffect.h
// Author:	David MacIntosh
//	
// Holds a particle effect.
//------------------------------------------------------------------------

#ifndef COMPONENT_EFFECT_H
#define COMPONENT_EFFECT_H

#include <tinyxml2.h>

#include "Common.h"
#include "Component.h"
#include "GameObject.h"

namespace Kartaclysm
{
	class ComponentEffect : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_Effect"; }
		virtual const std::string ComponentID() const override	{ return "GOC_Effect"; }

		// Destruction.
		virtual ~ComponentEffect();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		void SetDuration(float p_fDuration) { m_fDuration = p_fDuration; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentEffect(
			HeatStroke::GameObject* p_pGameObject
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;
		float m_fDuration;
		bool m_bStarted;
	};
}

#endif // COMPONENT_EFFECT_H