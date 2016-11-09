//-----------------------------------------------------------------------------
// Model
// Author: David Hanna
//
// A renderable model loaded in from .obj and .mtl data.
//-----------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include "Transform.h"
#include "Mesh.h"
#include "OBJFile.h"
#include "MTLFile.h"

#include <string>
#include <vector>

#ifdef _DEBUG
#include "Common.h"
#endif

namespace HeatStroke
{
	class Model : public Renderable
	{
	public:
		Model(const std::string& p_strOBJFileName);
		~Model();

		void		 Update(float p_fDelta);
		virtual void Render(const SceneCamera* p_pCamera = nullptr) override;

		std::vector<Mesh>&			GetMeshes()											{ return m_vMeshes; }
		void						SetTransform(const Transform& p_mWorldTransform)	{ m_mWorldTransform = p_mWorldTransform; }
		Transform&					GetTransform()										{ return m_mWorldTransform; }

	private:
		std::vector<Mesh>					m_vMeshes;
		Transform							m_mWorldTransform;
	};
}

#endif



