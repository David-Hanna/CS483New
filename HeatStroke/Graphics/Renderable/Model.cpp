//-----------------------------------------------------------------------------
// File:			Model.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------

#include "Model.h"

//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
HeatStroke::Model::Model(HeatStroke::OBJFile& p_mOBJFile)
{
	// Load in the OBJ file if not already loaded
	if (!p_mOBJFile.IsLoaded())
	{
		p_mOBJFile.ParseFile();
	}
	
	//================================================================
	// Vertex Data
	//================================================================

	// Grab the lists of positions, normals, and uvs for convenience.
	const std::vector<const glm::vec3>& vPositions = p_mOBJFile.GetPositions();
	const std::vector<const glm::vec3>& vNormals = p_mOBJFile.GetNormals();
	const std::vector<const glm::vec2>& vUVs = p_mOBJFile.GetUVs();

	// We assume for our models that every face is a triangle.
	const std::vector<const OBJFile::OBJFace>& vTriangles = p_mOBJFile.GetFaces();

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
	MTLFile mMTLFile(p_mOBJFile.GetMTLFileName());
	mMTLFile.ParseFile();

	const MTLFile::MTLMaterial& mMTLMaterial = mMTLFile.GetMaterial(p_mOBJFile.GetMaterialName());

	m_pMaterial = HeatStroke::MaterialManager::CreateMaterial(mMTLMaterial.GetMaterialName());
	m_pMaterial->SetProgram(mMTLMaterial.GetVertexShaderName(), mMTLMaterial.GetFragmentShaderName());

	HeatStroke::Texture* pTexture = HeatStroke::TextureManager::CreateTexture(mMTLMaterial.GetDiffuseTextureFileName());
	pTexture->SetWrapMode(HeatStroke::Texture::WM_Repeat);

	m_pMaterial->SetTexture("DiffuseTexture", pTexture);
}

//----------------------------------------------------------
// Destructor
//----------------------------------------------------------
HeatStroke::Model::~Model()
{
	if (m_pVertexBuffer != nullptr)
	{
		HeatStroke::BufferManager::DestroyBuffer(m_pVertexBuffer);
		m_pVertexBuffer = nullptr;
	}
	
	if (m_pIndexBuffer != nullptr)
	{
		HeatStroke::BufferManager::DestroyBuffer(m_pIndexBuffer);
		m_pIndexBuffer = nullptr;
	}

	DELETE_IF(m_pVertexDeclaration);
}

//----------------------------------------------------------
// Updates this model
//----------------------------------------------------------
void HeatStroke::Model::Update(float p_fDelta)
{
	/*
	m_fFrame += (p_fDelta * 30.0f);
	if(m_fFrame > m_pod.nNumFrame - 1)
	{
		m_fFrame = 0;
	}
	m_pod.SetFrame(m_fFrame);
	*/
}

//----------------------------------------------------------
// Renders this model
//----------------------------------------------------------
void HeatStroke::Model::Render(const Camera* p_pCamera)
{
	// Can't render without a camera.
	assert(p_pCamera != nullptr);

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

	/*
	// Go through every mesh node in the pod file
	for(unsigned int i = 0; i < m_pod.nNumMeshNode; i++)
	{
        SPODNode* pNode = &m_pod.pNode[i];
		SPODMesh* pMesh = &m_pod.pMesh[i];
        
		// Safeguard
        if( pNode->nIdx == -1 )
            continue;

		// Calculate its world matrix from the POD file
		glm::mat4 mWorld;
        m_pod.GetWorldMatrix(*((PVRTMATRIX*)glm::value_ptr(mWorld)), *pNode);
        mWorld = m_mWorldTransform * mWorld;
		glm::mat4 mWorldView = p_pCamera->GetViewMatrix() * mWorld;

//		m_pMaterial->SetUniform("WorldMatrix", mWorld);
		m_pMaterial->SetUniform("WorldInverseTransposeMatrix", glm::transpose(glm::inverse(glm::mat3(mWorld))));
//		m_pMaterial->SetUniform("WorldViewMatrix", mWorldView);
		m_pMaterial->SetUniform("WorldViewProjectionMatrix", p_pCamera->GetProjectionMatrix() * mWorldView);

		// Bind the source data
		m_lMeshes[pNode->nIdx].m_pDecl->Bind();

		if (pMesh->sBoneIdx.n > 0)
		{
			for(int i32Batch = 0; i32Batch < pMesh->sBoneBatches.nBatchCnt; ++i32Batch)
			{
				// Go through the bones for the current bone batch
				glm::mat4 amBoneWorld2[9];
				glm::mat3 mBoneIT2;

				int i32Count = pMesh->sBoneBatches.pnBatchBoneCnt[i32Batch];
				for(int i = 0; i < i32Count; ++i)
				{
					// Get the Node of the bone
					int i32NodeID = pMesh->sBoneBatches.pnBatches[i32Batch * pMesh->sBoneBatches.nBatchBoneMax + i];

					// Get the World transformation matrix for this bone and combine it with our app defined
					// transformation matrix
					PVRTMat4 mOut = m_pod.GetBoneWorldMatrix(*pNode, m_pod.pNode[i32NodeID]);
					amBoneWorld2[i] = *(glm::mat4*)&mOut;
				}
				m_pMaterial->SetUniform("BoneMatrixArray", amBoneWorld2, i32Count);

				// Find number of triangles to draw for this batch
				int i32Tris;
				if(i32Batch+1 < pMesh->sBoneBatches.nBatchCnt)
					i32Tris = pMesh->sBoneBatches.pnBatchOffset[i32Batch+1] - pMesh->sBoneBatches.pnBatchOffset[i32Batch];
				else
					i32Tris = pMesh->nNumFaces - pMesh->sBoneBatches.pnBatchOffset[i32Batch];

				// Apply the material
				m_pMaterial->Apply();

				// Draw it!
				size_t offset = sizeof(GLushort) * 3 * pMesh->sBoneBatches.pnBatchOffset[i32Batch];
				glDrawElements(GL_TRIANGLES, i32Tris * 3, GL_UNSIGNED_SHORT, (void*) offset);
				GL_CHECK_ERROR(__FILE__, __LINE__);
			}
		}
		else
		{
			// Apply the material
			m_pMaterial->Apply();

			// Draw it!
			glDrawElements(GL_TRIANGLES, m_lMeshes[pNode->nIdx].m_pIB->GetNumIndices(), GL_UNSIGNED_SHORT, 0);
			GL_CHECK_ERROR(__FILE__, __LINE__);
		}
	}
	*/
}