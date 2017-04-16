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

		// Load all the data into vectors in an order that our vertex declaration (below) will expect.
		std::vector<float> vVertexData;
		std::vector<unsigned short> vIndexData;

		// Create a map to track existing vertices for index buffer
		VertexToIndexMap mExistingVerticesMap;

		std::vector<OBJFile::OBJFace>::const_iterator triangleIt = vTriangles.begin(), triangleEnd = vTriangles.end();
		for (; triangleIt != triangleEnd; triangleIt++)
		{
			AddToVertexData(vVertexData, vIndexData, triangleIt->m_Vert1, vPositions, vNormals, vUVs, mExistingVerticesMap);
			AddToVertexData(vVertexData, vIndexData, triangleIt->m_Vert2, vPositions, vNormals, vUVs, mExistingVerticesMap);
			AddToVertexData(vVertexData, vIndexData, triangleIt->m_Vert3, vPositions, vNormals, vUVs, mExistingVerticesMap);
		}

		unsigned int uiVertexDataLength = sizeof(float) * vVertexData.size();
		unsigned int uiIndexDataLength = vIndexData.size();

		Mesh mMesh;

		mMesh.m_pVertexBuffer = HeatStroke::BufferManager::CreateVertexBuffer(&(vVertexData[0]), uiVertexDataLength);
		mMesh.m_pIndexBuffer = HeatStroke::BufferManager::CreateIndexBuffer(&(vIndexData[0]), uiIndexDataLength);

		mMesh.m_pVertexDeclaration = new HeatStroke::VertexDeclaration;
		mMesh.m_pVertexDeclaration->Begin();

		mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Position, 3, HeatStroke::CT_Float, 0);
		mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_TexCoord1, 2, HeatStroke::CT_Float, 3 * sizeof(float));
		mMesh.m_pVertexDeclaration->AppendAttribute(HeatStroke::AT_Normal, 3, HeatStroke::CT_Float, 5 * sizeof(float));

		mMesh.m_pVertexDeclaration->SetVertexBuffer(mMesh.m_pVertexBuffer);
		mMesh.m_pVertexDeclaration->SetIndexBuffer(mMesh.m_pIndexBuffer);
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
		HeatStroke::BufferManager::DestroyBuffer(meshIt->m_pIndexBuffer);
		DELETE_IF(meshIt->m_pVertexDeclaration);
		HeatStroke::MaterialManager::DestroyMaterial(meshIt->m_pMaterial);
	}
}

void HeatStroke::Model::AddToVertexData(
	std::vector<float>& p_vVertexData,
	std::vector<unsigned short>& p_vIndexData,
	const OBJFile::OBJVertex & p_Vertex,
	const std::vector<const glm::vec3>& p_vPositions,
	const std::vector<const glm::vec3>& p_vNormals,
	const std::vector<const glm::vec2>& p_vUVs,
	VertexToIndexMap& p_mExistingVerticesMap)
{
	// TODO: Custome objects added to karts are missing texture information in .obj file
	// i.e. Speedster's Engine_Cube (line 3663) does not have any lines beginning with "vt"
	unsigned int uvIndex = (p_Vertex.m_uiUVIndex == -1 ? p_vUVs.size() - 1 : p_Vertex.m_uiUVIndex);

	auto find = p_mExistingVerticesMap.find(p_Vertex);
	if (find != p_mExistingVerticesMap.end())
	{
		p_vIndexData.push_back(find->second);
		return;
	}

	glm::vec3 vertexPosition = p_vPositions.at(p_Vertex.m_uiPositionIndex);
	glm::vec3 vertexNormal = p_vNormals.at(p_Vertex.m_uiNormalIndex);
	glm::vec2 vertexUVs = p_vUVs.at(uvIndex);

	p_vVertexData.push_back(vertexPosition.x);
	p_vVertexData.push_back(vertexPosition.y);
	p_vVertexData.push_back(vertexPosition.z);

	p_vVertexData.push_back(vertexUVs.x);
	p_vVertexData.push_back(vertexUVs.y);

	p_vVertexData.push_back(vertexNormal.x);
	p_vVertexData.push_back(vertexNormal.y);
	p_vVertexData.push_back(vertexNormal.z);

	unsigned short usIndex = static_cast<unsigned short>(p_vVertexData.size() / 8) - 1;
	p_vIndexData.push_back(usIndex);
	p_mExistingVerticesMap[p_Vertex] = usIndex;
}