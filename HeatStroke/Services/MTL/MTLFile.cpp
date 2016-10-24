//================================================================================
// MTLFile.cpp
// Author: David Hanna
// 
// Loads an MTL file into a collection of data that may be used by other objects.
//================================================================================

#include "MTLFile.h"

std::string HeatStroke::MTLFile::MTLMaterial::ToString() const
{
	std::string strResult;

	strResult += "newmtl " + m_strMaterialName + "\n";
	strResult += "vs " + m_strVertexShaderName + "\n";
	strResult += "fs " + m_strFragmentShaderName + "\n";
	strResult += "Ka " + std::to_string(m_vAmbientColor.r) + " " + std::to_string(m_vAmbientColor.g) + " " + std::to_string(m_vAmbientColor.b) + "\n";
	strResult += "Kd " + std::to_string(m_vDiffuseColor.r) + " " + std::to_string(m_vDiffuseColor.g) + " " + std::to_string(m_vDiffuseColor.b) + "\n";
	strResult += "Ks " + std::to_string(m_vSpecularColor.r) + " " + std::to_string(m_vSpecularColor.g) + " " + std::to_string(m_vSpecularColor.b) + "\n";
	strResult += "Ns " + std::to_string(m_fSpecularExponent) + "\n";
	strResult += "d " + std::to_string(m_fOpacity) + "\n";
	strResult += "map_Ka " + m_strAmbientTextureFileName + "\n";
	strResult += "map_Kd " + m_strDiffuseTextureFileName + "\n";
	strResult += "map_Ks " + m_strSpecularTextureFileName + "\n";
	strResult += "map_Ns " + m_strSpecularExponentTextureFileName + "\n";
	strResult += "map_d " + m_strAlphaTextureFileName + "\n";

	return strResult;
}

HeatStroke::MTLFile::MTLFile(const std::string& p_strMTLFileName)
	:
	MTL_FILE_NAME(p_strMTLFileName),
	m_bLoaded(false),
	m_strMTLFileData(),
	m_vMaterials()
{
}

bool HeatStroke::MTLFile::ParseFile()
{
	if (m_bLoaded)
	{
		return true;
	}

	m_strMTLFileData = HeatStroke::Common::LoadWholeFile(MTL_FILE_NAME);

	// break file down line by line and loop over each line.
	std::vector<std::string> strLines = Common::StringSplit(m_strMTLFileData, "\n");
	std::vector<std::string>::iterator linesIt = strLines.begin(), linesEnd = strLines.end();
	while (linesIt != linesEnd)
	{
		// break the line down into space-separated tabs
		std::vector<std::string> strTabs = Common::StringSplit(*linesIt, " ");

		// the first tab in a line determines the format and meaning of the rest of the line
		std::string strLineKey = strTabs[0];

		// Until we encounter a newmtl line, nothing else matters.
		if (strTabs[0] == "newmtl")
		{
			MTLMaterial mMTLMaterial;
			mMTLMaterial.m_strMaterialName = strTabs[1];

			linesIt++;
			strTabs = Common::StringSplit(*linesIt, " ");

			// assume the following lines apply to the current material until we find a new "newmtl" line.
			while (linesIt != linesEnd && strTabs[0] != "newmtl")
			{
				strTabs = Common::StringSplit(*linesIt, " ");

				if (strTabs[0] == "vs")
				{
					mMTLMaterial.m_strVertexShaderName = strTabs[1];
				}
				else if (strTabs[0] == "fs")
				{
					mMTLMaterial.m_strFragmentShaderName = strTabs[1];
				}
				else if (strTabs[0] == "Ka")
				{
					mMTLMaterial.m_vAmbientColor = glm::vec3(atof(strTabs[1].c_str()), atof(strTabs[2].c_str()), atof(strTabs[3].c_str()));
				}
				else if (strTabs[0] == "Kd")
				{
					mMTLMaterial.m_vDiffuseColor = glm::vec3(atof(strTabs[1].c_str()), atof(strTabs[2].c_str()), atof(strTabs[3].c_str()));
				}
				else if (strTabs[0] == "Ks")
				{
					mMTLMaterial.m_vSpecularColor = glm::vec3(atof(strTabs[1].c_str()), atof(strTabs[2].c_str()), atof(strTabs[3].c_str()));
				}
				else if (strTabs[0] == "Ns")
				{
					mMTLMaterial.m_fSpecularExponent = static_cast<float>(atof(strTabs[1].c_str()));
				}
				else if (strTabs[0] == "d")
				{
					mMTLMaterial.m_fOpacity = static_cast<float>(atof(strTabs[1].c_str()));
				}
				else if (strTabs[0] == "map_Ka")
				{
					mMTLMaterial.m_strAmbientTextureFileName = strTabs[1];
				}
				else if (strTabs[0] == "map_Kd")
				{
					mMTLMaterial.m_strDiffuseTextureFileName = strTabs[1];
				}
				else if (strTabs[0] == "map_Ks")
				{
					mMTLMaterial.m_strSpecularTextureFileName = strTabs[1];
				}
				else if (strTabs[0] == "map_Ns")
				{
					mMTLMaterial.m_strSpecularExponentTextureFileName = strTabs[1];
				}
				else if (strTabs[0] == "map_d")
				{
					mMTLMaterial.m_strAlphaTextureFileName = strTabs[1];
				}

				linesIt++;
			}

			m_vMaterials.push_back(mMTLMaterial);
		}
		else
		{
			linesIt++;
		}
	}

	return (m_bLoaded = true);
}

const HeatStroke::MTLFile::MTLMaterial& HeatStroke::MTLFile::GetMaterial(const std::string& p_strMaterialName) const
{
	std::vector<const MTLMaterial>::const_iterator it = m_vMaterials.begin(), end = m_vMaterials.end();
	for (; it != end; it++)
	{
		if (it->GetMaterialName == p_strMaterialName)
		{
			return *it;
		}
	}
}

std::string HeatStroke::MTLFile::ToString() const
{
	std::string strResult;
	
	strResult += "MTL File Name: " + MTL_FILE_NAME + "\n";
	
	std::vector<const MTLMaterial>::const_iterator it = m_vMaterials.begin(), end = m_vMaterials.end();
	for (; it != end; it++)
	{
		strResult += it->ToString() + "\n";
	}

	return strResult;
}