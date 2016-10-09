//=================================================================================
// Collisions
// Author: David Hanna
//
// A collection of functions that test for collisions between shapes.
//=================================================================================

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "Plane.h"
#include "Sphere.h"
#include "Frustum.h"
#include "AABB.h"

namespace HeatStroke
{
	enum COLLISION_TEST_RESULT
	{
		CTR_A_INSIDE_B = 0,
		CTR_B_INSIDE_A,
		CTR_INTERSECTING,
		CTR_OUTSIDE			// Outside has a meaning akin to: not intersecting and one is not inside the other
	};

	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const Plane& p_pB);
	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const Sphere& p_pB);
	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const Frustum& p_pB);
	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const AABB& p_pB);

	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const Plane& p_pB);
	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const Sphere& p_pB);
	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const Frustum& p_pB);
	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const AABB& p_pB);

	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const Plane& p_pB);
	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const Sphere& p_pB);
	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const Frustum& p_pB);
	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const AABB& p_pB);

	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const Plane& p_pB);
	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const Sphere& p_pB);
	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const Frustum& p_pB);
	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const AABB& p_pB);
}



#endif // COLLISIONS_H