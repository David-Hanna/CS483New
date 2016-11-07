//------------------------------------------------------------------------
// SceneOrthographicCamera
// Author: David Hanna
//	
// View/Projection matrices representing an orthographic camera.
//------------------------------------------------------------------------

#ifndef SCENE_ORTHOGRAPHIC_CAMERA_H
#define SCENE_ORTHOGRAPHIC_CAMERA_H

#include "SceneCamera.h"

namespace HeatStroke
{
	class SceneOrthographicCamera : public SceneCamera
	{
	public:
		SceneOrthographicCamera();
		SceneOrthographicCamera(
			const glm::vec3& p_vPosition,
			const glm::vec3& p_vTarget,
			const glm::vec3& p_vUp,
			float p_fLeft, 
			float p_fRight, 
			float p_fBottom, 
			float p_fTop
		);

		virtual ~SceneOrthographicCamera() {}

		float GetLeft()		const	{ return m_fLeft; }
		float GetRight()	const	{ return m_fRight; }
		float GetBottom()	const	{ return m_fBottom; }
		float GetTop()		const	{ return m_fTop; }

		void SetLeft(float p_fLeft);
		void SetRight(float p_fRight);
		void SetBottom(float p_fBottom);
		void SetTop(float p_fTop);

	private:
		float m_fLeft;
		float m_fRight;
		float m_fBottom;
		float m_fTop;

		virtual glm::mat4 ComputeProjectionMatrix() const;
	};
}

#endif