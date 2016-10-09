//=================================================================================
// AABB
// Author: David Hanna
//
// Represents a cuboid which can not rotate.
//=================================================================================

#ifndef AABB_H
#define AABB_H

#include <glm.hpp>

#include "Collidable.h"

namespace HeatStroke
{
	class AABB : public Collidable
	{
	public:
		AABB() : m_vCenter(0.0f, 0.0f, 0.0f), m_fWidth(1.0f), m_fHeight(1.0f), m_fDepth(1.0f) {}

		AABB(
			const glm::vec3& p_vCenter,
			const float p_fWidth,
			const float p_fHeight,
			const float p_fDepth
		) :
			m_vCenter(p_vCenter),
			m_fWidth(p_fWidth),
			m_fHeight(p_fHeight),
			m_fDepth(p_fDepth)
		{
			assert(m_fWidth >= 0.0f && m_fHeight >= 0.0f && m_fDepth >= 0.0f);
		}

		AABB(
			const float p_fMinX,
			const float p_fMaxX,
			const float p_fMinY,
			const float p_fMaxY,
			const float p_fMinZ,
			const float p_fMaxZ
			)
			:
			m_vCenter((p_fMinX + p_fMaxX) / 2.0f, (p_fMinY + p_fMaxY) / 2.0f, (p_fMinZ + p_fMaxZ) / 2.0f),
			m_fWidth(p_fMaxX - p_fMinX),
			m_fHeight(p_fMaxY - p_fMinY),
			m_fDepth(p_fMaxZ - p_fMinZ)
		{
			assert(m_fWidth >= 0.0f && m_fHeight >= 0.0f && m_fDepth >= 0.0f);
		}

		const glm::vec3& GetCenter() const { return m_vCenter; }
		float GetWidth() const { return m_fWidth; }
		float GetHeight() const { return m_fHeight; }
		float GetDepth() const { return m_fDepth; }

		float GetMinX() const { return m_vCenter.x - (m_fWidth / 2.0f); }
		float GetMidX() const { return m_vCenter.x; }
		float GetMaxX() const { return m_vCenter.x + (m_fWidth / 2.0f); }

		float GetMinY() const { return m_vCenter.y - (m_fHeight / 2.0f); }
		float GetMidY() const { return m_vCenter.y; }
		float GetMaxY() const { return m_vCenter.y + (m_fHeight / 2.0f); }

		float GetMinZ() const { return m_vCenter.z - (m_fDepth / 2.0f); }
		float GetMidZ() const { return m_vCenter.z; }
		float GetMaxZ() const { return m_vCenter.z + (m_fDepth / 2.0f); }

		float SquaredDistanceFromPointToClosestPointOnAABB(const glm::vec3& p_vPoint) const;

	private:
		glm::vec3 m_vCenter;
		float m_fWidth;
		float m_fHeight;
		float m_fDepth;

		float AxisCheck(
			const float p_fPointAxisValue,
			const float p_fAABBAxisMin,
			const float p_fAABBAxisMax
		) const;
	};
}

#endif // AABB_H