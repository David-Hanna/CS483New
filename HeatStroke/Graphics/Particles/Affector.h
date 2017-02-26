#ifndef AFFECTOR_H
#define AFFECTOR_H

#include "Particle.h"

namespace HeatStroke
{
	class Affector
	{
		public:
			enum AFFECTOR_FUNCTION
			{
				AF_CONSTANT,
				AF_LIFETIME	
			};

			Affector() {}
			~Affector() {}
			virtual void Apply(const float p_fDelta, Particle* p) const = 0;

		protected:
	};
}

#endif