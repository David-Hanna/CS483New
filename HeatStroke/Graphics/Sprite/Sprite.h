//-----------------------------------------------------------------------------
// Sprite
// Author: David Hanna
//
// 2D textured quad.
//-----------------------------------------------------------------------------

#ifndef SPRITE_H
#define SPRITE_H

#include "Mesh.h"
#include "MTLFile.h"

#include <string>

namespace HeatStroke
{
	class Sprite
	{
	public:
		Sprite(const std::string& p_strMTLFileName, const std::string& p_strMaterialName);
		~Sprite();

		Mesh& GetMesh()										   { return m_mMesh; }

	private:
		static float		  m_vQuadVertex[];
		static unsigned short m_vQuadIndex[];

		Mesh		m_mMesh;
	};
}

#endif