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
		public:
			OBJVertex(const unsigned int p_uiPositionIndex, const unsigned int p_uiUVIndex, const unsigned int p_uiNormalIndex);

			unsigned int GetPositionIndex() const	{ return m_uiPositionIndex; }
			unsigned int GetUVIndex() const			{ return m_uiUVIndex; }
			unsigned int GetNormalIndex() const		{ return m_uiNormalIndex; }

			std::string ToString() const;

		private:
			const unsigned int m_uiPositionIndex;
			const unsigned int m_uiUVIndex;
			const unsigned int m_uiNormalIndex;
		};

		typedef std::vector<OBJVertex> OBJFace;

	public:
		OBJFile(std::string& p_strOBJFileName);

		// Data hasn't been loaded until this method is called. Returns false if failed to load.
		bool ParseFile();

		// Behaviour of accessors besides isLoaded() and getOBJFileName()
		// are undefined when isLoaded() == false.
		bool								IsLoaded()			const	{ return m_bLoaded; }
		const std::string&					GetOBJFileName()	const	{ return m_strOBJFileName; }
		const std::string&					GetObjectName()		const	{ return m_strObjectName; }
		const std::vector<const glm::vec3>&	GetPositions()		const	{ return m_vPositions; }
		const std::vector<const glm::vec2>&	GetUVs()			const	{ return m_vUVs; }
		const std::vector<const glm::vec3>&	GetNormals()		const	{ return m_vNormals; }
		const std::vector<const OBJFace>&	GetFaces()			const	{ return m_vFaces; }
		const std::string&					GetMTLFileName()	const	{ return m_strMTLFileName; }
		const std::string&					GetMaterialName()	const	{ return m_strMaterialName; }
		bool								IsSmoothShading()	const	{ return m_bSmoothShading; }
		
		std::string							ToString()			const;

	private:
		bool							m_bLoaded;
		std::string						m_strOBJFileName;
		std::string						m_strOBJFileData;
		std::string						m_strObjectName;
		std::vector<const glm::vec3>	m_vPositions;
		std::vector<const glm::vec2>	m_vUVs;
		std::vector<const glm::vec3>	m_vNormals;
		std::vector<const OBJFace>		m_vFaces;
		std::string						m_strMTLFileName;
		std::string						m_strMaterialName;
		bool							m_bSmoothShading;
	};
}

#endif