//----------------------------------------------------------------------------
// ComponentKartStats
// Author: David MacIntosh
//
// Component for the base stats of a kart
//----------------------------------------------------------------------------

#ifndef COMPONENT_KART_STATS_H
#define COMPONENT_KART_STATS_H

#include <tinyxml2.h>

#include "Component.h"
#include "GameObject.h"

namespace Kartaclysm
{
	class ComponentKartStats : public HeatStroke::Component
	{
	public:
		virtual const std::string FamilyID() const override { return "GOC_KartStats"; }
		virtual const std::string ComponentID() const override { return "GOC_KartStats"; }

		// Destruction.
		virtual ~ComponentKartStats();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override;
		virtual void Update(const float p_fDelta) override {};

		int GetMaxSpeed() { return m_iMaxSpeedStat; }
		int GetAcceleration() { return m_iAccelerationStat; }
		int GetHandling() { return m_iHandlingStat; }
		int GetDurability() { return m_iDurabilityStat; }

	protected:
		ComponentKartStats(
			HeatStroke::GameObject* p_pGameObject,
			int p_iMaxSpeed,
			int p_iAcceleration,
			int p_iHandling,
			int p_iDurability);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			int& p_iMaxSpeed,
			int& p_iAcceleration,
			int& p_iHandling,
			int& p_iDurability
			);

	private:
		int m_iMaxSpeedStat;
		int m_iAccelerationStat;
		int m_iHandlingStat;
		int m_iDurabilityStat;
	};
}

#endif // COMPONENT_KART_STATS_H