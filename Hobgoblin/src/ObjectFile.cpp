#include "ObjectFile.h"

ObjectFile::ObjectFile()
	:
	m_vPositions(),
	m_vUVs(),
	m_vNormals(),
	m_strMaterialFileName("placeholder.mtl")
{
}

ObjectFile::~ObjectFile()
{
}

ObjectFile::FileStatus ObjectFile::LoadFromFile(const std::string& p_strFileName)
{
	std::FILE* pFile = std::fopen(p_strFileName.c_str(), "r");
	if (pFile)
	{
		const int iLineLength = 80;
		char cstrLine[iLineLength];

		while (std::fgets(cstrLine, iLineLength, pFile))
		{
			ParseLine(cstrLine);
		}

		fclose(pFile);
		return OK;
	}
	else
	{
		return ERROR;
	}
}

void ObjectFile::ParseLine(const char* p_cstrLine)
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

void ObjectFile::ParseFace(const char* p_cstrLine)
{
	static Vertex s_v1;
	static Vertex s_v2;
	static Vertex s_v3;

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

	Face face;
	face.m_Vert1 = s_v1;
	face.m_Vert2 = s_v2;
	face.m_Vert3 = s_v3;
	m_vObjects.back().m_vFaces.push_back(face);
}

void ObjectFile::ParseVertex(const char* p_cstrLine)
{
	static float s_fPositions[3] = {0.0f, 0.0f, 0.0f};
	static int s_iPositionsSize = sizeof(s_fPositions) / sizeof(float);

	sscanf(p_cstrLine, "v %f %f %f", &s_fPositions[0], &s_fPositions[1], &s_fPositions[2]);
	m_vPositions.insert(m_vPositions.end(), s_fPositions, s_fPositions + s_iPositionsSize);
}

void ObjectFile::ParseUV(const char* p_cstrLine)
{
	static float s_fUVs[2] = {0.0f, 0.0f};
	static int s_iUVSize = sizeof(s_fUVs) / sizeof(float);

	sscanf(p_cstrLine, "vt %f %f", &s_fUVs[0], &s_fUVs[1]);
	m_vUVs.insert(m_vUVs.end(), s_fUVs, s_fUVs + s_iUVSize);
}

void ObjectFile::ParseNormal(const char* p_cstrLine)
{
	static float s_fNormals[3] = {0.0f, 0.0f, 0.0f};
	static int s_iNormalSize = sizeof(s_fNormals) / sizeof(float);

	sscanf(p_cstrLine, "vn %f %f %f", &s_fNormals[0], &s_fNormals[1], &s_fNormals[2]);
	m_vNormals.insert(m_vNormals.end(), s_fNormals, s_fNormals + s_iNormalSize);
}

void ObjectFile::ParseObject(const char* p_cstrLine)
{
	static char s_cstrObjectName[80];

	sscanf(p_cstrLine, "o %s", s_cstrObjectName);
	m_vObjects.push_back(Object());
	m_vObjects.back().m_strObjectName = s_cstrObjectName;
}

void ObjectFile::ParseUseMTL(const char* p_cstrLine)
{
	static char s_cstrMaterialName[80];

	sscanf(p_cstrLine, "usemtl %s", s_cstrMaterialName);
	m_vObjects.back().m_strMaterialName = s_cstrMaterialName;
}

void ObjectFile::ParseMTLLib(const char* p_cstrLine)
{
	static char s_cstrMaterialFile[80];

	sscanf(p_cstrLine, "mtllib %s", s_cstrMaterialFile);
	m_strMaterialFileName = s_cstrMaterialFile;
}