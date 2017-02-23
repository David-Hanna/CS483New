#ifndef AFFECTOR_COLOR_H
#define AFFECTOR_COLOR_H

#include "Affector.h"

namespace HeatStroke
{
	class AffectorColor : public Affector
	{
		public:
			AffectorColor(const AFFECTOR_FUNCTION p_eFunction, const glm::vec3& p_vColor);
			~AffectorColor();
			virtual void Apply(const float p_fDelta, Particle* p) const;

		private:
			glm::vec3 m_vColor;
			AFFECTOR_FUNCTION m_eFunction;
	};
}

#endif