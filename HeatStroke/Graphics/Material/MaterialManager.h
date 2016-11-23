//-----------------------------------------------------------------------------
// File:			MaterialManager.h
// Original Author:	Gordon Wood
//
// Class to manage materials. Handles creation of them and dealing with duplicate
// requests for the same material. 
//
// TODO: Left as an exercise is to load and populate materials from an XML
// definition of them (see accompanying slides).
//-----------------------------------------------------------------------------

#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include "Types.h"
#include "Material.h"
#include <string>
#include <map>

namespace HeatStroke
{
class MaterialManager
{
	public:
		//-------------------------------------------------------------------------
		// PUBLIC INTERFACE
		//-------------------------------------------------------------------------
		static Material* CreateMaterial(const std::string& p_strName);
		static void DestroyMaterial(Material* p_pMat);

		// TODO: You should really have a method like "Cleanup" that will delete
		// any leftover materials that weren't destroyed by the game, as
		// a safeguard - or at least prints a warning.
		//-------------------------------------------------------------------------

	private:
		//-------------------------------------------------------------------------
		// PRIVATE TYPES
		//-------------------------------------------------------------------------
		struct Entry
		{
			Material*	m_pMat;
			int			m_iRefCount;
			Entry(Material* p_pMat) : m_pMat(p_pMat), m_iRefCount(1) {}
		};
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE MEMBERS
		//-------------------------------------------------------------------------
		static std::map<std::string, Entry*>	m_materials;
		//-------------------------------------------------------------------------
};

}

#endif



