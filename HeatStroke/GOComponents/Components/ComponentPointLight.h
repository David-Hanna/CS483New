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
		static HeatStroke::Component* CreateComponent(
			GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode);

	public:
		~ComponentPointLight();

		virtual const std::string FamilyID() const override { return "GOC_PointLight"; }
		virtual const std::string ComponentID() const override { return "GOC_PointLight"; }

		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

	private:
		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			glm::vec3& p_vAmbient,
			glm::vec3& p_vDiffuse,
			glm::vec3& p_vSpecular,
			glm::vec3& p_vAttenuation,
			float& p_fRange);

		static void ParseColorVectorElement(tinyxml2::XMLElement* p_pElement, glm::vec3& p_vColorVector);

	private:
		ScenePointLight m_mScenePointLight;

	private:
		ComponentPointLight(
			GameObject* p_pGameObject,
			const glm::vec3& p_vAmbient,
			const glm::vec3& p_vDiffuse,
			const glm::vec3& p_vSpecular,
			const glm::vec3& p_vAttenuation,
			const float      p_fRange);		
	};
}

#endif // COMPONENT_POINT_LIGHT_H