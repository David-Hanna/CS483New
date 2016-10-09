//=======================================================================================
// ScenePointLight
// Author: David Hanna
//
// A Point Light which the Scene Manager uses to add light to the scene.
//=======================================================================================

#ifndef SCENE_POINT_LIGHT_H
#define SCENE_POINT_LIGHT_H

#include <glm.hpp>

namespace HeatStroke
{
	class ScenePointLight
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		// Construction/Destruction
		ScenePointLight(
			const glm::vec3& p_vPosition,
			const glm::vec3& p_vAmbient,
			const glm::vec3& p_vDiffuse,
			const glm::vec3& p_vSpecular,
			const glm::vec3& p_vAttenuation,
			const float      p_fRange );

		~ScenePointLight() {}

		// Accessors
		const glm::vec3& GetPosition()			{ return m_vPosition; }
		const glm::vec3& GetAmbient()			{ return m_vAmbient; }
		const glm::vec3& GetDiffuse()			{ return m_vDiffuse; }
		const glm::vec3& GetSpecular()			{ return m_vSpecular; }
		const glm::vec3& GetAttenuation()		{ return m_vAttenuation; }
		float			 GetRange()				{ return m_fRange; }

		// Mutators
		void SetPosition(const glm::vec3& p_vPosition)			{ m_vPosition = p_vPosition; }
		void SetAmbient(const glm::vec3& p_vAmbient)			{ m_vAmbient = p_vAmbient; }
		void SetDiffuse(const glm::vec3& p_vDiffuse)			{ m_vDiffuse = p_vDiffuse; }
		void SetSpecular(const glm::vec3& p_vSpecular)			{ m_vSpecular = p_vSpecular; }
		void SetAttenuation(const glm::vec3& p_vAttenuation)	{ m_vAttenuation = p_vAttenuation; }

	protected:
		//---------------------------------------------------------------------------
		// Protected members
		//---------------------------------------------------------------------------
		glm::vec3 m_vPosition;
		glm::vec3 m_vAmbient;
		glm::vec3 m_vDiffuse;
		glm::vec3 m_vSpecular;
		glm::vec3 m_vAttenuation;
		float	  m_fRange;
	};
}

#endif // SCENE_POINT_LIGHT_H