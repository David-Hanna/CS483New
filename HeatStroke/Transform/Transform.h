//------------------------------------------------------------------------
// Transform
// Author:	David Hanna
//------------------------------------------------------------------------

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm.hpp>
#include <gtx/matrix_decompose.hpp>

#include "EasyXML.h"

namespace HeatStroke
{
	class Transform
	{
	public:
		Transform();
		virtual ~Transform() {}

		virtual void				Translate			(const glm::vec3& p_vTranslation);
		virtual void				TranslateXYZ		(float x, float y, float z)				{ this->Translate(glm::vec3(x, y, z)); }
		virtual void				SetTranslation		(const glm::vec3 &p_vTranslation);
		virtual void				SetTranslationXYZ	(float x, float y, float z)				{ this->SetTranslation(glm::vec3(x, y, z)); }
		virtual const glm::vec3&	GetTranslation		() const;

		virtual void				Rotate				(const float p_fAngle, const glm::vec3& p_vRotation);
		virtual void				RotateXYZ			(const float p_fAngle, const float x, const float y, const float z)	{ this->Rotate(p_fAngle, glm::vec3(x, y, z)); }
		virtual void				SetRotation			(const float p_fAngle, const glm::vec3& p_vRotation);
		virtual void				SetRotation			(const glm::quat& p_qRotation)										{ m_qRotation = p_qRotation; }
		virtual void				SetRotationXYZ		(const float p_fAngle, float x, float y, float z)					{ this->SetRotation(p_fAngle, glm::vec3(x, y, z)); }
		virtual const glm::quat&	GetRotation			() const;

		virtual void				Scale				(const glm::vec3& p_vScale);
		virtual void				ScaleXYZ			(float x, float y, float z)				{ this->Scale(glm::vec3(x, y, z)); }
		virtual void				SetScale			(const glm::vec3& p_vScale);
		virtual void				SetScaleXYZ			(float x, float y, float z)				{ this->SetScale(glm::vec3(x, y, z)); }
		virtual const glm::vec3&	GetScale			() const;

		virtual void				SetTransform		(const glm::mat4& p_mTransform);
		virtual const glm::mat4&	GetTransform		() const;

		virtual void				ParseTransformNode	(tinyxml2::XMLNode* p_pTransformNode);

	protected:
		// If the transform changes, then all the individual components are dirty.
		mutable bool m_bComponentsDirty;

		// If any component changes, then the transform is dirty.
		mutable bool m_bTransformDirty;

		mutable glm::vec3 m_vScale;
		mutable glm::quat m_qRotation;
		mutable glm::vec3 m_vTranslation;

		mutable glm::mat4 m_mTransform;

	protected:
		void UpdateComponents() const; // Logically const-correct, not bitwise.
	};
}

#endif // TRANSFORM_H