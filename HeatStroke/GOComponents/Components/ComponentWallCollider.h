//------------------------------------------------------------------------
// ComponentWallCollider
// Author:	David MacIntosh
//	
// Collider that takes the shape of a wall
//------------------------------------------------------------------------

#ifndef COMPONENT_WALL_COLLIDER_H
#define COMPONENT_WALL_COLLIDER_H

#include "ComponentCollider.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"
#include "LineDrawer.h"

namespace HeatStroke
{
	class ComponentWallCollider : public ComponentCollider
	{
	public:
		virtual const std::string ComponentID() const override { return std::string("GOC_WallCollider"); }

		~ComponentWallCollider();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;
		virtual void PreRender() override;

		glm::vec3 GetOffset() { return m_pOffset; }
		glm::vec3 GetSurfaceNormal() { return m_pSurfaceNormal; }
		float GetHeight() { return m_fHeight; }
		float GetWidth() { return m_fWidth; }

	protected:
		ComponentWallCollider(
			GameObject* p_pGameObject,
			glm::vec3 p_pOffset,
			glm::vec3 p_pSurfaceNormal,
			float p_fHeight,
			float p_fWidth
			);

		virtual void SyncTransform();

		bool m_bDebugRender;
		LineDrawer* m_pDebugLineDrawer;

	private:
		// Prevent copying
		ComponentWallCollider(const ComponentWallCollider&) = delete;
		ComponentWallCollider& operator=(const ComponentWallCollider&) = delete;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			glm::vec3& p_pOffset,
			glm::vec3& p_pSurfaceNormal,
			float& p_fHeight,
			float& p_fWidth
			);

		glm::vec3 m_pOffset;
		glm::vec3 m_pSurfaceNormal;
		float m_fHeight;
		float m_fWidth;
	};
}

#endif // COMPONENT_WALL_COLLIDER_H