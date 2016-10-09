//=================================================================================
// Collisions
// Author: David Hanna
//
// A collection of functions that test for collisions between subclasses
// of IVolume.
//=================================================================================

#include "Collisions.h"

namespace HeatStroke
{
	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const Plane& p_pB)
	{
		assert(false && "Not Implemented");
		return CTR_OUTSIDE;
	}

	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const Sphere& p_pB)
	{
		float d = (p_pA.a * p_pB.GetCenter().x) + (p_pA.b * p_pB.GetCenter().y) + (p_pA.c * p_pB.GetCenter().z) + p_pA.d;

		if (d < -p_pB.GetRadius())
		{
			return CTR_OUTSIDE;
		}
		else if (glm::abs(d) <= p_pB.GetRadius())
		{
			return CTR_INTERSECTING;
		}
		else
		{
			return CTR_B_INSIDE_A;
		}
		// The plane can not be inside the sphere.
	}

	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const Frustum& p_pB)
	{
		assert(false && "Not Implemented");
		return CTR_OUTSIDE;
	}

	COLLISION_TEST_RESULT Collide(const Plane& p_pA, const AABB& p_pB)
	{
		assert(false && "Not Implemented");
		return CTR_OUTSIDE;
	}

	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const Plane& p_pB)
	{
		COLLISION_TEST_RESULT ctr = Collide(p_pB, p_pA);
		// Opposite results.
		if (ctr == CTR_B_INSIDE_A)
		{
			ctr = CTR_A_INSIDE_B;
		}
		return ctr;
	}

	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const Sphere& p_pB)
	{
		const float fRadiusA = p_pA.GetRadius();
		const float fRadiusB = p_pB.GetRadius();
		const float fDistance = glm::distance(p_pA.GetCenter(), p_pB.GetCenter());

		if (fDistance > fRadiusA + fRadiusB)
		{
			return CTR_OUTSIDE;
		}
		if (fDistance + fRadiusA < fRadiusB)
		{
			return CTR_A_INSIDE_B;
		}
		if (fDistance + fRadiusB < fRadiusA)
		{
			return CTR_B_INSIDE_A;
		}
		return CTR_OUTSIDE;
	}

	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const Frustum& p_pB)
	{
		COLLISION_TEST_RESULT ctr = CTR_A_INSIDE_B;

		std::vector<Plane>::const_iterator it = p_pB.begin(), end = p_pB.end();
		for (; it != end; it++)
		{
			COLLISION_TEST_RESULT plane_ctr = Collide(*it, p_pA);

			if (plane_ctr == CTR_OUTSIDE)
			{
				return CTR_OUTSIDE;
			}
			else if (plane_ctr == CTR_INTERSECTING)
			{
				ctr = CTR_INTERSECTING;
			}
		}

		return ctr;
	}

	COLLISION_TEST_RESULT Collide(const Sphere& p_pA, const AABB& p_pB)
	{
		const glm::vec3& vSphereCenter = p_pA.GetCenter();
		const float fSphereRadius = p_pA.GetRadius();

		const float fSphereMinX = vSphereCenter.x - fSphereRadius;
		const float fSphereMaxX = vSphereCenter.x + fSphereRadius;
		const float fSphereMinY = vSphereCenter.y - fSphereRadius;
		const float fSphereMaxY = vSphereCenter.y + fSphereRadius;
		const float fSphereMinZ = vSphereCenter.z - fSphereRadius;
		const float fSphereMaxZ = vSphereCenter.z + fSphereRadius;

		const float fAABBMinX = p_pB.GetMinX();
		const float fAABBMaxX = p_pB.GetMaxX();
		const float fAABBMinY = p_pB.GetMinY();
		const float fAABBMaxY = p_pB.GetMaxY();
		const float fAABBMinZ = p_pB.GetMinZ();
		const float fAABBMaxZ = p_pB.GetMaxZ();

		if
			(
				fSphereMinX >= fAABBMinX && fSphereMaxX <= fAABBMaxX
			&&  fSphereMinY >= fAABBMinY && fSphereMaxY <= fAABBMaxY
			&&	fSphereMinZ >= fAABBMinZ && fSphereMaxZ <= fAABBMaxZ
			)
		{
			return CTR_A_INSIDE_B;
		}
		else if
			(
				fAABBMinX >= fSphereMinX && fAABBMaxX <= fSphereMaxX
			&&	fAABBMinY >= fSphereMinY && fAABBMaxY <= fSphereMaxY
			&&	fAABBMinZ >= fSphereMinZ && fAABBMaxZ <= fSphereMaxZ
			)
		{
			return CTR_B_INSIDE_A;
		}
		else if
			(
			p_pB.SquaredDistanceFromPointToClosestPointOnAABB(vSphereCenter) <= fSphereRadius * fSphereRadius
			)
		{
			return CTR_INTERSECTING;
		}
		else
		{
			return CTR_OUTSIDE;
		}
	}

	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const Plane& p_pB)
	{
		assert(false && "Not Implemented");
		return CTR_OUTSIDE;
	}

	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const Sphere& p_pB)
	{
		COLLISION_TEST_RESULT ctr = Collide(p_pB, p_pA);
		// Opposite results.
		if (ctr == CTR_A_INSIDE_B)
		{
			ctr = CTR_B_INSIDE_A;
		}
		return ctr;
	}

	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const Frustum& p_pB)
	{
		assert(false && "Not Implemented");
		return CTR_OUTSIDE;
	}

	COLLISION_TEST_RESULT Collide(const Frustum& p_pA, const AABB& p_pB)
	{
		return CTR_INTERSECTING;
	}

	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const Plane& p_pB)
	{
		const float fAABBMaxX = p_pA.GetMaxX();
		const float fAABBMaxY = p_pA.GetMaxY();
		const float fAABBMaxZ = p_pA.GetMaxZ();
		const float fAABBDistanceToPoint = glm::distance(p_pA.GetCenter(), glm::vec3(fAABBMaxX, fAABBMaxY, fAABBMaxZ));

		glm::vec3 absNormal = glm::abs(p_pB.GetUnitNormal());
		float d = (absNormal.x * p_pA.GetCenter().x) + (absNormal.y * p_pA.GetCenter().y) + (absNormal.z * p_pA.GetCenter().z) + p_pB.d;

		if (d < -fAABBDistanceToPoint)
		{
			return CTR_OUTSIDE;
		}
		else if (glm::abs(d) <= fAABBDistanceToPoint)
		{
			return CTR_INTERSECTING;
		}
		else
		{
			return CTR_B_INSIDE_A;
		}
		// The plane can not be inside the AABB.
	}

	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const Sphere& p_pB)
	{
		COLLISION_TEST_RESULT ctr = Collide(p_pB, p_pA);
		if (ctr == CTR_A_INSIDE_B) ctr = CTR_B_INSIDE_A;
		else if (ctr == CTR_B_INSIDE_A) ctr = CTR_A_INSIDE_B;
		return ctr;
	}

	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const Frustum& p_pB)
	{
		COLLISION_TEST_RESULT ctr = CTR_B_INSIDE_A;

		std::vector<Plane>::const_iterator it = p_pB.begin(), end = p_pB.end();
		for (; it != end; it++)
		{
			COLLISION_TEST_RESULT plane_ctr = Collide(p_pA, *it);

			if (plane_ctr == CTR_OUTSIDE)
			{
				return CTR_OUTSIDE;
			}
			else if (plane_ctr == CTR_INTERSECTING)
			{
				ctr = CTR_INTERSECTING;
			}
		}

		return ctr;
	}

	COLLISION_TEST_RESULT Collide(const AABB& p_pA, const AABB& p_pB)
	{
		assert(false && "Not Implemented");
		return CTR_OUTSIDE;
	}
}