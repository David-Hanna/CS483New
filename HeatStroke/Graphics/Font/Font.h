//-----------------------------------------------------------------------------
// File:			Font.h
// Original Author:	Bradley Cooper
//
// Class describing a font tecture and mapping.
//
//-----------------------------------------------------------------------------
#ifndef W_FONT_H
#define W_FONT_H

#include "Types.h"
#include "Common.h"
#include "TextureManager.h"
#include <vector>
#include <string>
#include <map>
#include <sstream>

namespace HeatStroke
{
	class Font
	{
	public:
		//-------------------------------------------------------------------------
		// PUBLIC INTERFACE
		//-------------------------------------------------------------------------
		Font(const std::string& p_strFontFile);
		virtual ~Font();
		virtual GLuint GetCharWidth(const GLchar& p_cCharID);
		virtual GLuint GetCharHeight(const GLchar& p_cCharID);
		virtual Texture* GetTextureForChar(const GLchar& p_cCharID);
		virtual void GetMappingForChar(const GLchar& p_cCharID, GLfloat& p_fLeft, GLfloat& p_fRight, GLfloat& p_fTop, GLfloat& p_fBottom);

		virtual const std::string& GetFontName() const				{ return m_strName; };
		virtual GLuint GetFontSize() const							{ return m_uiSize; };
		virtual const std::string& GetFragmentProgramPath() const	{ return m_strFragmentProgramPath; };
		virtual const std::string& GetVertexProgramPath() const		{ return m_strVertexProgramPath; };
		//-------------------------------------------------------------------------

	private:
		//-------------------------------------------------------------------------
		// PRIVATE TYPES
		//-------------------------------------------------------------------------
		struct TextureMapping
		{
			Texture*	m_pTexture;
			GLuint		m_uiWidth, m_uiHeight;
			GLfloat		m_fLeft, m_fRight, m_fTop, m_fBottom;

			TextureMapping(Texture* p_pTexture, GLuint p_uiWidth, GLuint p_uiHeight, GLfloat p_fLeft, GLfloat p_fRight, GLfloat p_fTop, GLfloat p_fBottom) :
				m_pTexture(p_pTexture), m_uiWidth(p_uiWidth), m_uiHeight(p_uiHeight), m_fLeft(p_fLeft), m_fRight(p_fRight), m_fTop(p_fTop), m_fBottom(p_fBottom) {}
		};
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE METHODS
		//-------------------------------------------------------------------------
		virtual void ReadFont(const std::string& p_strFile);
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE MEMBERS
		//-------------------------------------------------------------------------
		std::string							m_strName;
		GLuint								m_uiSize;
		std::string							m_strFragmentProgramPath;
		std::string							m_strVertexProgramPath;
		std::map<GLchar, TextureMapping*>	m_mMapping;
		//-------------------------------------------------------------------------
	};

}

#endif

