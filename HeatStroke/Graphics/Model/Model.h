//-----------------------------------------------------------------------------
// Model
// Author: David Hanna
//
// A model is a collection of meshes loaded from OBJ and MTL.
//-----------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "OBJFile.h"
#include "MTLFile.h"

#include <string>
#include <vector>
#include <map>

#ifdef _DEBUG
#include "Common.h"
#endif

namespace HeatStroke
{
	class Model
	{
	public:
		Model(const std::string& p_strOBJFileName);
		~Model();

		std::vector<Mesh>& GetMeshes() { return m_vMeshes; }

	private:
		typedef std::map<OBJFile::OBJVertex, unsigned short, OBJFile::OBJVertexComparator> VertexToIndexMap;

		std::vector<Mesh> m_vMeshes;

		// the signature of this method is gross, but it's better than having all of this code inlined
		void AddToVertexData(
			std::vector<float>& p_vVertexData,
			std::vector<unsigned short>& p_vIndexData,
			const OBJFile::OBJVertex& p_Vertex,
			const std::vector<const glm::vec3>& p_vPositions,
			const std::vector<const glm::vec3>& p_vNormals,
			const std::vector<const glm::vec2>& p_vUVs,
			VertexToIndexMap& p_mExistingVerticesMap);
	};
}

#endif



