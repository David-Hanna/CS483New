//------------------------------------------------------------------------
// SceneCamera
// Author: David Hanna
//	
// View/Projection matrices. Must be subclassed into ortho/perspective.
//------------------------------------------------------------------------

#include "SceneCamera.h"

HeatStroke::SceneCamera::SceneCamera() :
	m_vPosition(glm::vec3(0.0f, 0.0f, -5.0f)),
	m_vTarget(glm::vec3(0.0f, 0.0f, 0.0f)),
	m_vUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	m_bViewDirty(true),
	m_bProjectionDirty(true),
	m_mViewMatrix(),
	m_mProjectionMatrix(),
	m_mViewProjectionMatrix()
{
}

HeatStroke::SceneCamera::SceneCamera(const glm::vec3& p_vPosition, const glm::vec3& p_vTarget, const glm::vec3& p_vUp) :
	m_vPosition(p_vPosition),
	m_vTarget(p_vTarget),
	m_vUp(p_vUp),
	m_bViewDirty(true),
	m_bProjectionDirty(true),
	m_mViewMatrix(),
	m_mProjectionMatrix(),
	m_mViewProjectionMatrix()
{
}

const glm::mat4& HeatStroke::SceneCamera::GetViewMatrix() const
{
	if (m_bViewDirty)
	{
		m_mViewMatrix = glm::lookAt(m_vPosition, m_vTarget, m_vUp);
		m_bViewDirty = false;
	}
	return m_mViewMatrix;
}

const glm::mat4& HeatStroke::SceneCamera::GetProjectionMatrix() const
{
	if (m_bProjectionDirty)
	{
		m_mProjectionMatrix = ComputeProjectionMatrix();
		m_bProjectionDirty = false;
	}
	return m_mProjectionMatrix;
}

const glm::mat4& HeatStroke::SceneCamera::GetViewProjectionMatrix() const
{
	if (m_bViewDirty || m_bProjectionDirty)
	{
		m_mViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
	}

	return m_mViewProjectionMatrix;
}

void HeatStroke::SceneCamera::SetPosition(const glm::vec3 &p_vPosition)
{
	m_bViewDirty = true;
	m_vPosition = p_vPosition;
}

void HeatStroke::SceneCamera::SetTarget(const glm::vec3& p_vTarget)
{
	m_bViewDirty = true;
	m_vTarget = p_vTarget;
}

void HeatStroke::SceneCamera::SetUp(const glm::vec3& p_vUp)
{
	m_bViewDirty = true;
	m_vUp = p_vUp;
}