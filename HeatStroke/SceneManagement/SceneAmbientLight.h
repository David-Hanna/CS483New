//------------------------------------------------------------------------
// SceneAmbientLight
// Author:	David Hanna
//	
// Data structure for an ambient light which may be added to the SceneManager.
//------------------------------------------------------------------------

#ifndef SCENE_AMBIENT_LIGHT_H
#define SCENE_AMBIENT_LIGHT_H

#include "glm.hpp"

namespace HeatStroke
{
	class SceneAmbientLight
	{
	public:
		SceneAmbientLight()							 : m_vColor(glm::vec3(1.0f, 1.0f, 1.0f)) {}
		SceneAmbientLight(const glm::vec3& p_vColor) : m_vColor(p_vColor)					 {}

		~SceneAmbientLight() {}

		const glm::vec3& GetColor() const							{ return m_vColor; }
		void			 SetColor(const glm::vec3& p_vColor)		{ m_vColor = p_vColor; }

	private:
		glm::vec3 m_vColor;
	};
}

#endif