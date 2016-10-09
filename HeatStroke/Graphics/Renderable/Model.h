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
//#include "PVRTModelPOD.h"
#include <string>
#include <map>
#include <vector>

namespace HeatStroke
{
class Model : public Renderable
{
	public:
		Model(const std::string& p_strFile, const std::string& p_strTexturePrefix = "", const std::string& p_strVertexProgram = "", const std::string& p_strFragmentProgram = "");
		~Model();

		void Update(float p_fDelta);
		virtual void Render(const Camera* p_pCamera = nullptr) override;

		//void SetAnimFrame(float p_fFrame) { m_fFrame = p_fFrame; m_pod.SetFrame(m_fFrame); }
		HeatStroke::Material* GetMaterial() { return m_pMaterial; }
		void SetTransform(const glm::mat4& p_mWorldTransform) { m_mWorldTransform = p_mWorldTransform; }

	private:
		struct Mesh
		{
			HeatStroke::VertexBuffer* m_pVB;
			HeatStroke::IndexBuffer* m_pIB;
			HeatStroke::VertexDeclaration* m_pDecl;
		};

		float				m_fFrame;
		//CPVRTModelPOD		m_pod;
		std::vector<Mesh>	m_lMeshes;
		Material*			m_pMaterial;
		glm::mat4			m_mWorldTransform;
};

}

#endif



