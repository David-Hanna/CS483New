//------------------------------------------------------------------------
// Component
// Author: David Hanna
//	
// This class contains the minimum interface that all game components
// must adhere to in order to be added to a GameObject.
//------------------------------------------------------------------------

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

namespace HeatStroke
{
    class GameObject;
    
	class Component
	{
	public:
		virtual const std::string FamilyID() const = 0;
		virtual const std::string ComponentID() const = 0;

		// Construction and Destruction
		Component(GameObject* p_pGameObject) : m_pGameObject(p_pGameObject) {}
		virtual ~Component() {}

		// Game Loop Methods
		virtual void Init() = 0;
		virtual void Update(const float p_fDelta) = 0;

		// Game Object accessors
		GameObject* GetGameObject() { return m_pGameObject; }
		void SetGameObject(GameObject* p_pGameObject)	{ m_pGameObject = p_pGameObject; }

	protected:
		// GameObject that owns this Component
		GameObject* m_pGameObject;
	};
}

#endif // COMPONENT_H