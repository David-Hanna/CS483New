//=================================================================================
// Frustum
// Author: David Hanna
//
// Represents the geometry of a frustum, typically used with cameras and culling.
//=================================================================================

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm.hpp>
#include <vector>

#include "Plane.h"
#include "Collidable.h"

namespace HeatStroke
{
	class Frustum : public Collidable
	{
	public:
		Frustum() : m_vPlanes(6)
		{
			m_vPlanes[0] = Plane(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			m_vPlanes[1] = Plane(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			m_vPlanes[2] = Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			m_vPlanes[3] = Plane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			m_vPlanes[4] = Plane(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			m_vPlanes[5] = Plane(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		Frustum(
			const Plane& p_mLeftPlane,
			const Plane& p_mRightPlane,
			const Plane& p_mBottomPlane,
			const Plane& p_mTopPlane,
			const Plane& p_mNearPlane,
			const Plane& p_mFarPlane)
			:
			m_vPlanes(6)
		{
			m_vPlanes[0] = p_mLeftPlane;
			m_vPlanes[1] = p_mRightPlane;
			m_vPlanes[2] = p_mBottomPlane;
			m_vPlanes[3] = p_mTopPlane;
			m_vPlanes[4] = p_mNearPlane;
			m_vPlanes[5] = p_mFarPlane;
		}

		Frustum(const glm::mat4& p_mTransform)
			:
			m_vPlanes(6)
		{
			// Left clipping plane
			m_vPlanes[0] = Plane(
				p_mTransform[0][3] + p_mTransform[0][0],
				p_mTransform[1][3] + p_mTransform[1][0],
				p_mTransform[2][3] + p_mTransform[2][0],
				p_mTransform[3][3] + p_mTransform[3][0]
			);
			// Right clipping plane
			m_vPlanes[1] = Plane(
				p_mTransform[0][3] - p_mTransform[0][0],
				p_mTransform[1][3] - p_mTransform[1][0],
				p_mTransform[2][3] - p_mTransform[2][0],
				p_mTransform[3][3] - p_mTransform[3][0]
			);
			// Bottom clipping plane
				m_vPlanes[2] = Plane(
				p_mTransform[0][3] + p_mTransform[0][1],
				p_mTransform[1][3] + p_mTransform[1][1],
				p_mTransform[2][3] + p_mTransform[2][1],
				p_mTransform[3][3] + p_mTransform[3][1]
			);
			// Top clipping plane
			m_vPlanes[3] = Plane(
				p_mTransform[0][3] - p_mTransform[0][1],
				p_mTransform[1][3] - p_mTransform[1][1],
				p_mTransform[2][3] - p_mTransform[2][1],
				p_mTransform[3][3] - p_mTransform[3][1]
			);
			// Near clipping plane
			m_vPlanes[4] = Plane(
				p_mTransform[0][3] + p_mTransform[0][2],
				p_mTransform[1][3] + p_mTransform[1][2],
				p_mTransform[2][3] + p_mTransform[2][2],
				p_mTransform[3][3] + p_mTransform[3][2]
				);
			// Far clipping plane
			m_vPlanes[5] = Plane(
				p_mTransform[0][3] - p_mTransform[0][2],
				p_mTransform[1][3] - p_mTransform[1][2],
				p_mTransform[2][3] - p_mTransform[2][2],
				p_mTransform[3][3] - p_mTransform[3][2]
			);

			for (unsigned int i = 0; i < 6; i++)
			{
				m_vPlanes[i].Normalize();
			}
		}

		~Frustum() {}

		const Plane& GetLeftPlane() const				{ return m_vPlanes[0]; }
		const Plane& GetRightPlane() const				{ return m_vPlanes[1]; }
		const Plane& GetBottomPlane() const				{ return m_vPlanes[2]; }
		const Plane& GetTopPlane() const				{ return m_vPlanes[3]; }
		const Plane& GetNearPlane() const				{ return m_vPlanes[4]; }
		const Plane& GetFarPlane() const				{ return m_vPlanes[5]; }

		void SetLeftPlane(const Plane& p_mLeftPlane)	{ m_vPlanes[0] = p_mLeftPlane; }
		void SetRightPlane(const Plane& p_mRightPlane)	{ m_vPlanes[1] = p_mRightPlane; }
		void SetBottomPlane(const Plane& p_mBottomPlane) { m_vPlanes[2] = p_mBottomPlane; }
		void SetTopPlane(const Plane& p_mTopPlane)		{ m_vPlanes[3] = p_mTopPlane; }
		void SetNearPlane(const Plane& p_mNearPlane)	{ m_vPlanes[4] = p_mNearPlane; }
		void SetFarPlane(const Plane& p_mFarPlane)		{ m_vPlanes[5] = p_mFarPlane; }

		std::vector<Plane>::iterator begin()			{ return m_vPlanes.begin(); }
		std::vector<Plane>::iterator end()				{ return m_vPlanes.end(); }
		
		std::vector<Plane>::const_iterator begin() const { return m_vPlanes.begin(); }
		std::vector<Plane>::const_iterator end() const	{ return m_vPlanes.end(); }

	private:
		std::vector<Plane> m_vPlanes;
	};
}


#endif // FRUSTUM_H