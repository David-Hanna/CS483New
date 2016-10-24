//-----------------------------------------------------------------------------
// File:			Model.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------

#include "Model.h"

HeatStroke::Model::Model(const std::string& p_mOBJFileName)
{
	OBJFile mOBJFile(p_mOBJFileName);
	mOBJFile.ParseFile();
	
	//================================================================
	// Vertex Data
	//================================================================

	// Grab the lists of positions, normals, and uvs for convenience.
	const std::vector<const glm::vec3>& vPositions = mOBJFile.GetPositions();
	const std::vector<const glm::vec3>& vNormals = mOBJFile.GetNormals();
	const std::vector<const glm::vec2>& vUVs = mOBJFile.GetUVs();

	// We assume for our models that every face is a triangle.
	const std::vector<const OBJFile::OBJFace>& vTriangles = mOBJFile.GetFaces();

	// Create the float array representing the vertex buffer data.
	// 3 floats for position, 3 for normals, and 2 for uvs (8 floats) per vertex. 
	// 3 vertices (24 floats) per triangle.
	// 24 floats per triangle * 4 bytes per float = 96 bytes per triangle.
	unsigned int uiVertexDataLength = 96 * vTriangles.size();

	// Load all the float data into a vector in an order that our vertex declaration (below) will expect.
	std::vector<float> vVertexData;

	std::vector<OBJFile::OBJFace>::const_iterator triangleIt = vTriangles.begin(), triangleEnd = vTriangles.end();
	for (; triangleIt != triangleEnd; triangleIt++)
	{
		std::vector<OBJFile::OBJVertex>::const_iterator vertexIt = triangleIt->begin(), vertexEnd = triangleIt->end();
		for (; vertexIt != vertexEnd; vertexIt++)
		{
			const glm::vec3 vertexPosition = vPositions[vertexIt->GetPositionIndex()];
			const glm::vec3 vertexNormal = vNormals[vertexIt->GetNormalIndex()];
			const glm::vec2 vertexUVs = vUVs[vertexIt->GetUVIndex()];

			vVertexData.push_back(vertexPosition.x);
			vVertexData.push_back(vertexPosition.y);
			vVertexData.push_back(vertexPosition.z);

			vVertexData.push_back(vertexNormal.x);
			vVertexData.push_back(vertexNormal.y);
			vVertexData.push_back(vertexNormal.z);

			vVertexData.push_back(vertexUVs.x);
			vVertexData.push_back(vertexUVs.y);
		}
	}

	m_pVertexBuffer = HeatStroke::BufferManager::CreateVertexBuffer(&(vVertexData[0]), uiVertexDataLength);
	//m_pIndexBuffer = HeatStroke::BufferManager::CreateIndexBuffer(pIndexData, uiIndexLength);

	m_pVertexDeclaration = new HeatStroke::VertexDeclaration;
	m_pVertexDeclaration->Begin();

	m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Position, 3, HeatStroke::CT_Float, 0);
	m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Normal, 3, HeatStroke::CT_Float, 12);
	m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_TexCoord1, 2, HeatStroke::CT_Float, 24);

	m_pVertexDeclaration->SetVertexBuffer(m_pVertexBuffer);
	//m_pVertexDeclaration->SetIndexBuffer(m_pIndexBuffer);
	m_pVertexDeclaration->End();

	//==============================================================
	// Material Data
	//==============================================================

	// Load the MTL file.
	MTLFile mMTLFile(mOBJFile.GetMTLFileName());
	mMTLFile.ParseFile();

	const MTLFile::MTLMaterial* mMTLMaterial = mMTLFile.GetMaterial(mOBJFile.GetMaterialName());

	if (mMTLMaterial == nullptr)
	{
#ifdef _DEBUG
		assert(false && "No material by this name.");
#endif
	}
	else
	{
		m_pMaterial = HeatStroke::MaterialManager::CreateMaterial(mMTLMaterial->GetMaterialName());
		m_pMaterial->SetProgram(mMTLMaterial->GetVertexShaderName(), mMTLMaterial->GetFragmentShaderName());

		HeatStroke::Texture* pTexture = HeatStroke::TextureManager::CreateTexture(mMTLMaterial->GetDiffuseTextureFileName());
		pTexture->SetWrapMode(HeatStroke::Texture::WM_Repeat);

		m_pMaterial->SetTexture("DiffuseTexture", pTexture);
	}
}


HeatStroke::Model::~Model()
{
	if (m_pVertexBuffer != nullptr)
	{
		HeatStroke::BufferManager::DestroyBuffer(m_pVertexBuffer);
		m_pVertexBuffer = nullptr;
	}
	
	/*if (m_pIndexBuffer != nullptr)
	{
		HeatStroke::BufferManager::DestroyBuffer(m_pIndexBuffer);
		m_pIndexBuffer = nullptr;
	}*/

	DELETE_IF(m_pVertexDeclaration);
}


void HeatStroke::Model::Update(float p_fDelta)
{
}


void HeatStroke::Model::Render(const Camera* p_pCamera)
{
	// Can't render without a camera.
	assert(p_pCamera != nullptr);

	std::cout << "Model is rendering.\n";

	m_pVertexDeclaration->Bind();

	glm::mat4 mWorldViewTransform = p_pCamera->GetViewMatrix() * m_mWorldTransform;
	glm::mat4 mWorldViewProjectionTransform = p_pCamera->GetProjectionMatrix() * mWorldViewTransform;
	glm::mat3 mWorldInverseTransposeTransform = glm::transpose(glm::inverse(glm::mat3(m_mWorldTransform)));

	m_pMaterial->SetUniform("WorldTransform", m_mWorldTransform);
	m_pMaterial->SetUniform("WorldViewTransform", mWorldViewTransform);
	m_pMaterial->SetUniform("WorldViewProjectionTransform", mWorldViewProjectionTransform);
	m_pMaterial->SetUniform("WorldInverseTransposeTransform", mWorldInverseTransposeTransform);
	
	m_pMaterial->Apply();

	glDrawArrays(GL_TRIANGLES, 0, m_pVertexBuffer->GetLength());
}