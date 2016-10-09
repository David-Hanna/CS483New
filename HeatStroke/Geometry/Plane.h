//=================================================================================
// Plane
// Author: David Hanna
//
// Represents a geometrical plane for a variety of uses.
//=================================================================================

#ifndef PLANE_H
#define PLANE_H

#include <glm.hpp>

#include "Collidable.h"

namespace HeatStroke
{
	class Plane : public Collidable
	{
	public:
		Plane()
			:
			m_vUnitNormal(0.0f, 1.0f, 0.0f),
			m_vDistanceToOrigin(0.0f)
		{}

		Plane(const float p_fA, const float p_fB, const float p_fC, const float p_fD)
			:
			a(p_fA),
			b(p_fB),
			c(p_fC),
			d(p_fD)
			//m_vUnitNormal(glm::normalize(glm::vec3(p_fA, p_fB, p_fC))),
			//m_vDistanceToOrigin(p_fD / glm::length(glm::vec3(p_fA, p_fB, p_fC)))
		{}

		Plane(const glm::vec3& p_vPoint, const glm::vec3& p_vNormal)
			:
			m_vUnitNormal(glm::normalize(p_vNormal)),
			m_vDistanceToOrigin(ComputeDistanceToOrigin(p_vPoint))
		{}

		~Plane() {}

		const glm::vec3& GetUnitNormal() const			{ return m_vUnitNormal; }
		float GetDistanceToOrigin() const				{ return m_vDistanceToOrigin; }

		void SetUnitNormal(const glm::vec3& p_vNormal)	{ m_vUnitNormal = glm::normalize(p_vNormal); }
		void SetPoint(const glm::vec3& p_vPoint)		{ m_vDistanceToOrigin = ComputeDistanceToOrigin(p_vPoint); }

		float a, b, c, d;
		void Normalize()
		{
			float length = sqrtf((a * a) + (b * b) + (c * c));
			a /= length;
			b /= length;
			c /= length;
			d /= length;
		}

	private:
		glm::vec3 m_vUnitNormal;
		float m_vDistanceToOrigin;

		float ComputeDistanceToOrigin(const glm::vec3& p_vPoint)
		{
			return -glm::dot(m_vUnitNormal, p_vPoint);
		}
	};
}

#endif // PLANE_H