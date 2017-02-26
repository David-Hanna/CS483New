//================================================================================
// OBJFile.cpp
// Author: David Hanna
// 
// Loads an OBJ file into a collection of data that may be used by other objects.
//================================================================================

#include "OBJFile.h"

std::string HeatStroke::OBJFile::OBJVertex::ToString() const
{
	std::string strResult;

	strResult += std::to_string(m_uiPositionIndex);
	strResult += "/";
	strResult += std::to_string(m_uiUVIndex);
	strResult += "/";
	strResult += std::to_string(m_uiNormalIndex);

	return strResult;
}

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

	std::FILE* pFile = std::fopen(OBJ_FILE_NAME.c_str(), "r");
	if (pFile)
	{
		const int iLineLength = 80;
		char cstrLine[iLineLength];

		while (std::fgets(cstrLine, iLineLength, pFile))
		{
			ParseLine(cstrLine);
		}

		fclose(pFile);
		m_bLoaded = true;
		return true;
	}
	else
	{
		return false;
	}
}

void HeatStroke::OBJFile::ParseLine(const char* p_cstrLine)
{
	static char s_cstrLineType[10];

	sscanf(p_cstrLine, "%s", s_cstrLineType);
	if (std::strcmp(s_cstrLineType, "f") == 0)
	{
		ParseFace(p_cstrLine);
	}
	else if (std::strcmp(s_cstrLineType, "v") == 0)
	{
		ParseVertex(p_cstrLine);
	}
	else if (std::strcmp(s_cstrLineType, "vt") == 0)
	{
		ParseUV(p_cstrLine);
	}
	else if (std::strcmp(s_cstrLineType, "vn") == 0)
	{
		ParseNormal(p_cstrLine);
	}
	else if (std::strcmp(s_cstrLineType, "o") == 0)
	{
		ParseObject(p_cstrLine);
	}
	else if (std::strcmp(s_cstrLineType, "usemtl") == 0)
	{
		ParseUseMTL(p_cstrLine);
	}
	else if (std::strcmp(s_cstrLineType, "mtllib") == 0)
	{
		ParseMTLLib(p_cstrLine);
	}
}

void HeatStroke::OBJFile::ParseFace(const char* p_cstrLine)
{
	static OBJVertex s_v1;
	static OBJVertex s_v2;
	static OBJVertex s_v3;

	sscanf(p_cstrLine, "f %u/%u/%u %u/%u/%u %u/%u/%u",
		&s_v1.m_uiPositionIndex, &s_v1.m_uiUVIndex, &s_v1.m_uiNormalIndex,
		&s_v2.m_uiPositionIndex, &s_v2.m_uiUVIndex, &s_v2.m_uiNormalIndex,
		&s_v3.m_uiPositionIndex, &s_v3.m_uiUVIndex, &s_v3.m_uiNormalIndex);

	--s_v1.m_uiPositionIndex;
	--s_v1.m_uiUVIndex;
	--s_v1.m_uiNormalIndex;
	--s_v2.m_uiPositionIndex;
	--s_v2.m_uiUVIndex;
	--s_v2.m_uiNormalIndex;
	--s_v3.m_uiPositionIndex;
	--s_v3.m_uiUVIndex;
	--s_v3.m_uiNormalIndex;

	OBJFace face;
	face.push_back(s_v1);
	face.push_back(s_v2);
	face.push_back(s_v3);
	m_vOBJObjectList.back().m_vFaces.push_back(face);
}

void HeatStroke::OBJFile::ParseVertex(const char* p_cstrLine)
{
	static glm::vec3 s_vPos(0.0f, 0.0f, 0.0f);

	sscanf(p_cstrLine, "v %f %f %f", &s_vPos.x, &s_vPos.y, &s_vPos.z);
	m_vPositions.push_back(s_vPos);
}

void HeatStroke::OBJFile::ParseUV(const char* p_cstrLine)
{
	static glm::vec2 s_vUV(0.0f, 0.0f);

	sscanf(p_cstrLine, "vt %f %f", &s_vUV.x, &s_vUV.y);
	m_vUVs.push_back(s_vUV);
}

void HeatStroke::OBJFile::ParseNormal(const char* p_cstrLine)
{
	static glm::vec3 s_vNormal(0.0f, 0.0f, 0.0f);

	sscanf(p_cstrLine, "vn %f %f %f", &s_vNormal.x, &s_vNormal.y, &s_vNormal.z);
	m_vNormals.push_back(s_vNormal);
}

void HeatStroke::OBJFile::ParseObject(const char* p_cstrLine)
{
	static char s_cstrObjectName[80];

	sscanf(p_cstrLine, "o %s", s_cstrObjectName);
	m_vOBJObjectList.push_back(OBJObject());
	m_vOBJObjectList.back().m_strObjectName = s_cstrObjectName;
}

void HeatStroke::OBJFile::ParseUseMTL(const char* p_cstrLine)
{
	static char s_cstrMaterialName[80];

	sscanf(p_cstrLine, "usemtl %s", s_cstrMaterialName);
	m_vOBJObjectList.back().m_strMaterialName = s_cstrMaterialName;
}

void HeatStroke::OBJFile::ParseMTLLib(const char* p_cstrLine)
{
	static char s_cstrMaterialFile[80];

	sscanf(p_cstrLine, "mtllib %s", s_cstrMaterialFile);
	m_strMTLFileName = s_cstrMaterialFile;
}

std::string HeatStroke::OBJFile::ToString() const
{
	std::string strResult;

	strResult += "OBJ File Name: " + OBJ_FILE_NAME + "\n";
	strResult += "mtllib " + m_strMTLFileName + "\n";

	std::vector<const glm::vec3>::const_iterator positionIt = m_vPositions.begin(), positionEnd = m_vPositions.end();
	for (; positionIt != positionEnd; positionIt++)
	{
		strResult += "v " + std::to_string((*positionIt)[0]) + " " + std::to_string((*positionIt)[1]) + " " + std::to_string((*positionIt)[2]) + "\n";
	}

	std::vector<const glm::vec2>::const_iterator UVIt = m_vUVs.begin(), UVEnd = m_vUVs.end();
	for (; UVIt != UVEnd; UVIt++)
	{
		strResult += "vt " + std::to_string((*UVIt)[0]) + " " + std::to_string((*UVIt)[1]) + "\n";
	}

	std::vector<const glm::vec3>::const_iterator normalIt = m_vNormals.begin(), normalEnd = m_vNormals.end();
	for (; normalIt != normalEnd; normalIt++)
	{
		strResult += "vn " + std::to_string((*normalIt)[0]) + " " + std::to_string((*normalIt)[1]) + " " + std::to_string((*normalIt)[2]) + "\n";
	}

	OBJObjectList::const_iterator objIt = m_vOBJObjectList.begin(), objEnd = m_vOBJObjectList.end();
	for (; objIt != objEnd; objIt++)
	{
		strResult += "o " + objIt->m_strObjectName + "\n";

		std::vector<const OBJFace>::const_iterator faceIt = objIt->m_vFaces.begin(), faceEnd = objIt->m_vFaces.end();
		for (; faceIt != faceEnd; faceIt++)
		{
			strResult += "f ";

			OBJFace::const_iterator vertexIt = faceIt->begin(), vertexEnd = faceIt->end();
			for (; vertexIt != vertexEnd; vertexIt++)
			{
				strResult += vertexIt->ToString() + " ";
			}

			strResult += "\n";
		}

		strResult += "usemtl " + objIt->m_strMaterialName + "\n";
	}

	return strResult;
}