//================================================================================
// OBJFile.cpp
// Author: David Hanna
// 
// Loads an OBJ file into a collection of data that may be used by other objects.
//================================================================================

#include "OBJFile.h"

HeatStroke::OBJFile::OBJVertex::OBJVertex(const unsigned int p_uiPositionIndex, const unsigned int p_uiUVIndex, const unsigned int p_uiNormalIndex) :
	m_uiPositionIndex(p_uiPositionIndex),
	m_uiUVIndex(p_uiUVIndex),
	m_uiNormalIndex(p_uiNormalIndex)
{
}

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

HeatStroke::OBJFile::OBJFile(std::string& p_strOBJFileName) :
	m_bLoaded(false),
	m_strOBJFileName(p_strOBJFileName),
	m_strOBJFileData(),
	m_strObjectName(),
	m_strMTLFileName(),
	m_vPositions(),
	m_vUVs(),
	m_vNormals(),
	m_strMaterialName(),
	m_bSmoothShading(false),
	m_vFaces()
{
}

bool HeatStroke::OBJFile::ParseFile()
{
	if (m_bLoaded)
	{
		return true;
	}

	m_strOBJFileData = Common::LoadWholeFile(m_strOBJFileName);
	
	// break file down line by line and loop over each line.
	std::vector<std::string> strLines = Common::StringSplit(m_strOBJFileData, "\n");
	std::vector<std::string>::iterator linesIt = strLines.begin(), linesEnd = strLines.end();
	for (; linesIt != linesEnd; linesIt++)
	{
		// break the line down into space-separated tabs
		std::vector<std::string> strTabs = Common::StringSplit(*linesIt, " ");

		// the first tab in a line determines the format and meaning of the rest of the line
		std::string strLineKey = strTabs[0];
		
		if (strLineKey == "v") // parse a position
		{
			m_vPositions.push_back(glm::vec3(atof(strTabs[1].c_str()), atof(strTabs[2].c_str()), atof(strTabs[3].c_str())));
		}
		else if (strLineKey == "vt") // parse a texture coord
		{
			m_vUVs.push_back(glm::vec2(atof(strTabs[1].c_str()), atof(strTabs[2].c_str())));
		}
		else if (strLineKey == "vn") // parse a normal
		{
			m_vNormals.push_back(glm::vec3(atof(strTabs[1].c_str()), atof(strTabs[2].c_str()), atof(strTabs[3].c_str())));
		}
		else if (strLineKey == "f") // parse a face (triangle)
		{
			OBJFace vOBJFace;

			// iterate over the tabs
			std::vector<std::string>::iterator faceIt = strTabs.begin(), faceEnd = strTabs.end();
			// skip the key in the iterator
			faceIt++;
			for (; faceIt != faceEnd; faceIt++)
			{
				// split the tab into the parts of the vertex separated by '/'
				std::vector<std::string> strVertex = Common::StringSplit(*faceIt, "/");

				// iterate over the parts of the vertex (v/vt/vn)
				OBJVertex mOBJVertex(std::stoul(strVertex[0]), std::stoul(strVertex[1]), std::stoul(strVertex[2]));
				vOBJFace.push_back(mOBJVertex);
			}

			m_vFaces.push_back(vOBJFace);
		}
		else if (strLineKey == "o") // name of object being parsed
		{
			m_strOBJFileName = strTabs[1];
		}
		else if (strLineKey == "mtllib") // name of mtl file to find the material to use for this object
		{
			m_strMTLFileName = strTabs[1];
		}
		else if (strLineKey == "usemtl") // name of the material to use for this object
		{
			m_strMaterialName = strTabs[1];
		}
		else if (strLineKey == "s") // smooth shading (not implemented)
		{
			if (strTabs[1] == "off")
			{
				m_bSmoothShading = false;
			}
		}
	}

	m_bLoaded = true;

	return true;
}

std::string HeatStroke::OBJFile::ToString() const
{
	std::string strResult;

	strResult += "OBJ File Name: " + m_strOBJFileName + "\n";
	strResult += "Object Name: " + m_strObjectName + "\n";
	
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

	std::vector<const OBJFace>::const_iterator faceIt = m_vFaces.begin(), faceEnd = m_vFaces.end();
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

	strResult += "mtllib " + m_strMTLFileName + "\n";
	strResult += "usemtl " + m_strMaterialName + "\n";
	strResult += "s " + std::to_string(m_bSmoothShading)+ "\n";

	return strResult;
}