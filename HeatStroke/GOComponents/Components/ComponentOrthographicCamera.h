//----------------------------------------------------------------------------
// ComponentOrthographicCamera
// Author: David Hanna
//
// Adds an orthographic-view scene camera into the scene manager.
//----------------------------------------------------------------------------

#ifndef COMPONENT_ORTHOGRAPHIC_CAMERA_H
#define COMPONENT_ORTHOGRAPHIC_CAMERA_H

#include "Component.h"
#include "GameObject.h"
#include "EasyXML.h"
#include "SceneManager.h"

namespace HeatStroke
{
	class ComponentOrthographicCamera : public Component
	{
	public:
		virtual const std::string FamilyID() const override		{ return "GOC_OrthographicCamera"; }
		virtual const std::string ComponentID() const override	{ return "GOC_OrthographicCamera"; }

		// Destruction.
		virtual ~ComponentOrthographicCamera();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		// Game Loop methods.
		virtual void Init() override						{}
		virtual void Update(const float p_fDelta) override	{}

	private:
		// The Scene Camera this component is responsible for.
		SceneOrthographicCamera m_mSceneOrthographicCamera;

		// The viewport the scene camera was placed in in the scene manager.
		SceneManager::SceneViewportSelection m_eViewportSelection;

	private:
		// Prevent public construction.
		ComponentOrthographicCamera(
			GameObject* p_pGameObject,
			SceneManager::SceneViewportSelection p_eViewportSelection,
			const glm::vec3& p_vPosition,
			const glm::vec3& p_vTarget,
			const glm::vec3& p_vUp,
			const float p_fLeft,
			const float p_fRight,
			const float p_fBottom,
			const float p_fTop
		);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strViewportSelection,
			glm::vec3& p_vPosition,
			glm::vec3& p_vTarget,
			glm::vec3& p_vUp,
			float& p_fLeft,
			float& p_fRight,
			float& p_fBottom,
			float& p_fTop
		);
	};
}

#endif // COMPONENT_CAMERA_H