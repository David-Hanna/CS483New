#include <fstream>
#include <iostream>
#include <string>

#include "ObjectFile.h"

void ConvertFileToBin(const std::string& p_strFileName);
std::string GetObjectName(const std::string& p_strFileName);

void WriteToBinFile(const std::string& p_strFileName, const ObjectFile& p_ObjectFile);
void WriteMaterialFileNameToBin(std::fstream& p_fsBinFile, const std::string& p_strMaterialFileName);
void WriteVertsToBin(std::fstream& p_fsBinFile, const std::vector<float>& p_vVerts);
void WriteUVsToBin(std::fstream& p_fsBinFile, const std::vector<float>& p_vUVs);
void WriteNormalsToBin(std::fstream& p_fsBinFile, const std::vector<float>& p_vNormals);
void WriteObjectsToBin(std::fstream& p_fsBinFile, const std::vector<ObjectFile::Object>& p_vObjects);

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		ConvertFileToBin(argv[i]);
	}

	return 0;
}

void ConvertFileToBin(const std::string& p_strFileName)
{
	std::cout << "converting " << p_strFileName << "..." << std::endl;

	std::string strObjectName = GetObjectName(p_strFileName);

	ObjectFile obj;
	if (obj.LoadFromFile(p_strFileName) == ObjectFile::OK)
	{
		WriteToBinFile(strObjectName, obj);

		std::cout << p_strFileName << " converted successfully" << std::endl;
	}
	else
	{
		std::cout << "ERROR: Failed to load " << p_strFileName << ". Skipping..." << std::endl;
	}
}

std::string GetObjectName(const std::string& p_strFileName)
{
	// remove extension from file name
	return p_strFileName.substr(0, p_strFileName.size() - 4);
}

void WriteToBinFile(const std::string& p_strFileName, const ObjectFile& p_ObjectFile)
{
	std::fstream fsBinFile(p_strFileName + ".hobj", std::ios::out | std::ios::binary);
	fsBinFile.seekp(0);

	WriteMaterialFileNameToBin(fsBinFile, p_ObjectFile.GetMaterialFile());
	WriteVertsToBin(fsBinFile, p_ObjectFile.GetPositions());
	WriteUVsToBin(fsBinFile, p_ObjectFile.GetUVs());
	WriteNormalsToBin(fsBinFile, p_ObjectFile.GetNormals());
	WriteObjectsToBin(fsBinFile, p_ObjectFile.GetObjects());

	fsBinFile.close();
}

void WriteMaterialFileNameToBin(std::fstream & p_fsBinFile, const std::string & p_strMaterialFileName)
{
	uint32_t uiSize = p_strMaterialFileName.length();
	p_fsBinFile.write((char*)&uiSize, sizeof(uiSize));
	p_fsBinFile.write((char*)&p_strMaterialFileName[0], uiSize * sizeof(char));
}

void WriteVertsToBin(std::fstream& p_fsBinFile, const std::vector<float>& p_vVerts)
{
	uint32_t uiSize = p_vVerts.size();
	p_fsBinFile.write((char*)&uiSize, sizeof(uiSize));
	p_fsBinFile.write((char*)&p_vVerts[0], uiSize * sizeof(float));
}

void WriteUVsToBin(std::fstream& p_fsBinFile, const std::vector<float>& p_vUVs)
{
	uint32_t uiSize = p_vUVs.size();
	p_fsBinFile.write((char*)&uiSize, sizeof(uiSize));
	p_fsBinFile.write((char*)&p_vUVs[0], uiSize * sizeof(float));
}

void WriteNormalsToBin(std::fstream& p_fsBinFile, const std::vector<float>& p_vNormals)
{
	uint32_t uiSize = p_vNormals.size();
	p_fsBinFile.write((char*)&uiSize, sizeof(uiSize));
	p_fsBinFile.write((char*)&p_vNormals[0], uiSize * sizeof(float));
}

void WriteObjectsToBin(std::fstream & p_fsBinFile, const std::vector<ObjectFile::Object>& p_vObjects)
{
	uint32_t uiNumObjects = p_vObjects.size();
	p_fsBinFile.write((char*)&uiNumObjects, sizeof(uiNumObjects));

	for (ObjectFile::Object obj : p_vObjects)
	{
		uint32_t uiObjectNameLength = obj.m_strObjectName.size();
		uint32_t uiMaterialNameLength = obj.m_strMaterialName.size();
		uint32_t uiNumFaces = obj.m_vFaces.size();

		p_fsBinFile.write((char*)&uiObjectNameLength, sizeof(uiObjectNameLength));
		p_fsBinFile.write((char*)&(obj.m_strObjectName[0]), uiObjectNameLength * sizeof(char));

		p_fsBinFile.write((char*)&uiMaterialNameLength, sizeof(uiMaterialNameLength));
		p_fsBinFile.write((char*)&(obj.m_strMaterialName[0]), uiMaterialNameLength * sizeof(char));

		p_fsBinFile.write((char*)&uiNumFaces, sizeof(uiNumFaces));
		p_fsBinFile.write((char*)&(obj.m_vFaces[0]), uiNumFaces * sizeof(ObjectFile::Face));
	}
}