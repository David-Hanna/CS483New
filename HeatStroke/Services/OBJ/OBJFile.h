//================================================================================
// OBJFile.h
// Author: David Hanna
// 
// Loads an OBJ file into a collection of data that may be used by other objects.
//================================================================================

#ifndef OBJ_FILE_H
#define OBJ_FILE_H

#include "Common.h"
#include "MTLFile.h"

#include <assert.h>
#include <string>
#include <vector>
#include <glm.hpp>

namespace HeatStroke
{
	class OBJFile
	{
	public:
		class OBJVertex
		{
			friend class OBJFile;

		public:
			unsigned int GetPositionIndex() const	{ return m_uiPositionIndex; }
			unsigned int GetUVIndex()		const	{ return m_uiUVIndex; }
			unsigned int GetNormalIndex()	const	{ return m_uiNormalIndex; }

			std::string ToString() const;

		private:
			unsigned int m_uiPositionIndex;
			unsigned int m_uiUVIndex;
			unsigned int m_uiNormalIndex;

			OBJVertex() {}
		};
		typedef std::vector<const OBJVertex> OBJFace;

		class OBJObject
		{
			friend class OBJFile;

		public:
			const std::string&					GetObjectName()		const	{ return m_strObjectName; }
			const std::vector<const OBJFace>&	GetFaces()			const	{ return m_vFaces; }
			const std::string&					GetMaterialName()	const	{ return m_strMaterialName; }

		private:
			std::string						m_strObjectName;
			std::vector<const OBJFace>		m_vFaces;
			std::string						m_strMaterialName;

			OBJObject() {}
		};
		typedef std::vector<const OBJObject> OBJObjectList;

	public:
		OBJFile(const std::string& p_strOBJFileName);

		// Data hasn't been loaded until this method is called. Returns false if failed to load.
		bool ParseFile();

		// Behaviour of accessors besides isLoaded() and getOBJFileName()
		// are undefined when isLoaded() == false.
		bool								IsLoaded()			const	{ return m_bLoaded; }
		const std::string&					GetOBJFileName()	const	{ return OBJ_FILE_NAME; }
		const std::string&					GetMTLFileName()	const	{ return m_strMTLFileName; }

		const std::vector<const glm::vec3>&	GetPositions()		const	{ return m_vPositions; }
		const std::vector<const glm::vec2>&	GetUVs()			const	{ return m_vUVs; }
		const std::vector<const glm::vec3>&	GetNormals()		const	{ return m_vNormals; }
		const OBJObjectList&				GetOBJObjectList()  const	{ return m_vOBJObjectList; }
		
		std::string							ToString()			const;

	private:
		const std::string				OBJ_FILE_NAME;

		bool							m_bLoaded;
		std::string						m_strMTLFileName;

		std::vector<const glm::vec3>	m_vPositions;
		std::vector<const glm::vec2>	m_vUVs;
		std::vector<const glm::vec3>	m_vNormals;
		OBJObjectList					m_vOBJObjectList;

		void ParseLine(const char* p_cstrLine);
		void ParseFace(const char* p_cstrLine);
		void ParseVertex(const char* p_cstrLine);
		void ParseUV(const char* p_cstrLine);
		void ParseNormal(const char* p_cstrLine);
		void ParseObject(const char* p_cstrLine);
		void ParseUseMTL(const char* p_cstrLine);
		void ParseMTLLib(const char* p_cstrLine);
	};
}

#endif