//=======================================================================================
// Sphere
// Author: David Hanna
//
// A typical sphere class for a variety of uses.
//=======================================================================================

#ifndef SPHERE_H
#define SPHERE_H

#include <glm.hpp>
#include <gtc/constants.hpp>

#include "Collidable.h"

namespace HeatStroke
{
	class Sphere : public Collidable
	{
	public:
		Sphere()
			:
			m_fCenter(0.0f, 0.0f, 0.0f),
			m_fRadius(1.0f)
		{}

		Sphere(const glm::vec3& p_fCenter, const float p_fRadius)
			:
			m_fCenter(p_fCenter),
			m_fRadius(p_fRadius)
		{}

		virtual ~Sphere() {}

		void SetCenter(const glm::vec3& p_fCenter)								{ m_fCenter = p_fCenter; }
		void SetCenterXYZ(const float p_fX, const float p_fY, const float p_fZ) { SetCenter(glm::vec3(p_fX, p_fY, p_fZ)); }
		const glm::vec3& GetCenter() const										{ return m_fCenter; }

		void SetRadius(const float p_fRadius)									{ m_fRadius = p_fRadius; }
		float GetRadius() const													{ return m_fRadius; }

		virtual float GetVolume() const											{ return (4.0f / 3.0f) * glm::pi<float>() * m_fRadius * m_fRadius * m_fRadius; }

	private:
		glm::vec3 m_fCenter;
		float m_fRadius;
	};
}

#endif // SPHERE_H