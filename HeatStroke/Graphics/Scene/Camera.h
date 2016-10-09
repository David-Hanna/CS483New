//===================================================================================
// Camera
// Author: David Hanna
//
// An interface providing a view matrix and a projection matrix.
//===================================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>

#include "Frustum.h"

namespace HeatStroke
{
	class Camera
	{
	public:
		Camera();

		Camera(
			const float p_fFOV, 
			const float p_fAspectRatio, 
			const float p_fNearClip, 
			const float p_fFarClip, 
			const glm::vec3& p_vPos, 
			const glm::vec3& p_vTarget, 
			const glm::vec3& p_vUp
		);

		virtual ~Camera() {}

		void SetFOV(const float p_fFOV);
		float GetFOV() const;

		void SetAspectRatio(const float p_fAspectRatio);
		float GetAspectRatio() const;

		void SetNearClip(const float p_fNearClip);
		float GetNearClip() const;

		void SetFarClip(const float p_fFarClip);
		float GetFarClip() const;

		void SetPos(const glm::vec3 &p_vPos);
		const glm::vec3& GetPos() const;

		void SetTarget(const glm::vec3& p_vTarget);
		const glm::vec3& GetTarget() const;

		void SetUp(const glm::vec3& p_vUp);
		const glm::vec3& GetUp() const;

		virtual const glm::mat4& GetViewMatrix() const;
		virtual const glm::mat4& GetProjectionMatrix() const;
		virtual const glm::mat4& GetViewProjectionMatrix() const;

		virtual void SetViewMatrix(const glm::mat4& p_mViewMatrix) const						{ m_mViewMatrix = p_mViewMatrix; }
		virtual void SetProjectionMatrix(const glm::mat4& p_mProjectionMatrix) const			{ m_mProjectionMatrix = p_mProjectionMatrix; }
		virtual void SetViewProjectionMatrix(const glm::mat4& p_mViewProjectionMatrix) const	{ m_mViewProjectionMatrix = p_mViewProjectionMatrix; }

		virtual const HeatStroke::Frustum& GetViewFrustum() const;

		virtual void Update(const float p_fDelta) {}

	private: 
		float m_fFOV;
		float m_fAspectRatio;
		float m_fNearClip;
		float m_fFarClip;

		glm::vec3 m_vPos;
		glm::vec3 m_vTarget;
		glm::vec3 m_vUp;

		mutable bool m_bProjectionDirty;
		mutable bool m_bViewDirty;
		mutable bool m_bViewProjectionDirty;

		mutable bool m_bViewFrustumDirty;

		mutable glm::mat4 m_mViewMatrix;
		mutable glm::mat4 m_mProjectionMatrix;
		mutable glm::mat4 m_mViewProjectionMatrix;

		mutable HeatStroke::Frustum m_mViewFrustum;
	};
}

#endif // W_CAMERA_H