//----------------------------------------------------------------------------
// ComponentCameraController.cpp
// Author: David MacIntosh
//
// Component that handles camera positioning.
//----------------------------------------------------------------------------

#ifndef COMPONENT_CAMERA_CONTROLLER_H
#define COMPONENT_CAMERA_CONTROLLER_H

#include <tinyxml2.h>

#include "Component.h"

namespace Kartaclysm
{
	class ComponentCameraController : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_CameraController"; }
		virtual const std::string ComponentID() const override	{ return "GOC_CameraController"; }

		// Destruction.
		virtual ~ComponentCameraController();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentCameraController(
			HeatStroke::GameObject* p_pGameObject
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		HeatStroke::GameObject* m_pTargetKart;
	};
}

#endif // COMPONENT_CAMERA_CONTROLLER_H