//------------------------------------------------------------------------
// ComponentRotateOverTime
// Author:	David Hanna
//	
// Provides a rotation over time behaviour to an object.
//------------------------------------------------------------------------

#ifndef COMPONENT_ROTATE_OVER_TIME_H
#define COMPONENT_ROTATE_OVER_TIME_H

#include "ComponentRenderable.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ComponentRotateOverTime : public Component
	{
	public:
		virtual const std::string FamilyID() const override { return std::string("GOC_RotateOverTime"); }
		virtual const std::string ComponentID() const override { return std::string("GOC_RotateOverTime"); }

		virtual ~ComponentRotateOverTime()	{}

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
		);

		virtual void Init() override						{}
		virtual void Update(const float p_fDelta) override;

	protected:
		ComponentRotateOverTime(
			GameObject* p_pGameObject, 
			float p_fRotationPerSecondX,
			float p_fRotationPerSecondY,
			float p_fRotationPerSecondZ
		);

	private:
		float m_fRotationPerSecondX;
		float m_fRotationPerSecondY;
		float m_fRotationPerSecondZ;

		// Prevent copying
		ComponentRotateOverTime(const ComponentRotateOverTime&) = delete;
		ComponentRotateOverTime& operator=(const ComponentRotateOverTime&) = delete;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			float& p_fRotationPerSecondX,
			float& p_fRotationPerSecondY,
			float& p_fRotationPerSecondZ
		);
	};
}

#endif