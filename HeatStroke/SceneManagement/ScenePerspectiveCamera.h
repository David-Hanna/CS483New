//------------------------------------------------------------------------
// ScenePerspectiveCamera
// Author: David Hanna
//	
// View/Projection matrices representing a perspective camera.
//------------------------------------------------------------------------

#ifndef SCENE_PERSPECTIVE_CAMERA_H
#define SCENE_PERSPECTIVE_CAMERA_H

#include "SceneCamera.h"
#include "Frustum.h"

namespace HeatStroke
{
	class ScenePerspectiveCamera : public SceneCamera
	{
	public:
		ScenePerspectiveCamera();

		ScenePerspectiveCamera(
			const glm::vec3& p_vPos,
			const glm::vec3& p_vTarget,
			const glm::vec3& p_vUp,
			const float p_fFOV,
			const float p_fAspectRatio,
			const float p_fNearClip,
			const float p_fFarClip
		);

		virtual ~ScenePerspectiveCamera() {}

		float GetFOV()			const	{ return m_fFOV; }
		float GetAspectRatio()	const	{ return m_fAspectRatio; }
		float GetNearClip()		const	{ return m_fNearClip; }
		float GetFarClip()		const	{ return m_fFarClip; }

		const Frustum& GetViewFrustum() const;

		virtual void SetPosition(const glm::vec3 &p_vPos) override;
		virtual void SetTarget(const glm::vec3& p_vTarget) override;
		virtual void SetUp(const glm::vec3& p_vUp) override;

		void SetFOV(const float p_fFOV);
		void SetAspectRatio(const float p_fAspectRatio);
		void SetNearClip(const float p_fNearClip);
		void SetFarClip(const float p_fFarClip);

	protected:
		float m_fFOV;
		float m_fAspectRatio;
		float m_fNearClip;
		float m_fFarClip;

		mutable bool m_bViewFrustumDirty;

		mutable Frustum m_mViewFrustum;

		virtual glm::mat4 ComputeProjectionMatrix() const override;
	};
}

#endif