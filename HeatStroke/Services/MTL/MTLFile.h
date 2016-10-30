//================================================================================
// MTLFile.h
// Author: David Hanna
// 
// Loads an MTL file into a collection of data that may be used by other objects.
//================================================================================

#ifndef MTL_FILE_H
#define MTL_FILE_H

#include "Common.h"

#include <string>
#include <vector>
#include <glm.hpp>
#include <cassert>

namespace HeatStroke
{
	class MTLFile
	{
	public:
		class MTLMaterial
		{
			friend class MTLFile;

		public:
			const std::string&			GetMaterialName()						const	{ return m_strMaterialName; }

			const std::string&			GetVertexShaderName()					const	{ return m_strVertexShaderName; }
			const std::string&			GetFragmentShaderName()					const	{ return m_strFragmentShaderName; }

			const glm::vec3&			GetAmbientColor()						const	{ return m_vAmbientColor; }
			const glm::vec3&			GetDiffuseColor()						const	{ return m_vDiffuseColor; }
			const glm::vec3&			GetSpecularColor()						const	{ return m_vSpecularColor; }
			float						GetSpecularExponent()					const	{ return m_fSpecularExponent; }
			float						GetOpacity()							const	{ return m_fOpacity; }

			const std::string&			GetAmbientTextureFileName()				const	{ return m_strAmbientTextureFileName; }
			const std::string&			GetDiffuseTextureFileName()				const	{ return m_strDiffuseTextureFileName; }
			const std::string&			GetSpecularTextureFileName()			const	{ return m_strSpecularTextureFileName; }
			const std::string&			GetSpecularExponentTextureFileName()	const	{ return m_strSpecularExponentTextureFileName; }
			const std::string&			GetAlphaTextureFileName()				const	{ return m_strAlphaTextureFileName; }

			std::string					ToString()								const;

		private:
			std::string					m_strMaterialName;

			std::string					m_strVertexShaderName;
			std::string					m_strFragmentShaderName;

			glm::vec3					m_vAmbientColor;
			glm::vec3					m_vDiffuseColor;
			glm::vec3					m_vSpecularColor;
			float						m_fSpecularExponent;
			float						m_fOpacity;

			// textured lighting / alpha
			std::string					m_strAmbientTextureFileName;
			std::string					m_strDiffuseTextureFileName;
			std::string					m_strSpecularTextureFileName;
			std::string					m_strSpecularExponentTextureFileName;
			std::string					m_strAlphaTextureFileName;

			MTLMaterial() {}
		};

	public:
		MTLFile(const std::string& p_strMTLFileName);

		// Data hasn't been loaded until this method is called. Returns false if failed to load.
		bool ParseFile();

		const std::string&						GetMTLFileName()	const	{ return MTL_FILE_NAME; }
		bool									IsLoaded()			const	{ return m_bLoaded; }
		const std::vector<const MTLMaterial>&	GetMaterials()		const	{ return m_vMaterials; }
		const MTLMaterial*						GetMaterial(const std::string& p_strMaterialName) const;

		std::string								ToString()			const;

	private:
		const std::string				MTL_FILE_NAME;

		bool							m_bLoaded;
		std::string						m_strMTLFileData;
		std::vector<const MTLMaterial>	m_vMaterials;
	};
}

#endif