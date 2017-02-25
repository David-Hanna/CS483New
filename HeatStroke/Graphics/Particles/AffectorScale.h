#ifndef AFFECTOR_SCALE_H
#define AFFECTOR_SCALE_H

#include "Affector.h"

namespace HeatStroke
{
	class AffectorScale : public Affector
	{
		public:
			AffectorScale(const AFFECTOR_FUNCTION p_eFunction, const float p_fSize);
			~AffectorScale();
			virtual void Apply(const float p_fDelta, Particle* p) const;

		private:
			float m_fSize;
			AFFECTOR_FUNCTION m_eFunction;
	};
}

#endif