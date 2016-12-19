//-----------------------------------------------------------------------------
// Sprite
// Author: David Hanna
//
// 2D textured quad.
//-----------------------------------------------------------------------------

#include "Sprite.h"

float HeatStroke::Sprite::m_vQuadVertex[] = {
				// pos           // uv
/* top left	*/	-1.0f,  1.0f,	0.0f, 1.0f,
/* top right*/	 1.0f,  1.0f,	1.0f, 1.0f,
/* bot right*/	 1.0f, -1.0f,	1.0f, 0.0f,
/* bot left */	-1.0f, -1.0f,	0.0f, 0.0f
};

unsigned short HeatStroke::Sprite::m_vQuadIndex[] = { 0, 1, 2, 0, 2, 3 };

HeatStroke::Sprite::Sprite(const std::string& p_strMTLFileName, const std::string& p_strMaterialName)
{
	MTLFile mMTLFile(p_strMTLFileName);
	mMTLFile.ParseFile();
	const MTLFile::MTLMaterial* mMTLMaterial = mMTLFile.GetMaterial(p_strMaterialName);

	m_mMesh.m_pVertexBuffer = HeatStroke::BufferManager::CreateVertexBuffer(m_vQuadVertex, 16 * sizeof(float));
	m_mMesh.m_pIndexBuffer = HeatStroke::BufferManager::CreateIndexBuffer(m_vQuadIndex, 6 * sizeof(unsigned short));

	m_mMesh.m_pVertexDeclaration = new HeatStroke::VertexDeclaration;
	m_mMesh.m_pVertexDeclaration->Begin();

	m_mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Position, 2, HeatStroke::CT_Float, 0);
	m_mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_TexCoord1, 2, HeatStroke::CT_Float, 2 * sizeof(float));

	m_mMesh.m_pVertexDeclaration->SetVertexBuffer(m_mMesh.m_pVertexBuffer);
	m_mMesh.m_pVertexDeclaration->SetIndexBuffer(m_mMesh.m_pIndexBuffer);
	m_mMesh.m_pVertexDeclaration->End();

	m_mMesh.m_pMaterial = HeatStroke::MaterialManager::CreateMaterial(mMTLMaterial->GetMaterialName());
	m_mMesh.m_pMaterial->SetProgram(mMTLMaterial->GetVertexShaderName(), mMTLMaterial->GetFragmentShaderName());

	m_mMesh.m_pTexture = HeatStroke::TextureManager::CreateTexture(mMTLMaterial->GetDiffuseTextureFileName());
	m_mMesh.m_pTexture->SetWrapMode(HeatStroke::Texture::WM_Repeat);

	m_mMesh.m_pMaterial->SetTexture("DiffuseTexture", m_mMesh.m_pTexture);
}

HeatStroke::Sprite::~Sprite()
{
	HeatStroke::BufferManager::DestroyBuffer(m_mMesh.m_pVertexBuffer);
	DELETE_IF(m_mMesh.m_pVertexDeclaration);
	HeatStroke::MaterialManager::DestroyMaterial(m_mMesh.m_pMaterial);
}