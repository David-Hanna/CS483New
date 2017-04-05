#ifndef OBJECT_FILE_H
#define OBJECT_FILE_H

#include <string>
#include <vector>

class ObjectFile
{
public:
	enum FileStatus
	{
		ERROR = 0,
		OK
	};

	struct Vertex
	{
		unsigned int m_uiPositionIndex;
		unsigned int m_uiUVIndex;
		unsigned int m_uiNormalIndex;
	};
	struct Face
	{
		Vertex m_Vert1;
		Vertex m_Vert2;
		Vertex m_Vert3;
	};
	struct Object
	{
		std::vector<Face> m_vFaces;
		std::string m_strObjectName;
		std::string m_strMaterialName;
	};

	ObjectFile();
	~ObjectFile();

	FileStatus LoadFromFile(const std::string& p_strFileName);

	const std::vector<Object>& GetObjects() const { return m_vObjects; }
	const std::vector<float>& GetPositions() const { return m_vPositions; }
	const std::vector<float>& GetUVs() const { return m_vUVs; }
	const std::vector<float>& GetNormals() const { return m_vNormals; }
	const std::string& GetMaterialFile() const { return m_strMaterialFileName; }

private:
	std::vector<Object> m_vObjects;
	std::vector<float> m_vPositions;
	std::vector<float> m_vUVs;
	std::vector<float> m_vNormals;

	std::string m_strMaterialFileName;

	void ParseLine(const char* p_cstrLine);
	void ParseFace(const char* p_cstrLine);
	void ParseVertex(const char* p_cstrLine);
	void ParseUV(const char* p_cstrLine);
	void ParseNormal(const char* p_cstrLine);
	void ParseObject(const char* p_cstrLine);
	void ParseUseMTL(const char* p_cstrLine);
	void ParseMTLLib(const char* p_cstrLine);

};

#endif // !OBJECT_FILE_H
