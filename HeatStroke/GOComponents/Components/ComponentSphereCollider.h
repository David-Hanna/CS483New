//------------------------------------------------------------------------
// ComponentSphereCollider
// Author:	David MacIntosh
//	
// Collider that takes the shape of a sphere
//------------------------------------------------------------------------

#ifndef COMPONENT_SPHERE_COLLIDER
#define COMPONENT_SPHERE_COLLIDER

#include "ComponentCollider.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"
#include "LineDrawer.h"

namespace HeatStroke
{
	class ComponentSphereCollider : public ComponentCollider
	{
	public:
		virtual const std::string ComponentID() const override { return std::string("GOC_SphereCollider"); }

		~ComponentSphereCollider();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;

		//virtual void PreRender() override { ComponentRenderable::PreRender(); }
		void DebugRender(LineDrawer* p_pLineDrawer);

		glm::vec3 GetOffset() { return m_pOffset; }
		float GetRadius() { return m_fRadius; }

	protected:
		ComponentSphereCollider(
			GameObject* p_pGameObject,
			glm::vec3 p_pOffset,
			float p_fRadius
		);

		virtual void SyncTransform();

	private:
		// Prevent copying
		ComponentSphereCollider(const ComponentSphereCollider&) = delete;
		ComponentSphereCollider& operator=(const ComponentSphereCollider&) = delete;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			glm::vec3& p_pOffset,
			float& p_fRadius
		);

		glm::vec3 m_pOffset;
		float m_fRadius;
	};
}

#endif // COMPONENT_SPHERE_COLLIDER