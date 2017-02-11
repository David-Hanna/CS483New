#ifndef COMPONENT_KART_CONTROLLER_AI_H
#define COMPONENT_KART_CONTROLLER_AI_H

#include "Component.h"

#include "Common.h"
#include "ComponentRacer.h"
#include "ComponentTrack.h"
#include "EasyXML.h"

namespace Kartaclysm
{
	class ComponentKartControllerAI : public HeatStroke::Component
	{
		public:
			virtual const std::string FamilyID() const override		{ return "GOC_KartController"; }
			virtual const std::string ComponentID() const override	{ return "GOC_KartControllerAI"; }

			virtual ~ComponentKartControllerAI();

			static HeatStroke::Component* CreateComponent(
				HeatStroke::GameObject* p_pGameObject,
				tinyxml2::XMLNode* p_pBaseNode,
				tinyxml2::XMLNode* p_pOverrideNode);

			virtual void Init() override {}
			virtual void Update(const float p_fDelta) override;

		protected:
			ComponentKartControllerAI(HeatStroke::GameObject* p_pGameObject);

		private:
			glm::vec3 m_vGoalPos;
	};
}

#endif