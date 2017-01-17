//------------------------------------------------------------------------
// ComponentCollider
// Author:	David MacIntosh
//	
// Base class for collider components to inherit from
//------------------------------------------------------------------------

#ifndef COMPONENT_COLLIDER
#define COMPONENT_COLLIDER

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"
#include "LineDrawer.h"

namespace HeatStroke
{
	class ComponentCollider : public ComponentRenderable
	{
	public:
		virtual const std::string FamilyID() const override { return std::string("GOC_Collider"); }

		virtual void Init() = 0;
		virtual void Update(const float p_fDelta) = 0;
		virtual void PreRender() = 0;

		bool HasPhysics() const { return m_bAppliesPhysics; }

		glm::vec3 GetPosition() { return m_pPosition; }
		glm::vec3 GetPreviousPosition() { return m_pPreviousPosition; }

	protected:
		// Prevent public construction.
		ComponentCollider(GameObject* p_pGameObject, bool p_bAppliesPhysics, bool p_bDebugRender = false) :
			ComponentRenderable(p_pGameObject),
			m_bAppliesPhysics(p_bAppliesPhysics),
			m_bDebugRender(p_bDebugRender),
			m_pDebugLineDrawer(nullptr) {}

		virtual void SyncTransform() = 0;

		glm::vec3 m_pPosition;
		glm::vec3 m_pPreviousPosition;

		bool m_bDebugRender;
		LineDrawer* m_pDebugLineDrawer;

	private:
		// Prevent copying
		ComponentCollider(const ComponentCollider&) = delete;
		ComponentCollider& operator=(const ComponentCollider&) = delete;

		bool m_bAppliesPhysics;
	};
}

#endif // COMPONENT_COLLIDER
