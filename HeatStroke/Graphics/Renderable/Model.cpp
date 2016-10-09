//-----------------------------------------------------------------------------
// File:			Model.cpp
// Original Author:	Gordon Wood
//
// See header for notes
//-----------------------------------------------------------------------------

/*

namespace HeatStroke
{
static ComponentType gs_aPODTypeMap[] = 
{
    HeatStroke::CT_Invalid,   //EPODDataNone,
	HeatStroke::CT_Float,		//EPODDataFloat,
	HeatStroke::CT_Int,		//EPODDataInt,
	HeatStroke::CT_UShort,	//EPODDataUnsignedShort,
	HeatStroke::CT_UByte4,	//EPODDataRGBA,
	HeatStroke::CT_UByte4,	//EPODDataARGB,
	HeatStroke::CT_UByte4,	//EPODDataD3DCOLOR,
	HeatStroke::CT_UByte4,	//EPODDataUBYTE4,
	HeatStroke::CT_Invalid,	//EPODDataDEC3N,
	HeatStroke::CT_Invalid,	//EPODDataFixed16_16,
	HeatStroke::CT_UByte,		//EPODDataUnsignedByte,
	HeatStroke::CT_Short,		//EPODDataShort,
	HeatStroke::CT_ShortNorm,	//EPODDataShortNorm,
	HeatStroke::CT_Byte,		//EPODDataByte,
	HeatStroke::CT_ByteNorm,	//EPODDataByteNorm,
	HeatStroke::CT_UByteNorm, //EPODDataUnsignedByteNorm,
	HeatStroke::CT_UShortNorm, //EPODDataUnsignedShortNorm,
	HeatStroke::CT_UInt,		//EPODDataUnsignedInt
};

//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
Model::Model(const std::string& p_strFile, const std::string& p_strTexturePrefix, const std::string& p_strVertexProgram, const std::string& p_strFragmentProgram) 
	:
	m_fFrame(0.0f)
{
	// Load in the POD file
	EPVRTError error = m_pod.ReadFromFile(p_strFile.c_str());
	if (error != PVR_SUCCESS)
	{
		exit(1);
	}

	// Build all the meshes
	for(unsigned int i = 0; i < m_pod.nNumMesh; i++)
	{
		SPODMesh* pMesh = &m_pod.pMesh[i];

		// Create the vertex declaration
		HeatStroke::VertexDeclaration* pDecl = new HeatStroke::VertexDeclaration;
		pDecl->Begin();

        // Create the vertex buffer
		HeatStroke::VertexBuffer* pVB = HeatStroke::BufferManager::CreateVertexBuffer(pMesh->pInterleaved, pMesh->nNumVertex * pMesh->sVertex.nStride);
        
		// Create the index buffer
		HeatStroke::IndexBuffer* pIB = HeatStroke::BufferManager::CreateIndexBuffer(pMesh->nNumFaces*3);
		pIB->Write(pMesh->sFaces.pData);
        
		// We'll always have a position
		pDecl->AppendAttribute(HeatStroke::AT_Position, pMesh->sVertex.n, gs_aPODTypeMap[pMesh->sVertex.eType], *((int*)&pMesh->sVertex.pData));

		if( pMesh->sNormals.n > 0 )
			pDecl->AppendAttribute(HeatStroke::AT_Normal, pMesh->sNormals.n, gs_aPODTypeMap[pMesh->sNormals.eType], *((int*)&pMesh->sNormals.pData));

		for(unsigned int x = 0; x < pMesh->nNumUVW; x++ )
			pDecl->AppendAttribute((HeatStroke::Attribute)(HeatStroke::AT_TexCoord1 + x), pMesh->psUVW[x].n, gs_aPODTypeMap[pMesh->psUVW[x].eType], *((int*)&pMesh->psUVW[x].pData));

		if( pMesh->sVtxColours.n > 0 )
			pDecl->AppendAttribute(HeatStroke::AT_Color, pMesh->sVtxColours.n, gs_aPODTypeMap[pMesh->sVtxColours.eType], *((int*)&pMesh->sVtxColours.pData));

		if( pMesh->sBoneIdx.n != 0 )
			pDecl->AppendAttribute(HeatStroke::AT_BoneIndices, pMesh->sBoneIdx.n, gs_aPODTypeMap[pMesh->sBoneIdx.eType], *((int*)&pMesh->sBoneIdx.pData));

		if( pMesh->sBoneWeight.n != 0 )
			pDecl->AppendAttribute(HeatStroke::AT_BoneWeight, pMesh->sBoneWeight.n, gs_aPODTypeMap[pMesh->sBoneWeight.eType], *((int*)&pMesh->sBoneWeight.pData));

		pDecl->SetVertexBuffer(pVB);
		pDecl->SetIndexBuffer(pIB);
		pDecl->End();
    
		// Add them to our list of meshes
		Mesh m;
		m.m_pVB = pVB;
		m.m_pIB = pIB;
		m.m_pDecl = pDecl;
		m_lMeshes.push_back(m);
	}

	// Now make sure the material manager knows about all materials we'll need
	// CLASS NOTE: This does a bunch of work more than once (like if many nodes
	// reference the same material - which is likely). This is a candidate for
	// cleanup as an exercise
	for(unsigned int i = 0; i < m_pod.nNumMeshNode; i++)
	{
        SPODNode* pNode = &m_pod.pNode[i];

		if (pNode->nIdxMaterial == -1)
		{
			continue;
		}

		SPODMaterial* pPODMat = &m_pod.pMaterial[pNode->nIdxMaterial];
        
		// Not been created yet, so add it
		m_pMaterial = HeatStroke::MaterialManager::CreateMaterial(pPODMat->pszName);

		m_pMaterial->SetProgram(p_strVertexProgram, p_strFragmentProgram);

		// Grab the texture it's using and change the filename it expects to
		// be our TGA converted files, in the right path
        if( pPODMat->nIdxTexDiffuse != -1 )
        {
            SPODTexture* pPODTex = &m_pod.pTexture[pPODMat->nIdxTexDiffuse];
                
            std::string strFilename = pPODTex->pszName;
            if( strFilename.find(".jpg") != std::string::npos )
                strFilename = strFilename.substr(0,strFilename.find(".jpg")) + std::string(".tga");
            else if( strFilename.find(".png") != std::string::npos )
                strFilename = strFilename.substr(0,strFilename.find(".png")) + std::string(".tga");

            strFilename = p_strTexturePrefix + strFilename;

            // Create the texture, make sure it's set to repeat, and assign it
            // to the material
            HeatStroke::Texture* pTex = HeatStroke::TextureManager::CreateTexture(strFilename.c_str());
            pTex->SetWrapMode(HeatStroke::Texture::WM_Repeat);
            m_pMaterial->SetTexture("Sampler1",pTex);
		}

		// Specular map texture if it has one
		if (pPODMat->nIdxTexSpecularColour != -1)
		{
			SPODTexture* pPODTex = &m_pod.pTexture[pPODMat->nIdxTexSpecularColour];
			std::string strFilename = pPODTex->pszName;
			if( strFilename.find(".jpg") != std::string::npos )
				strFilename = strFilename.substr(0,strFilename.find(".jpg")) + std::string(".tga");
			else if( strFilename.find(".png") != std::string::npos )
				strFilename = strFilename.substr(0,strFilename.find(".png")) + std::string(".tga");

			strFilename = p_strTexturePrefix + strFilename;

			// Create the texture, make sure it's set to repeat, and assign it
			// to the material
			HeatStroke::Texture* pTex = HeatStroke::TextureManager::CreateTexture(strFilename.c_str());
			pTex->SetWrapMode(HeatStroke::Texture::WM_Repeat);
			m_pMaterial->SetTexture("Sampler2",pTex);
		}
        
		// CLASS NOTE: I'm not reading blend modes from the materials in the POD
		// file. This is bad and will lead to incorrect rendering. Add some code
		// to look in the POD file for this data and set it up in the
		// HeatStroke::Material accordingly!
	}
}

//----------------------------------------------------------
// Destructor
//----------------------------------------------------------
Model::~Model()
{
	for(unsigned int i = 0; i < m_lMeshes.size(); i++)
	{
		const Mesh& m = m_lMeshes[i];
		HeatStroke::BufferManager::DestroyBuffer(m.m_pVB);
		HeatStroke::BufferManager::DestroyBuffer(m.m_pIB);
		delete m.m_pDecl;
	}
}

//----------------------------------------------------------
// Updates this model
//----------------------------------------------------------
void Model::Update(float p_fDelta)
{
	m_fFrame += (p_fDelta * 30.0f);
	if(m_fFrame > m_pod.nNumFrame - 1)
	{
		m_fFrame = 0;
	}
	m_pod.SetFrame(m_fFrame);
}

//----------------------------------------------------------
// Renders this model
//----------------------------------------------------------
void Model::Render(const Camera* p_pCamera)
{
	// Can't render without a camera.
	assert(p_pCamera != nullptr);

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
}

}



*/