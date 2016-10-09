//=================================================================================
// AABB
// Author: David Hanna
//
// Represents a cuboid which can not rotate.
//=================================================================================

#include "AABB.h"

namespace HeatStroke
{
	float AABB::SquaredDistanceFromPointToClosestPointOnAABB(const glm::vec3& p_vPoint) const
	{
		float fSquareDistance = 0.0f;

		fSquareDistance += AxisCheck(p_vPoint.x, GetMinX(), GetMaxX());
		fSquareDistance += AxisCheck(p_vPoint.y, GetMinY(), GetMaxY());
		fSquareDistance += AxisCheck(p_vPoint.z, GetMinZ(), GetMaxZ());

		return fSquareDistance;
	}

	float AABB::AxisCheck(
		const float p_fPointAxisValue,
		const float p_fAABBAxisMin,
		const float p_fAABBAxisMax
	) const
	{
		float out = 0.0f;

		if (p_fPointAxisValue < p_fAABBAxisMin)
		{
			float fDifference = p_fAABBAxisMin - p_fPointAxisValue;
			out += fDifference * fDifference;
		}
		if (p_fPointAxisValue > p_fAABBAxisMax)
		{
			float fDifference = p_fPointAxisValue - p_fAABBAxisMax;
			out += fDifference * fDifference;
		}

		return out;
	}
}