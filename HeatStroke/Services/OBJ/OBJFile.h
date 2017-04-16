//================================================================================
// OBJFile.h
// Author: David Hanna
// Modified by: Matthew White
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
		struct OBJVertex
		{
			unsigned int m_uiPositionIndex;
			unsigned int m_uiUVIndex;
			unsigned int m_uiNormalIndex;
		};
		struct OBJVertexComparator
		{
			// Required to store OBJVertex in a map
			bool operator()(const OBJVertex& lhs, const OBJVertex& rhs) const
			{
				return std::tie(lhs.m_uiPositionIndex, lhs.m_uiUVIndex, lhs.m_uiNormalIndex) <
					std::tie(rhs.m_uiPositionIndex, rhs.m_uiUVIndex, rhs.m_uiNormalIndex);
			}
		};
		struct OBJFace
		{
			OBJVertex m_Vert1;
			OBJVertex m_Vert2;
			OBJVertex m_Vert3;
		};
		struct OBJObject
		{
			std::vector<OBJFace> m_vFaces;
			std::string m_strObjectName;
			std::string m_strMaterialName;
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

	private:
		const std::string				OBJ_FILE_NAME;

		bool							m_bLoaded;
		std::string						m_strMTLFileName;

		std::vector<const glm::vec3>	m_vPositions;
		std::vector<const glm::vec2>	m_vUVs;
		std::vector<const glm::vec3>	m_vNormals;
		OBJObjectList					m_vOBJObjectList;

		bool ParseHOBJ();
		void ParseMaterial(std::fstream& p_fsHOBJ);
		void ParseVertices(std::fstream& p_fsHOBJ);
		void ParseUVs(std::fstream& p_fsHOBJ);
		void ParseNormals(std::fstream& p_fsHOBJ);
		void ParseObjects(std::fstream& p_fsHOBJ);
	};
}

#endif