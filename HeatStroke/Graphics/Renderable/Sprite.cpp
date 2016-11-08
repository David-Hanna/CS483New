//-----------------------------------------------------------------------------
// Sprite
// Author: David Hanna
//
// Renderable 2D textured quad.
//-----------------------------------------------------------------------------

#include "Sprite.h"

float HeatStroke::Sprite::m_vQuad[] = {
				// pos           // uv
/* top left	*/	-1.0f,  1.0f,	0.0f, 1.0f,
/* top right*/	 1.0f,  1.0f,	1.0f, 1.0f,
/* bot right*/	 1.0f, -1.0f,	1.0f, 0.0f,

/* top left	*/	-1.0f,  1.0f,   0.0f, 1.0f,
/* bot right*/	 1.0f, -1.0f,   1.0f, 0.0f,
/* bot left */	-1.0f, -1.0f,	0.0f, 0.0f
};

HeatStroke::Sprite::Sprite(const std::string& p_strMTLFileName, const std::string& p_strMaterialName)
{
	MTLFile mMTLFile(p_strMTLFileName);
	mMTLFile.ParseFile();
	const MTLFile::MTLMaterial* mMTLMaterial = mMTLFile.GetMaterial(p_strMaterialName);

	m_mMesh.m_pVertexBuffer = HeatStroke::BufferManager::CreateVertexBuffer(m_vQuad, 24 * sizeof(float));

	m_mMesh.m_pVertexDeclaration = new HeatStroke::VertexDeclaration;
	m_mMesh.m_pVertexDeclaration->Begin();

	m_mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Position, 2, HeatStroke::CT_Float, 0);
	m_mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_TexCoord1, 2, HeatStroke::CT_Float, 2 * sizeof(float));

	m_mMesh.m_pVertexDeclaration->SetVertexBuffer(m_mMesh.m_pVertexBuffer);
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

void HeatStroke::Sprite::Update(float p_fDelta)
{

}

void HeatStroke::Sprite::Render(const SceneCamera* p_pCamera)
{
	assert(p_pCamera != nullptr);

	glm::mat4 mWorldViewProjectionTransform = p_pCamera->GetViewProjectionMatrix() * m_mWorldTransform;

	m_mMesh.m_pVertexDeclaration->Bind();
	m_mMesh.m_pMaterial->SetUniform("WorldViewProjectionTransform", mWorldViewProjectionTransform);
	m_mMesh.m_pMaterial->Apply();

	glDrawArrays(GL_TRIANGLES, 0, m_mMesh.m_pVertexBuffer->GetLength());
}