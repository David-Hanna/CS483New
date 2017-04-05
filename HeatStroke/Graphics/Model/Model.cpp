//-----------------------------------------------------------------------------
// Model
// Author: David Hanna
//
// A renderable model loaded in from .obj and .mtl data.
//-----------------------------------------------------------------------------

#include "Model.h"

HeatStroke::Model::Model(const std::string& p_mOBJFileName)
{
	// Load the OBJ file.
	OBJFile mOBJFile(p_mOBJFileName);
	mOBJFile.ParseFile();

	// Load the MTL file.
	MTLFile mMTLFile(mOBJFile.GetMTLFileName());
	mMTLFile.ParseFile();

	// Grab the lists of positions, normals, and uvs for convenience.
	const std::vector<const glm::vec3>& vPositions = mOBJFile.GetPositions();
	const std::vector<const glm::vec3>& vNormals = mOBJFile.GetNormals();
	const std::vector<const glm::vec2>& vUVs = mOBJFile.GetUVs();
	
	// Loop over all the OBJObjects in the OBJFile, which will turn into our meshes.
	const OBJFile::OBJObjectList& vOBJObjectList = mOBJFile.GetOBJObjectList();
	OBJFile::OBJObjectList::const_iterator objIt = vOBJObjectList.begin(), objEnd = vOBJObjectList.end();

	for (; objIt != objEnd; objIt++)
	{
		//================================================================
		// Vertex Data
		//================================================================
		// We assume for our models that every face is a triangle.
		auto vTriangles = objIt->m_vFaces;

		// Create the float array representing the vertex buffer data.
		// 3 floats for position, 3 for normals, and 2 for uvs (8 floats) per vertex. 
		// 3 vertices (24 floats) per triangle.
		// 24 floats per triangle * 4 bytes per float = 96 bytes per triangle.
		unsigned int uiVertexDataLength = 24 * sizeof(float) * vTriangles.size();

		// Load all the float data into a vector in an order that our vertex declaration (below) will expect.
		std::vector<float> vVertexData;

		std::vector<OBJFile::OBJFace>::const_iterator triangleIt = vTriangles.begin(), triangleEnd = vTriangles.end();
		for (; triangleIt != triangleEnd; triangleIt++)
		{
			AddToVertexData(vVertexData, triangleIt->m_Vert1, vPositions, vNormals, vUVs);
			AddToVertexData(vVertexData, triangleIt->m_Vert2, vPositions, vNormals, vUVs);
			AddToVertexData(vVertexData, triangleIt->m_Vert3, vPositions, vNormals, vUVs);

		}

		Mesh mMesh;

		mMesh.m_pVertexBuffer = HeatStroke::BufferManager::CreateVertexBuffer(&(vVertexData[0]), uiVertexDataLength);

		mMesh.m_pVertexDeclaration = new HeatStroke::VertexDeclaration;
		mMesh.m_pVertexDeclaration->Begin();

		mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Position, 3, HeatStroke::CT_Float, 0);
		mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Normal, 3, HeatStroke::CT_Float, 3 * sizeof(float));
		mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_TexCoord1, 2, HeatStroke::CT_Float, 6 * sizeof(float));

		mMesh.m_pVertexDeclaration->SetVertexBuffer(mMesh.m_pVertexBuffer);
		mMesh.m_pVertexDeclaration->End();

		//==============================================================
		// Material Data
		//==============================================================
		const MTLFile::MTLMaterial* mMTLMaterial = mMTLFile.GetMaterial(objIt->m_strMaterialName);

		if (mMTLMaterial == nullptr)
		{
#ifdef _DEBUG
			assert(false && "No material by this name.");
#endif
		}
		else
		{
			mMesh.m_pMaterial = HeatStroke::MaterialManager::CreateMaterial(mMTLMaterial->GetMaterialName());
			mMesh.m_pMaterial->SetProgram(mMTLMaterial->GetVertexShaderName(), mMTLMaterial->GetFragmentShaderName());

			mMesh.m_pTexture = HeatStroke::TextureManager::CreateTexture(mMTLMaterial->GetDiffuseTextureFileName());
			mMesh.m_pTexture->SetWrapMode(HeatStroke::Texture::WM_Repeat);

			mMesh.m_pMaterial->SetTexture("DiffuseTexture", mMesh.m_pTexture);
		}

		m_vMeshes.push_back(mMesh);
	}
}


HeatStroke::Model::~Model()
{
	std::vector<Mesh>::iterator meshIt = m_vMeshes.begin(), meshEnd = m_vMeshes.end();
	for (; meshIt != meshEnd; meshIt++)
	{
		HeatStroke::BufferManager::DestroyBuffer(meshIt->m_pVertexBuffer);
		DELETE_IF(meshIt->m_pVertexDeclaration);
		HeatStroke::MaterialManager::DestroyMaterial(meshIt->m_pMaterial);
	}
}

void HeatStroke::Model::AddToVertexData(std::vector<float>& p_vVertexData, const OBJFile::OBJVertex & p_Vertex, const std::vector<const glm::vec3>& p_vPositions, const std::vector<const glm::vec3>& p_vNormals, const std::vector<const glm::vec2>& p_vUVs)
{
	int uvIndex = p_Vertex.m_uiUVIndex; // Temporary fix for crash while loading kart models in debug mode
	if (uvIndex >= p_vUVs.size()) uvIndex = p_vUVs.size() - 1;

	glm::vec3 vertexPosition = p_vPositions[p_Vertex.m_uiPositionIndex];
	glm::vec3 vertexNormal = p_vNormals[p_Vertex.m_uiNormalIndex];
	glm::vec2 vertexUVs = p_vUVs[uvIndex];

	p_vVertexData.push_back(vertexPosition.x);
	p_vVertexData.push_back(vertexPosition.y);
	p_vVertexData.push_back(vertexPosition.z);

	p_vVertexData.push_back(vertexNormal.x);
	p_vVertexData.push_back(vertexNormal.y);
	p_vVertexData.push_back(vertexNormal.z);

	p_vVertexData.push_back(vertexUVs.x);
	p_vVertexData.push_back(vertexUVs.y);
}