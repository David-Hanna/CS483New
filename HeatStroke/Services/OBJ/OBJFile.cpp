//================================================================================
// OBJFile.cpp
// Author: David Hanna
// Modified by: Matthew White
// 
// Loads an OBJ file into a collection of data that may be used by other objects.
//================================================================================

#include "OBJFile.h"

HeatStroke::OBJFile::OBJFile(const std::string& p_strOBJFileName) :
	OBJ_FILE_NAME(p_strOBJFileName),
	m_bLoaded(false),
	m_strMTLFileName(),
	m_vOBJObjectList()
{
}

bool HeatStroke::OBJFile::ParseFile()
{
	if (m_bLoaded)
	{
		return true;
	}

	if (ParseHOBJ())
	{
		m_bLoaded = true;
		return true;
	}
	return false;
}

bool HeatStroke::OBJFile::ParseHOBJ()
{
	std::fstream fsFile(OBJ_FILE_NAME, std::ios::in | std::ios::binary);
	if (!fsFile)
	{
		return false;
	}

	fsFile.seekg(0);

	ParseMaterial(fsFile);
	ParseVertices(fsFile);
	ParseUVs(fsFile);
	ParseNormals(fsFile);
	ParseObjects(fsFile);

	fsFile.close();
	return true;
}

void HeatStroke::OBJFile::ParseMaterial(std::fstream& p_fsHOBJ)
{
	size_t uiMaterialFileNameLength = 0;
	p_fsHOBJ.read(reinterpret_cast<char*>(&uiMaterialFileNameLength), sizeof(size_t));
	char* cstrMaterialFileName = new char[uiMaterialFileNameLength + 1];
	p_fsHOBJ.read(cstrMaterialFileName, uiMaterialFileNameLength * sizeof(char));
	cstrMaterialFileName[uiMaterialFileNameLength] = '\0';

	m_strMTLFileName = cstrMaterialFileName;
	delete[] cstrMaterialFileName;
	cstrMaterialFileName = nullptr;
}

void HeatStroke::OBJFile::ParseVertices(std::fstream& p_fsHOBJ)
{
	size_t uiNumVertices = 0;
	p_fsHOBJ.read(reinterpret_cast<char*>(&uiNumVertices), sizeof(size_t));
	m_vPositions.resize(uiNumVertices / 3);
	p_fsHOBJ.read(reinterpret_cast<char*>(&m_vPositions[0]), uiNumVertices * sizeof(float));
}

void HeatStroke::OBJFile::ParseUVs(std::fstream& p_fsHOBJ)
{
	size_t uiNumUVs = 0;
	p_fsHOBJ.read(reinterpret_cast<char*>(&uiNumUVs), sizeof(size_t));
	m_vUVs.resize(uiNumUVs / 2);
	p_fsHOBJ.read(reinterpret_cast<char*>(&m_vUVs[0]), uiNumUVs * sizeof(float));
}

void HeatStroke::OBJFile::ParseNormals(std::fstream& p_fsHOBJ)
{
	size_t uiNumNormals = 0;
	p_fsHOBJ.read(reinterpret_cast<char*>(&uiNumNormals), sizeof(size_t));
	m_vNormals.resize(uiNumNormals / 3);
	p_fsHOBJ.read(reinterpret_cast<char*>(&m_vNormals[0]), uiNumNormals * sizeof(float));
}

void HeatStroke::OBJFile::ParseObjects(std::fstream& p_fsHOBJ)
{
	size_t uiNumObjects = 0;
	p_fsHOBJ.read(reinterpret_cast<char*>(&uiNumObjects), sizeof(size_t));
	m_vOBJObjectList.resize(uiNumObjects);

	for (int i = 0; i < uiNumObjects; ++i)
	{
		m_vOBJObjectList[i] = OBJObject();

		size_t uiObjectNameLength = 0;
		p_fsHOBJ.read(reinterpret_cast<char*>(&uiObjectNameLength), sizeof(size_t));
		char* cstrObjectName = new char[uiObjectNameLength + 1];
		p_fsHOBJ.read(cstrObjectName, uiObjectNameLength * sizeof(char));
		cstrObjectName[uiObjectNameLength] = '\0';
		m_vOBJObjectList[i].m_strObjectName = cstrObjectName;
		delete[] cstrObjectName;
		cstrObjectName = nullptr;

		size_t uiMaterialNameLength = 0;
		p_fsHOBJ.read(reinterpret_cast<char*>(&uiMaterialNameLength), sizeof(size_t));
		char* cstrMaterialName = new char[uiMaterialNameLength + 1];
		p_fsHOBJ.read(cstrMaterialName, uiMaterialNameLength * sizeof(char));
		cstrMaterialName[uiMaterialNameLength] = '\0';
		m_vOBJObjectList[i].m_strMaterialName = cstrMaterialName;
		delete[] cstrMaterialName;
		cstrMaterialName = nullptr;

		size_t uiNumFaces = 0;
		p_fsHOBJ.read(reinterpret_cast<char*>(&uiNumFaces), sizeof(size_t));
		m_vOBJObjectList[i].m_vFaces.resize(uiNumFaces);
		p_fsHOBJ.read(reinterpret_cast<char*>(&(m_vOBJObjectList[i].m_vFaces[0])), uiNumFaces * sizeof(OBJFace));
	}
}