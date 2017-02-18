//----------------------------------------------------------------------------
// ComponentAIDriver.h
// Author: David MacIntosh
//
// Component that uses AI to control a ComponentKartController
//----------------------------------------------------------------------------

#ifndef COMPONENT_AI_DRIVER_H
#define COMPONENT_AI_DRIVER_H

#include <tinyxml2.h>

#include "Common.h"
#include "Component.h"
#include "GameObject.h"

namespace Kartaclysm
{
	class ComponentAIDriver : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_AIDriver"; }
		virtual const std::string ComponentID() const override	{ return "GOC_AIDriver"; }

		// Destruction.
		virtual ~ComponentAIDriver();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		void QueryPlayerMovement(
			const int p_iPlayerNum,
			int& p_iAccelerate,
			int& p_iBrake,
			int& p_iSlide,
			float& p_fTurn) const;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAIDriver(
			HeatStroke::GameObject* p_pGameObject
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;
	};
}

#endif // COMPONENT_AI_DRIVER_H