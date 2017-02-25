#ifndef AAFFECTOR_FADE_H
#define AAFFECTOR_FADE_H

#include "Affector.h"

namespace HeatStroke
{
	class AffectorFade : public Affector
	{
		public:
			AffectorFade(const AFFECTOR_FUNCTION p_eFunction, float p_fFade);
			~AffectorFade();
			virtual void Apply(const float p_fDelta, Particle* p) const;

		private:
			float m_fFade;
			AFFECTOR_FUNCTION m_eFunction;
	};
}

#endif