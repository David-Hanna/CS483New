//----------------------------------------------------------------------------
// ComponentObjectSpawn.h
// Author: David MacIntosh
//
// Simple component that spawns an object when initialized
//----------------------------------------------------------------------------

#ifndef COMPONENT_OBJECT_SPAWN_H
#define COMPONENT_OBJECT_SPAWN_H

#include <tinyxml2.h>

#include "Component.h"
#include "GameObject.h"

namespace Kartaclysm
{
	class ComponentObjectSpawn : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_ObjectSpawn"; }
		virtual const std::string ComponentID() const override	{ return "GOC_ObjectSpawn"; }

		// Destruction.
		virtual ~ComponentObjectSpawn();

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
		ComponentObjectSpawn(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strObjectXML
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strObjectXML;

		bool m_bCreated;
	};
}

#endif // COMPONENT_OBJECT_SPAWN_H