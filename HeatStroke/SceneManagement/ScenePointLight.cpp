//=======================================================================================
// ScenePointLight
// Author: David Hanna
//
// A Point Light which the Scene Manager uses to add light to the scene.
//=======================================================================================

#include "ScenePointLight.h"

namespace HeatStroke
{
	//----------------------------------------------------------------------------
	// ScenePointLight::ScenePointLight
	//
	// Constructor.
	//----------------------------------------------------------------------------
	ScenePointLight::ScenePointLight(
		const glm::vec3& p_vPosition,
		const glm::vec3& p_vAmbient,
		const glm::vec3& p_vDiffuse,
		const glm::vec3& p_vSpecular,
		const glm::vec3& p_vAttenuation,
		const float      p_fRange)
		:
		m_vPosition(p_vPosition),
		m_vAmbient(p_vAmbient),
		m_vDiffuse(p_vDiffuse),
		m_vSpecular(p_vSpecular),
		m_vAttenuation(p_vAttenuation),
		m_fRange(p_fRange)
	{
	}
}