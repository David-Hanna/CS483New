//----------------------------------------------------------------------------
// ComponentProjectile.cpp
// Author: Bradley Cooper
//
// Component that handles projectile collisions and events.
//----------------------------------------------------------------------------

#ifndef COMPONENT_PROJECTILE_H
#define COMPONENT_PROJECTILE_H

#include <tinyxml2.h>

#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentProjectile : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_Projectile"; }
		virtual const std::string ComponentID() const override	{ return "GOC_Projectile"; }

		// Destruction.
		virtual ~ComponentProjectile();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override {}

		// Set projectile information
		void SetOriginator(const std::string& p_strOriginator) { m_strOriginator = p_strOriginator; }
		void SetOnHitEvent(const std::string& p_strOnHitEvent) { m_strOnHitEvent = p_strOnHitEvent; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentProjectile(
			HeatStroke::GameObject* p_pGameObject
			);

		void HandleCollisionEvent(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strOriginator;
		std::string m_strOnHitEvent;

	private:
		std::function<void(const HeatStroke::Event*)>* m_pCollisionDelegate;
	};
}

#endif // COMPONENT_PROJECTILE_H