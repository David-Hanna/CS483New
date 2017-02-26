#ifndef PARTICLE_H
#define PARTICLE_H

#include "Common.h"

namespace HeatStroke
{
	struct Particle
	{
		Particle* m_pNext;
		Particle* m_pPrev;
		glm::vec3 m_vPos;
		glm::vec3 m_vVelocity;
		glm::vec3 m_vColor;
		glm::vec3 m_vColorStart;
		float m_fFade;
		float m_fFadeStart;
		float m_fSize;
		float m_fSizeStart;
		float m_fAge;
		float m_fAgeFactor;
	};
}

#endif