//-----------------------------------------------------------------------------
// File:			Model.h
// Original Author:	Gordon Wood
//
// Class representing a model that can be loaded from a POD file
//
// ASSUMPTIONS:
//	1) POD files will always be exported as Index Triangle Lists
//	2) No more than 8 UV channels in the POD file
//	3) Model is exported with interleaved data
//-----------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include "Types.h"
#include "Renderable.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BufferManager.h"
#include "Material.h"
#include "VertexDeclaration.h"
#include "TextureManager.h"
#include "ProgramManager.h"
#include "MaterialManager.h"
#include "OBJFile.h"
#include "MTLFile.h"

#include <string>
#include <map>
#include <vector>

#ifdef _DEBUG
#include "Common.h"
#endif

namespace HeatStroke
{
	class Model : public Renderable
	{
	public:
		struct Mesh
		{
			VertexBuffer*		m_pVertexBuffer;
			VertexDeclaration*	m_pVertexDeclaration;
			Texture*			m_pTexture;
			Material*			m_pMaterial;
		};

	public:
		Model(const std::string& p_strOBJFileName);
		~Model();

		void Update(float p_fDelta);
		virtual void Render(const SceneCamera* p_pCamera = nullptr) override;

		std::vector<Mesh>&			GetMeshes()											{ return m_vMeshes; }
		void						SetTransform(const glm::mat4& p_mWorldTransform)	{ m_mWorldTransform = p_mWorldTransform; }

	private:
		std::vector<Mesh>					m_vMeshes;
		glm::mat4							m_mWorldTransform;
	};
}

#endif



