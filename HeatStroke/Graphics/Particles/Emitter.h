#ifndef EMITTER_H
#define EMITTER_H

#include <random>

#include "Affector.h"
#include "Particle.h"
#include "Common.h"
#include "RNGesus.h"
#include "EasyXML.h"
#include "HierarchicalTransform.h"
#include "SceneCamera.h"
#include "BufferManager.h"
#include "MaterialManager.h"
#include "VertexDeclaration.h"

namespace HeatStroke
{
	class Emitter
	{
		public:
			Emitter(const std::string& p_strDefinitionFile);
			~Emitter();

			void Update(const float p_fDelta);
			void Render(const SceneCamera* p_pCamera);

			void Start();
			void Stop();

			// TEMP - make private
			HierarchicalTransform m_Transform;

		private:
			struct Vertex
			{
				glm::vec3 vPos;
				glm::vec2 vUV;
				glm::vec4 vColor;
			};

			bool m_bActive;
			bool m_bContinuous;
			bool m_bRandomBurstTimer;
			bool m_bRepeatBursts;
			int m_iNumParticles;
			int m_iNumActiveParticles;
			float m_fBirthRate;
			float m_fBurstRateMin;
			float m_fBurstRateMax;
			float m_fTimeToBurst;
			float m_fDuration;
			float m_fToSpawnAcc;

			// TODO - consider creating ParticlePool class
			Particle* m_pFreeList;
			Particle* m_pActiveHead;
			Particle* m_pActiveTail;
			Vertex* m_pVerts;
			GLuint* m_pIndices;
			std::vector<Affector*> m_vAffectors;

			// spawn properties
			// TODO - move into separate SpawnProperties class
			bool m_bRandomFade;
			bool m_bRandomLifetime;
			bool m_bRandomSize;
			bool m_bRandomColor;
			bool m_bRandomVelocity;
			float m_fFadeMin;
			float m_fFadeMax;
			float m_fLifetimeMin;
			float m_fLifetimeMax;
			float m_fSizeMin;
			float m_fSizeMax;
			glm::vec3 m_vColorMin;
			glm::vec3 m_vColorMax;
			glm::vec3 m_vVelocityMin;
			glm::vec3 m_vVelocityMax;

			// render properties
			HeatStroke::VertexBuffer* m_pVB;
			HeatStroke::IndexBuffer* m_pIB;
			HeatStroke::VertexDeclaration* m_pDecl;
			HeatStroke::Material* m_pMat;

			void InitFrequencyProperties(tinyxml2::XMLElement* p_pFrequencyPropertiesElement);
			void InitRenderProperties(tinyxml2::XMLElement* p_pRenderPropertiesElement);
			void InitBuffers();
			void InitVertexDeclaration();
			void InitMaterial(tinyxml2::XMLElement* p_pMaterialElement);

			// TODO - move into separate SpawnProperties class
			void InitSpawnProperties(tinyxml2::XMLElement* p_pSpawnPropertiesElement);
			void InitSpawnPropertyColor(tinyxml2::XMLElement* p_pSpawnPropertyColorElement);
			void InitSpawnPropertyFade(tinyxml2::XMLElement* p_pSpawnPropertyFadeElement);
			void InitSpawnPropertyLifetime(tinyxml2::XMLElement* p_pSpawnPropertyLifetimeElement);
			void InitSpawnPropertySize(tinyxml2::XMLElement* p_pSpawnPropertySizeElement);
			void InitSpawnPropertyVelocity(tinyxml2::XMLElement* p_pSpawnPropertyVelocityElement);

			// TODO - consider factory pattern for Affector creation
			void InitAffectors(tinyxml2::XMLElement* p_pAffectorsElement);
			void InitAffectorColor(tinyxml2::XMLElement* p_pAffectorColorElement);
			void InitAffectorFade(tinyxml2::XMLElement* p_pAffectorFadeElement);
			void InitAffectorScale(tinyxml2::XMLElement* p_pAffectorSizeElement);
			void InitAffectorVelocity();
			
			void InitParticles();
			void AddParticleToPool(Particle* p_pParticle);

			void SpawnNewParticles(const float p_fDelta);
			void UpdateActiveParticles(const float p_fDelta);

			// TODO - if ParticlePool class is created, move to there
			void SpawnParticle();
			Particle* GetFreeParticle();
			void AddToActive(Particle* p_pParticle);
			void KillParticle(Particle* p_pParticle);
			void RemoveFromActive(Particle* p_pParticle);

			// TODO - move to separate SpawnProperties class
			void ApplySpawnProperties(Particle* p_pParticle);
			void ApplySpawnPropertyColor(Particle* p_pParticle);
			void ApplySpawnPropertyFade(Particle* p_pParticle);
			void ApplySpawnPropertyLifetime(Particle* p_pParticle);
			void ApplySpawnPropertySize(Particle* p_pParticle);
			void ApplySpawnPropertyVelocity(Particle* p_pParticle);

			void GetTimeToNextBurst();
			void SetRandomPosition(Particle* p_pParticle);
	};
}

#endif