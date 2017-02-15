#ifndef COMPONENT_PARTICLE_EFFECT_H
#define COMPONENT_PARTICLE_EFFECT_H

#include "Component.h"

#include <map>
#include <vector>

#include "EasyXML.h"
#include "Effect.h"
#include "SceneManager.h"

namespace HeatStroke
{
	class ComponentParticleEffect : Component
	{
		public:
			virtual const std::string ComponentID() const override {return "GOC_ParticleEffect";}
			virtual const std::string FamilyID() const override { return "GOC_ParticleEffect";}
			~ComponentParticleEffect();

			static Component* CreateComponent(GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode);

			virtual void Init() override;
			virtual void Update(const float p_fDelta) override;

			void Start();
			void Stop();

			Effect* GetEffect(const std::string& p_strEffectId);

		private:
			std::map<std::string, Effect*> m_pEffects;
			
			ComponentParticleEffect(GameObject* p_pGameObject, const std::vector<std::pair<std::string, std::string>>& p_vEffectDefinitionFile, const glm::vec3& p_vOffset = {0.0f, 0.0f, 0.0f});

			static std::pair<std::string, std::string> ParseEffectDefintionFile(const tinyxml2::XMLElement* p_pEffectElement);
			static glm::vec3 ParseEffectOffset(const tinyxml2::XMLElement* p_pOffsetElement);
	};
}

#endif