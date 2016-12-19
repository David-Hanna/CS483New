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
		std::vector<Mesh> m_vMeshes;
	};
}

#endif



