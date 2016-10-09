//=================================================================================
// Collidable
// Author: David Hanna
//
// Interface for having collision detection with other Collidables.
//=================================================================================

#ifndef COLLIDABLE_H
#define COLLIDABLE_H

namespace HeatStroke
{
	class Collidable
	{
	public:
		virtual ~Collidable() = 0 {}
	};
}

#endif // COLLIDABLE_H