//----------------------------------------------------------------------------
// ComponentAIDriver.h
// Author: David MacIntosh
//
// Component that uses AI to control a ComponentKartController
//----------------------------------------------------------------------------

#ifndef COMPONENT_AI_DRIVER_H
#define COMPONENT_AI_DRIVER_H

#include <tinyxml2.h>

#include "Common.h"
#include "Component.h"
#include "ComponentTrack.h"
#include "GameObject.h"

#include "ComponentAbility.h"

namespace Kartaclysm
{
	class ComponentAIDriver : public HeatStroke::Component
	{
	public:
		virtual const std::string FamilyID() const override		{ return "GOC_AIDriver"; }
		virtual const std::string ComponentID() const override	{ return "GOC_AIDriver"; }

		// Destruction.
		virtual ~ComponentAIDriver();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override;

		void QueryPlayerMovement(
			const int p_iPlayerNum,
			int& p_iAccelerate,
			int& p_iBrake,
			int& p_iSlide,
			float& p_fTurn) const;

		void SetNode(ComponentTrack::PathNode p_sNode);

		// Ability use
		void RegisterComponentAbility(ComponentAbility* p_pAbility);
		float AngleToNextNode() { return m_fAngleToNextNode; }
		float DistanceToNextNode() { return m_fDistanceToNextNode; }
		int CurrentPosition();
		bool IsInWheelie();
		float DistanceToClosestOpponent();

	protected:
		ComponentAIDriver(
			HeatStroke::GameObject* p_pGameObject
			);

		HeatStroke::GameObject* m_pGameObject;

	private:
		std::vector<ComponentAbility*> m_vAbilities;

		ComponentTrack::PathNode m_sCurrentNode;
		float m_fXTarget;
		float m_fZTarget;

		glm::vec3 m_vInitialNodeDir;

		int m_iAccelerate;
		int m_iBrake;
		float m_fTurn;
		int m_iSlide;
		int m_iSlideDir;

		void NextNode();

		float m_fAngleToNextNode;
		float m_fDistanceToNextNode;
	};
}

#endif // COMPONENT_AI_DRIVER_H