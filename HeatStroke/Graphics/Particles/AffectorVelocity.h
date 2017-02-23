#ifndef AFFECTOR_VELOCITY_H
#define AFFECTOR_VELOCITY_H

#include "Affector.h"

namespace HeatStroke
{
	class AffectorVelocity : public Affector
	{
		public:
		AffectorVelocity();
		~AffectorVelocity();
		virtual void Apply(const float p_fDelta, Particle* p) const;

		private:

	};
}

#endif