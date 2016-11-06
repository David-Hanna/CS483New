//------------------------------------------------------------------------
// SceneDirectionalLight
// Author:	David Hanna
//	
// Data structure for a directional light which may be added to the SceneManager.
//------------------------------------------------------------------------

#include "SceneDirectionalLight.h"

HeatStroke::SceneDirectionalLight::SceneDirectionalLight() :
	m_vDirection(glm::vec3(0.0f, -1.0f, 0.0f)),
	m_vColor(glm::vec3(1.0f, 1.0f, 1.0f))
{
}

HeatStroke::SceneDirectionalLight::SceneDirectionalLight(const glm::vec3& p_vDirection, const glm::vec3& p_vColor) :
	m_vDirection(p_vDirection),
	m_vColor(p_vColor)
{
}