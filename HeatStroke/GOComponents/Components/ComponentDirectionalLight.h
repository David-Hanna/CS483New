//------------------------------------------------------------------------
// ComponentDirectionalLight
// Author:	David Hanna
//	
// Adds a Directional Light to the scene for a GameObject.
//------------------------------------------------------------------------

#ifndef COMPONENT_DIRECTIONAL_LIGHT_H
#define COMPONENT_DIRECTIONAL_LIGHT_H

#include "Component.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ComponentDirectionalLight : public Component
	{
	public:
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

	public:
		~ComponentDirectionalLight();

		const std::string FamilyID()	const override { return std::string("GOC_DirectionalLight"); }
		const std::string ComponentID() const override { return std::string("GOC_DirectionalLight"); }

		void Init() override;
		void Update(const float p_fDelta) override {}

	private:
		static void ParseNode(tinyxml2::XMLNode* p_pNode, glm::vec3& p_vDirectionalLightDirection, glm::vec3& p_vDirectionalLightColor);

	private:
		SceneDirectionalLight m_mDirectionalLight;

	private:
		ComponentDirectionalLight(GameObject* p_pGameObject, const glm::vec3& p_vDirectionalLightDirection, const glm::vec3& p_vDirectionalLightColor);
	};
}

#endif