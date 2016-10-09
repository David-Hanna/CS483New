//------------------------------------------------------------------------
// SceneCamera
//
// Created:	2012/12/23
// Author:	Carel Boers
//	
// Abstraction of the underlying OpenGL view/projection matrices.
//------------------------------------------------------------------------

#ifndef SCENECAMERA_H
#define SCENECAMERA_H

#include "Camera.h"

namespace HeatStroke
{
	class SceneCamera : public HeatStroke::Camera
	{
	public:
		//---------------------------------------------------------------------
		// Public methods
		//---------------------------------------------------------------------
		SceneCamera() : Camera() {}

		SceneCamera
		(
			float p_fFOV,
			float p_fAspectRatio,
			float p_fNearClip,
			float p_fFarClip,
			const glm::vec3 &p_vPos,
			const glm::vec3& p_vTarget,
			const glm::vec3& p_vUp
		) :
			Camera(p_fFOV, p_fAspectRatio, p_fNearClip, p_fFarClip, p_vPos, p_vTarget, p_vUp) 
		{}

		virtual ~SceneCamera() {}
	};

}

#endif // SCENECAMERA_H