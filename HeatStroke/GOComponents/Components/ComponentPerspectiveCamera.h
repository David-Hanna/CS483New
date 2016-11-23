//----------------------------------------------------------------------------
// ComponentPerspectiveCamera
// Author: David Hanna
//
// Adds a perspective-view scene camera into the scene manager.
//----------------------------------------------------------------------------

#ifndef COMPONENT_PERSPECTIVE_CAMERA_H
#define COMPONENT_PERSPECTIVE_CAMERA_H

#include "Component.h"
#include "GameObject.h"
#include "EasyXML.h"
#include "SceneManager.h"

namespace HeatStroke
{
	class ComponentPerspectiveCamera : public Component
	{
	public:
		virtual const std::string FamilyID() const override		{ return "GOC_PerspectiveCamera"; }
		virtual const std::string ComponentID() const override	{ return "GOC_PerspectiveCamera"; }

		// Destruction.
		virtual ~ComponentPerspectiveCamera();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

	private:
		// The Scene Camera this component is responsible for.
		ScenePerspectiveCamera m_mScenePerspectiveCamera;

		// The viewport the scene camera was placed in in the scene manager.
		SceneManager::SceneViewportSelection m_eViewportSelection;

		// If this Camera is targetting a Game Object, this will be non-null.
		GameObject* m_pTargetGameObject;

	private:
		// Prevent public construction.
		ComponentPerspectiveCamera(
			GameObject* p_pGameObject,
			SceneManager::SceneViewportSelection p_eViewportSelection,
			const float p_fFOV,
			const float p_fAspectRatio,
			const float p_fNearClip,
			const float p_fFarClip,
			GameObject* p_pTargetGameObject
		);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strViewportSelection,
			float& p_fFOV,
			float& p_fAspectRatioWidth,
			float& p_fAspectRatioHeight,
			float& p_fNearClip,
			float& p_fFarClip,
			std::string& p_strTargetGUID
		);
	};
}

#endif // COMPONENT_CAMERA_H