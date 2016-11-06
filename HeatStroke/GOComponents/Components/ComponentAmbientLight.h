//------------------------------------------------------------------------
// ComponentAmbientLight
// Author:	David Hanna
//	
// Adds an Ambient Light to the scene for a GameObject.
//------------------------------------------------------------------------

#ifndef COMPONENT_AMBIENT_LIGHT_H
#define COMPONENT_AMBIENT_LIGHT_H

#include "Component.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ComponentAmbientLight : public Component
	{
	public:
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

	public:
		~ComponentAmbientLight();

		const std::string FamilyID()	const override { return std::string("GOC_AmbientLight"); }
		const std::string ComponentID() const override { return std::string("GOC_AmbientLight"); }

		void Init() override;
		void Update(const float p_fDelta) override {}

	private:
		static void ParseNode(tinyxml2::XMLNode* p_pNode, glm::vec3& p_vAmbientLightColor);

	private:
		SceneAmbientLight m_mAmbientLight;

	private:
		ComponentAmbientLight(GameObject* p_pGameObject, const glm::vec3& p_vAmbientLightColor);
	};
}

#endif