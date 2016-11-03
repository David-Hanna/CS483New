//------------------------------------------------------------------------
// Component3DModel
// Author:	David Hanna
//	
// Provides 3D rendering of a .obj model to the screen for a GameObject.
//------------------------------------------------------------------------

#ifndef COMPONENT_3D_MODEL_H
#define COMPONENT_3D_MODEL_H

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class Component3DModel : public ComponentRenderable
	{
	public:
		virtual const std::string ComponentID() const override { return std::string("GOC_3DModel"); }

		~Component3DModel();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;
		virtual void PreRender() override { ComponentRenderable::PreRender(); }

	protected:
		Component3DModel(GameObject* p_pGameObject, const std::string& p_strOBJFileName);

		virtual void SyncTransform();

	private:
		Model m_mModel;

		// Prevent copying
		Component3DModel(const Component3DModel&) = delete;
		Component3DModel& operator=(const Component3DModel&) = delete;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strOBJFileName
		);
	};
}

#endif