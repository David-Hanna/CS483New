//----------------------------------------------------------------------------
// ComponentCamera.h
// Author: Carel Boers / David Hanna
//
// This component allows a GameObject to be attached as a camera in the
// SceneManager. This allows great flexibility for camera behaviour.
//----------------------------------------------------------------------------

#ifndef COMPONENT_CAMERA_H
#define COMPONENT_CAMERA_H

#include <GL\glew.h>
#include <glm.hpp>
#include <tinyxml2.h>

#include "Component.h"
#include "HierarchicalTransform.h"
#include "GameObject.h"
#include "EasyXML.h"
#include "SceneCamera.h"
#include "SceneManager.h"

namespace HeatStroke
{
	class ComponentCamera : public Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods.
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_Camera"; }
		virtual const std::string ComponentID() const override	{ return "GOC_Camera"; }

		// Destruction.
		virtual ~ComponentCamera();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		// Accessors
		SceneCamera& GetSceneCamera() { return m_mSceneCamera; }
		void		 SetSceneCamera(const SceneCamera& p_mSceneCamera);

	protected:
		//----------------------------------------------------------------------
		// Protected members.
		//----------------------------------------------------------------------
		// The Scene Camera this component is responsible for.
		SceneCamera m_mSceneCamera;

		// If this Camera is targetting a Game Object, this will be non-null.
		GameObject* m_pTargetGameObject;

		//----------------------------------------------------------------------
		// Protected methods
		//----------------------------------------------------------------------
		// Prevent public construction.
		ComponentCamera(
			GameObject* p_pGameObject,
			const float p_fFOV,
			const float p_fAspectRatio,
			const float p_fNearClip,
			const float p_fFarClip,
			GameObject* p_pTargetGameObject
		);

	private:
		//----------------------------------------------------------------------
		// Private methods
		//----------------------------------------------------------------------
		// Prevent Copying
		ComponentCamera(const ComponentCamera&) = delete;
		ComponentCamera& operator=(const ComponentCamera&) = delete;

		// Helper for parsing the GOC_Camera node.
		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
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