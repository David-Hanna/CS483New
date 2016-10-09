//===================================================================================
// Camera
// Author: David Hanna
//
// An interface providing a view matrix and a projection matrix.
//===================================================================================

#include "Camera.h"

namespace HeatStroke
{
	Camera::Camera()
		:
		m_fFOV(45.0f),
		m_fAspectRatio(1280.0f / 720.0f),
		m_fNearClip(0.1f),
		m_fFarClip(1000.0f),
		m_vPos(glm::vec3(0.0f, 0.0f, -5.0f)),
		m_vTarget(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_vUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		m_bProjectionDirty(true),
		m_bViewDirty(true),
		m_bViewProjectionDirty(true),
		m_bViewFrustumDirty(true),
		m_mViewMatrix(),
		m_mProjectionMatrix(),
		m_mViewProjectionMatrix(),
		m_mViewFrustum()
	{
	}

	Camera::Camera(
		const float p_fFOV, 
		const float p_fAspectRatio, 
		const float p_fNearClip, 
		const float p_fFarClip, 
		const glm::vec3& p_vPos, 
		const glm::vec3& p_vTarget, 
		const glm::vec3& p_vUp)
		:
		m_fFOV(p_fFOV),
		m_fAspectRatio(p_fAspectRatio),
		m_fNearClip(p_fNearClip),
		m_fFarClip(p_fFarClip),
		m_vPos(p_vPos),
		m_vTarget(p_vTarget),
		m_vUp(p_vUp),
		m_bProjectionDirty(true),
		m_bViewDirty(true),
		m_bViewProjectionDirty(true),
		m_bViewFrustumDirty(true),
		m_mViewMatrix(),
		m_mProjectionMatrix(),
		m_mViewProjectionMatrix(),
		m_mViewFrustum()
	{
	}

	void Camera::SetFOV(float p_fFOV)
	{
		m_bProjectionDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_fFOV = p_fFOV;
	}

	float Camera::GetFOV() const
	{
		return m_fFOV;
	}

	void Camera::SetAspectRatio(float p_fAspectRatio)
	{
		m_bProjectionDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_fAspectRatio = p_fAspectRatio;
	}

	float Camera::GetAspectRatio() const
	{
		return m_fAspectRatio;
	}

	void Camera::SetNearClip(float p_fNearClip)
	{
		m_bProjectionDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_fNearClip = p_fNearClip;
	}

	float Camera::GetNearClip() const
	{
		return m_fNearClip;
	}

	void Camera::SetFarClip(float p_fFarClip)
	{
		m_bProjectionDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_fFarClip = p_fFarClip;
	}

	float Camera::GetFarClip() const
	{
		return m_fFarClip;
	}

	void Camera::SetPos(const glm::vec3 &p_vPos)
	{
		m_bViewDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_vPos = p_vPos;
	}

	const glm::vec3& Camera::GetPos() const
	{
		return m_vPos;
	}

	void Camera::SetTarget(const glm::vec3& p_vTarget)
	{
		m_bViewDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_vTarget = p_vTarget;
	}

	const glm::vec3& Camera::GetTarget() const
	{
		return m_vTarget;
	}

	void Camera::SetUp(const glm::vec3& p_vUp)
	{
		m_bViewDirty = true;
		m_bViewProjectionDirty = true;
		m_bViewFrustumDirty = true;
		m_vUp = p_vUp;
	}

	const glm::vec3& Camera::GetUp() const
	{
		return m_vUp;
	}

	const glm::mat4& Camera::GetProjectionMatrix() const
	{
		if (m_bProjectionDirty)
		{
			m_mProjectionMatrix = glm::perspective(m_fFOV, m_fAspectRatio, m_fNearClip, m_fFarClip);
			m_bProjectionDirty = false;
		}
		return m_mProjectionMatrix;
	}

	const glm::mat4& Camera::GetViewMatrix() const
	{
		if (m_bViewDirty)
		{
			m_mViewMatrix = glm::lookAt(m_vPos, m_vTarget, m_vUp);
			m_bViewDirty = false;
		}
		return m_mViewMatrix;
	}

	const glm::mat4& Camera::GetViewProjectionMatrix() const
	{
		if (m_bViewProjectionDirty)
		{
			m_mViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
			m_bViewProjectionDirty = false;
		}
		return m_mViewProjectionMatrix;
	}

	const HeatStroke::Frustum& Camera::GetViewFrustum() const
	{
		if (m_bViewFrustumDirty)
		{
			m_mViewFrustum = HeatStroke::Frustum(GetViewProjectionMatrix());
			m_bViewFrustumDirty = false;
		}
		return m_mViewFrustum;
	}
}