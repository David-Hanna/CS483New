//------------------------------------------------------------------------
// SceneDirectionalLight
// Author:	David Hanna
//	
// Data structure for a directional light which may be added to the SceneManager.
//------------------------------------------------------------------------

#ifndef SCENE_DIRECTIONAL_LIGHT_H
#define SCENE_DIRECTIONAL_LIGHT_H

#include "glm.hpp"

namespace HeatStroke
{
	class SceneDirectionalLight
	{
	public:
		SceneDirectionalLight() :
			m_vDirection(glm::vec3(0.0f, -1.0f, 0.0f)),
			m_vColor(glm::vec3(1.0f, 1.0f, 1.0f)) 
		{}

		SceneDirectionalLight(const glm::vec3& p_vDirection, const glm::vec3& p_vColor) :
			m_vDirection(p_vDirection),
			m_vColor(p_vColor)
		{}

		~SceneDirectionalLight() {}

		const glm::vec3& GetDirection	() const						{ return m_vDirection; }
		const glm::vec3& GetColor		() const						{ return m_vColor; }

		void			 SetDirection	(const glm::vec3& p_vDirection) { m_vDirection = p_vDirection; }
		void			 SetColor		(const glm::vec3& p_vColor)		{ m_vColor = p_vColor; }

	private:
		glm::vec3 m_vDirection;
		glm::vec3 m_vColor;
	};
}

#endif