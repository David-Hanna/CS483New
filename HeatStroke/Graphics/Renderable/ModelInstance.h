//-----------------------------------------------------------------------------
// ModelInstance
// Author: David Hanna
//
// Adds world transform and render behaviour to a model.
//-----------------------------------------------------------------------------

#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H

#include "Renderable.h"
#include "ModelManager.h"

namespace HeatStroke
{
	class ModelInstance : public Renderable
	{
	public:
		ModelInstance(const std::string& p_strOBJFileName);
		ModelInstance(Model* p_pModel);

		virtual void Render(const SceneCamera* p_pCamera = nullptr) override;

		Model*		GetModel()											{ return m_pModel; }

		void		SetTransform(const glm::mat4& p_mWorldTransform)	{ m_mWorldTransform = p_mWorldTransform; }
		glm::mat4&	GetTransform()										{ return m_mWorldTransform; }

	private:
		Model*		 m_pModel;
		glm::mat4	 m_mWorldTransform;
	};
}

#endif MODEL_INSTANCE_H