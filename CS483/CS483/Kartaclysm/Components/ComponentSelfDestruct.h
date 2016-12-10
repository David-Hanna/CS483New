//----------------------------------------------------------------------------
// ComponentSelfDestruct.h
// Author: Bradley Cooper
//
// Component that destroys the game object after a time period or on collision.
//----------------------------------------------------------------------------

#ifndef COMPONENT_SELF_DESTRUCT_H
#define COMPONENT_SELF_DESTRUCT_H

#include <tinyxml2.h>
#include <boost/algorithm/string.hpp>
#include <vector>

#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentSelfDestruct : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_SelfDestruct"; }
		virtual const std::string ComponentID() const override	{ return "GOC_SelfDestruct"; }

		// Destruction.
		virtual ~ComponentSelfDestruct();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentSelfDestruct(
			HeatStroke::GameObject* p_pGameObject,
			float p_fTimer,
			const std::vector<std::string>& p_vCollisionTags
			);

		void HandleCollisionEvent(const HeatStroke::Event* p_pEvent);
		void SelfDestruct();

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& p_fTimer,
			std::vector<std::string>& p_vCollisionTags
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		bool m_bDestroy;
		float m_fTimer;
		std::vector<std::string> m_vCollisionTags;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
	};
}

#endif // COMPONENT_SELF_DESTRUCT_H