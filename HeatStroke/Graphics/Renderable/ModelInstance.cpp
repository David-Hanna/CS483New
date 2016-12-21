//-----------------------------------------------------------------------------
// ModelInstance
// Author: David Hanna
//
// An instance of a model which adds transformation and render behaviours to the mesh.
//-----------------------------------------------------------------------------

#include "ModelInstance.h"

HeatStroke::ModelInstance::ModelInstance(const std::string& p_strOBJFileName) :
	m_mWorldTransform()
{
	m_pModel = ModelManager::Instance()->GetOrCreateModel(p_strOBJFileName);
}

HeatStroke::ModelInstance::ModelInstance(Model* p_pModel) :
	m_pModel(p_pModel),
	m_mWorldTransform()
{
}

void HeatStroke::ModelInstance::Render(const SceneCamera* p_pCamera)
{
	// Can't render without a camera.
	assert(p_pCamera != nullptr);

	glm::mat4 mWorldViewTransform = p_pCamera->GetViewMatrix() * m_mWorldTransform;
	glm::mat4 mWorldViewProjectionTransform = p_pCamera->GetProjectionMatrix() * mWorldViewTransform;
	glm::mat3 mWorldInverseTransposeTransform = glm::transpose(glm::inverse(glm::mat3(m_mWorldTransform)));

	std::vector<Mesh> vMeshes = m_pModel->GetMeshes();
	std::vector<Mesh>::const_iterator meshIt = vMeshes.begin(), meshEnd = vMeshes.end();
	for (; meshIt != meshEnd; meshIt++)
	{
		meshIt->m_pVertexDeclaration->Bind();

		meshIt->m_pMaterial->SetUniform("WorldTransform", m_mWorldTransform);
		meshIt->m_pMaterial->SetUniform("WorldViewTransform", mWorldViewTransform);
		meshIt->m_pMaterial->SetUniform("WorldViewProjectionTransform", mWorldViewProjectionTransform);
		meshIt->m_pMaterial->SetUniform("WorldInverseTransposeTransform", mWorldInverseTransposeTransform);

		meshIt->m_pMaterial->Apply();

		glDrawArrays(GL_TRIANGLES, 0, meshIt->m_pVertexBuffer->GetLength());
	}
}