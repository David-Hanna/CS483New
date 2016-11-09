//------------------------------------------------------------------------
// ComponentCollider
// Author:	David MacIntosh
//	
// Base class for collider components to inherit from
//------------------------------------------------------------------------

#ifndef COMPONENT_COLLIDER
#define COMPONENT_COLLIDER

#include "Component.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ComponentCollider : public Component
	{
	public:
		virtual const std::string FamilyID() const override { return std::string("GOC_Collider"); }
		virtual const std::string ComponentID() const override { return std::string("GOC_Collider"); }

		//~Component3DModel();

		/*static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);*/

		//ComponentCollider(GameObject* p_pGameObject) : m_pGameObject(p_pGameObject) {}
		//virtual ~ComponentCollider() {}

		/*virtual void Init() override;
		virtual void Update(const float p_fDelta) override;*/
		virtual void Init() = 0;
		virtual void Update(const float p_fDelta) = 0;

		//virtual void PreRender() override { ComponentRenderable::PreRender(); }

	protected:
		// Prevent public construction.
		ComponentCollider(GameObject* p_pGameObject) : Component(p_pGameObject) {}

		//Component3DModel(GameObject* p_pGameObject, const std::string& p_strOBJFileName);

		//virtual void SyncTransform();

	private:
		// Prevent copying
		/*ComponentCollider(const ComponentCollider&) = delete;
		ComponentCollider& operator=(const ComponentCollider&) = delete;*/

		/*static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strOBJFileName
			);*/
	};
}

#endif // COMPONENT_COLLIDER