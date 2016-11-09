//------------------------------------------------------------------------
// SceneOrthographicCamera
// Author: David Hanna
//	
// View/Projection matrices representing an orthographic camera.
//------------------------------------------------------------------------

#include "SceneOrthographicCamera.h"

HeatStroke::SceneOrthographicCamera::SceneOrthographicCamera() :
	SceneCamera(),
	m_fLeft(-100.0f),
	m_fRight(100.0f),
	m_fBottom(-100.0f),
	m_fTop(100.0f)
{
}

HeatStroke::SceneOrthographicCamera::SceneOrthographicCamera(
	const glm::vec3& p_vPosition,
	const glm::vec3& p_vTarget,
	const glm::vec3& p_vUp,
	float p_fLeft, 
	float p_fRight, 
	float p_fBottom, 
	float p_fTop) 
:
	SceneCamera(p_vPosition, p_vTarget, p_vUp),
	m_fLeft(p_fLeft),
	m_fRight(p_fRight),
	m_fBottom(p_fBottom),
	m_fTop(p_fTop)
{
}

void HeatStroke::SceneOrthographicCamera::SetLeft(float p_fLeft)
{
	m_bProjectionDirty = true;
	m_fLeft = p_fLeft;
}

void HeatStroke::SceneOrthographicCamera::SetRight(float p_fRight)
{
	m_bProjectionDirty = true;
	m_fRight = p_fRight;
}

void HeatStroke::SceneOrthographicCamera::SetBottom(float p_fBottom)
{
	m_bProjectionDirty = true;
	m_fBottom = p_fBottom;
}

void HeatStroke::SceneOrthographicCamera::SetTop(float p_fTop)
{
	m_bProjectionDirty = true;
	m_fTop = p_fTop;
}

glm::mat4 HeatStroke::SceneOrthographicCamera::ComputeProjectionMatrix() const
{
	return glm::ortho(m_fLeft, m_fRight, m_fBottom, m_fTop);
}