//------------------------------------------------------------------------
// ScenePerspectiveCamera
// Author: David Hanna
//	
// View/Projection matrices representing a perspective camera.
//------------------------------------------------------------------------

#include "ScenePerspectiveCamera.h"

HeatStroke::ScenePerspectiveCamera::ScenePerspectiveCamera()
	:
	SceneCamera(),
	m_fFOV(45.0f),
	m_fAspectRatio(1280.0f / 720.0f),
	m_fNearClip(0.1f),
	m_fFarClip(1000.0f),
	m_bViewFrustumDirty(true),
	m_mViewFrustum()
{
}

HeatStroke::ScenePerspectiveCamera::ScenePerspectiveCamera(
	const glm::vec3& p_vPos,
	const glm::vec3& p_vTarget,
	const glm::vec3& p_vUp,
	const float p_fFOV,
	const float p_fAspectRatio,
	const float p_fNearClip,
	const float p_fFarClip
	)
	:
	SceneCamera(p_vPos, p_vTarget, p_vUp),
	m_fFOV(p_fFOV),
	m_fAspectRatio(p_fAspectRatio),
	m_fNearClip(p_fNearClip),
	m_fFarClip(p_fFarClip),
	m_bViewFrustumDirty(true),
	m_mViewFrustum()
{
}

void HeatStroke::ScenePerspectiveCamera::SetPosition(const glm::vec3 &p_vPos)
{
	SceneCamera::SetPosition(p_vPos);
	m_bViewFrustumDirty = true;
}

void HeatStroke::ScenePerspectiveCamera::SetTarget(const glm::vec3& p_vTarget)
{
	SceneCamera::SetTarget(p_vTarget);
	m_bViewFrustumDirty = true;
}

void HeatStroke::ScenePerspectiveCamera::SetUp(const glm::vec3& p_vUp)
{
	SceneCamera::SetUp(p_vUp);
	m_bViewFrustumDirty = true;
}

void HeatStroke::ScenePerspectiveCamera::SetFOV(float p_fFOV)
{
	m_bProjectionDirty = true;
	m_bViewFrustumDirty = true;
	m_fFOV = p_fFOV;
}

void HeatStroke::ScenePerspectiveCamera::SetAspectRatio(float p_fAspectRatio)
{
	m_bProjectionDirty = true;
	m_bViewFrustumDirty = true;
	m_fAspectRatio = p_fAspectRatio;
}

void HeatStroke::ScenePerspectiveCamera::SetNearClip(float p_fNearClip)
{
	m_bProjectionDirty = true;
	m_bViewFrustumDirty = true;
	m_fNearClip = p_fNearClip;
}

void HeatStroke::ScenePerspectiveCamera::SetFarClip(float p_fFarClip)
{
	m_bProjectionDirty = true;
	m_bViewFrustumDirty = true;
	m_fFarClip = p_fFarClip;
}

const glm::mat4& HeatStroke::ScenePerspectiveCamera::GetProjectionMatrix() const
{
	if (m_bProjectionDirty)
	{
		m_mProjectionMatrix = glm::perspective(m_fFOV, m_fAspectRatio, m_fNearClip, m_fFarClip);
		m_bProjectionDirty = false;
	}
	return m_mProjectionMatrix;
}

const glm::mat4& HeatStroke::ScenePerspectiveCamera::GetViewMatrix() const
{
	if (m_bViewDirty)
	{
		m_mViewMatrix = glm::lookAt(m_vPosition, m_vTarget, m_vUp);
		m_bViewDirty = false;
	}
	return m_mViewMatrix;
}

const glm::mat4& HeatStroke::ScenePerspectiveCamera::GetViewProjectionMatrix() const
{
	if (m_bViewDirty || m_bProjectionDirty)
	{
		m_mViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
	}
	
	return m_mViewProjectionMatrix;
}

const HeatStroke::Frustum& HeatStroke::ScenePerspectiveCamera::GetViewFrustum() const
{
	if (m_bViewFrustumDirty)
	{
		m_mViewFrustum = HeatStroke::Frustum(GetViewProjectionMatrix());
		m_bViewFrustumDirty = false;
	}
	return m_mViewFrustum;
}