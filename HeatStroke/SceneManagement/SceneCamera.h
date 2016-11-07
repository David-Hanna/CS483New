//------------------------------------------------------------------------
// SceneCamera
// Author: David Hanna
//	
// View/Projection matrices. Must be subclassed into ortho/perspective.
//------------------------------------------------------------------------

#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>

namespace HeatStroke
{
	class SceneCamera
	{
	public:
		SceneCamera();
		SceneCamera(const glm::vec3& p_vPosition, const glm::vec3& p_vTarget, const glm::vec3& p_vUp);

		virtual ~SceneCamera() = 0 {}

		virtual const glm::vec3& GetPosition()	const	{ return m_vPosition; }
		virtual const glm::vec3& GetTarget()	const	{ return m_vTarget; }
		virtual const glm::vec3& GetUp()		const	{ return m_vUp; }

		virtual const glm::mat4& GetViewMatrix()			const;
		virtual const glm::mat4& GetProjectionMatrix()		const;
		virtual const glm::mat4& GetViewProjectionMatrix()	const;

		virtual void SetPosition(const glm::vec3 &p_vPosition);
		virtual void SetTarget(const glm::vec3& p_vTarget);
		virtual void SetUp(const glm::vec3& p_vUp);

	protected:
		glm::vec3 m_vPosition;
		glm::vec3 m_vTarget;
		glm::vec3 m_vUp;

		mutable bool m_bViewDirty;
		mutable bool m_bProjectionDirty;

		mutable glm::mat4 m_mViewMatrix;
		mutable glm::mat4 m_mProjectionMatrix;
		mutable glm::mat4 m_mViewProjectionMatrix;

		virtual glm::mat4 ComputeProjectionMatrix() const = 0;
	};
}

#endif