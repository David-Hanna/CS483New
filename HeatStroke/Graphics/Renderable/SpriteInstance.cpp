//-----------------------------------------------------------------------------
// SpriteInstance
// Author: David Hanna
//
// Adds world transform and render behaviour to a sprite.
//-----------------------------------------------------------------------------

#include "SpriteInstance.h"

HeatStroke::SpriteInstance::SpriteInstance(const std::string& p_strMTLFileName, const std::string& p_strMaterialName) :
m_mWorldTransform()
{
	m_pSprite = SpriteManager::Instance()->GetOrCreateSprite(p_strMTLFileName, p_strMaterialName);
}

HeatStroke::SpriteInstance::SpriteInstance(Sprite* p_pSprite) :
m_pSprite(p_pSprite),
m_mWorldTransform()
{
}

void HeatStroke::SpriteInstance::Render(const SceneCamera* p_pCamera)
{
	assert(p_pCamera != nullptr);

	glm::mat4 mWorldViewProjectionTransform = p_pCamera->GetViewProjectionMatrix() * m_mWorldTransform;

	Mesh& mMesh = m_pSprite->GetMesh();
	mMesh.m_pVertexDeclaration->Bind();
	mMesh.m_pMaterial->SetUniform("WorldViewProjectionTransform", mWorldViewProjectionTransform);
	mMesh.m_pMaterial->Apply();

	glDrawElements(GL_TRIANGLES, mMesh.m_pIndexBuffer->GetNumIndices(), GL_UNSIGNED_SHORT, 0);
}