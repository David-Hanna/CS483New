#ifndef EFFECT_H
#define EFFECT_H

#include "Emitter.h"
#include "Common.h"
#include "EasyXML.h"
#include "HierarchicalTransform.h"
#include "SceneCamera.h"

namespace HeatStroke
{
	class Effect
	{
		public:
			Effect(const std::string& p_strDefinitionFile);
			~Effect();

			void Update(const float p_fDelta);
			void Render(const SceneCamera* p_pCamera);

			void Start();
			void Stop();

			// TEMP - make private
			HierarchicalTransform m_Transform;
		private:
			std::vector<Emitter*> m_vEmitters;

			void ParseEmitters(tinyxml2::XMLElement* p_pEffectElement);
			static std::string ParseEmitterDefinitionFile(const tinyxml2::XMLElement* p_pEmitterElement);
			static glm::vec3 ParseEmitterOffset(const tinyxml2::XMLElement* p_pOffsetElement);
	};
}

#endif