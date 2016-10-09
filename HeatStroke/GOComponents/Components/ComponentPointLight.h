//-----------------------------------------------------------------------
// ComponentPointLight.h
// Author: David Hanna
//
// Represents a component which acts as a point light from the
// parent GameObject's transform.
//-----------------------------------------------------------------------

#ifndef COMPONENT_POINT_LIGHT_H
#define COMPONENT_POINT_LIGHT_H

#include "Component.h"
#include "GameObject.h"
#include "ScenePointLight.h"
#include "SceneManager.h"

namespace HeatStroke
{
	class ComponentPointLight : public Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		// Component identification
		virtual const std::string FamilyID() const override { return "GOC_PointLight"; }
		virtual const std::string ComponentID() const override { return "GOC_PointLight"; }

		// Destructor.
		virtual ~ComponentPointLight();

		// Factory Construction
		static HeatStroke::Component* CreateComponent(
			GameObject* p_pGameObject, 
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode);

		// Updates the light position to match the parent Game Object.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		// Accessors
		ScenePointLight& GetScenePointLight() { return m_mScenePointLight; }
		void SetScenePointLight(const ScenePointLight& p_mScenePointLight);

	protected:
		//--------------------------------------------------------------------------
		// Protected members
		//--------------------------------------------------------------------------
		ScenePointLight m_mScenePointLight;

		//--------------------------------------------------------------------------
		// Protected methods.
		//--------------------------------------------------------------------------
		// Prevent public construction.
		ComponentPointLight(
			GameObject* p_pGameObject,
			const glm::vec3& p_vAmbient,
			const glm::vec3& p_vDiffuse,
			const glm::vec3& p_vSpecular,
			const glm::vec3& p_vAttenuation,
			const float      p_fRange);

	private:
		//--------------------------------------------------------------------------
		// Private methods.
		//--------------------------------------------------------------------------
		// Prevent copying
		ComponentPointLight(const ComponentPointLight&) = delete;
		ComponentPointLight& operator=(const ComponentPointLight&) = delete;

		// Helper method for parsing a "GOC_PointLight" node.
		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			glm::vec3& p_vAmbient,
			glm::vec3& p_vDiffuse,
			glm::vec3& p_vSpecular,
			glm::vec3& p_vAttenuation,
			float& p_fRange);

		// Helper method for parsing an xml element with rgb attributes.
		static void ParseColorVectorElement(tinyxml2::XMLElement* p_pElement, glm::vec3& p_vColorVector);
	};
}

#endif // COMPONENT_POINT_LIGHT_H