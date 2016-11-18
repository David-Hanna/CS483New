//-----------------------------------------------------------------------------
// File:			Font.cpp
// Original Author:	Bradley Cooper
//
// See header for notes.
//
//-----------------------------------------------------------------------------
#include "Font.h"

namespace HeatStroke
{
	//----------------------------------------------------------
	// Constructor
	//----------------------------------------------------------
	Font::Font(const std::string& p_strFontFile, const std::string& p_strTextureFilesPath) :
		m_strName(""),
		m_uiSize(0),
		m_mMapping()
	{
		ReadFont(p_strFontFile, p_strTextureFilesPath);
	}

	//----------------------------------------------------------
	// Destructor
	//----------------------------------------------------------
	Font::~Font()
	{
		for (std::map<GLchar, TextureMapping*>::iterator iter = m_mMapping.begin(); iter != m_mMapping.end(); iter++)
		{
			TextureManager::DestroyTexture(iter->second->m_pTexture);
		}
	}

	//----------------------------------------------------------
	// Reads font mapping from .fnt file
	//----------------------------------------------------------
	void Font::ReadFont(const std::string& p_strFile, const std::string& p_strTextureFilesPath)
	{
		std::string strLine, strRead;
		GLfloat fTextureWidth = 1.0f, fTextureHeight = 1.0f; //default values to avoid divide by zero
		std::map<GLuint, std::string> mTextures;

		//get contents of .fnt file to read through
		std::string strFntFile = Common::LoadWholeFile(p_strFile);
		assert(strFntFile.length() != 0 && "Error reading font.");
		std::stringstream mFileStream(strFntFile.c_str());

		//keep looping through contents of file until end of file
		while (!mFileStream.eof())
		{
			//read one line (until \n) and then read the first word (until " ")
			std::getline(mFileStream, strLine, '\n');
			std::stringstream lineStream(strLine);
			std::getline(lineStream, strRead, ' ');

			//select action to perform based on the first word
			if (strRead == "char")
			{
				//char info: get information for individual chars
				GLchar cCharID;
				GLuint uiX, uiY, uiWidth, uiHeight, uiPageID;

				for (int i = 0; i < 9; ++i) //there are 10 fields, but we're not interested in the last one (chnl)
				{
					std::getline(lineStream, strRead, '='); //multiple (inconsistent) spacing between information
					std::getline(lineStream, strRead, ' ');

					switch (i)
					{
					case 0: cCharID = atoi(strRead.c_str());	//char id
						break;
					case 1: uiX = atoi(strRead.c_str());		//x
						break;
					case 2: uiY = atoi(strRead.c_str());		//y
						break;
					case 3: uiWidth = atoi(strRead.c_str());	//width
						break;
					case 4: uiHeight = atoi(strRead.c_str());	//height
						break;
					case 5: case 6: case 7: break;				//xoffset, yoffset, xadvance. Don't need

					case 8: uiPageID = atoi(strRead.c_str());	//page id

						//now that we have our information for this char, add to mapping
						m_mMapping[cCharID] = new TextureMapping(
							TextureManager::CreateTexture(mTextures[uiPageID]),
							uiWidth, uiHeight,
							uiX / fTextureWidth,									//left
							(uiX + uiWidth) / fTextureWidth,						//right
							(fTextureHeight - uiY) / fTextureHeight,				//top
							(fTextureHeight - (uiY + uiHeight)) / fTextureHeight	//bottom
							//*Note: y coordinates for font file is measured from top->bottom pixels, must reverse for uv texturing
							);
						break;
					}
				}
			}
			else if (strRead == "page")
			{
				//page info: get information on texture pages
				//get page id
				std::getline(lineStream, strRead, ' ');
				GLuint uiPageID = atoi(strRead.substr(strRead.find('=') + 1).c_str());

				//get texture file corresponding to page id
				std::getline(lineStream, strRead, ' ');
				GLuint uiPos = strRead.find('\"') + 1;
				std::string strTextureFile = strRead.substr(uiPos, strRead.length() - uiPos - 1);

				mTextures[uiPageID] = p_strTextureFilesPath + strTextureFile;
			}
			else if (strRead == "info")
			{
				//font info: get font name and size
				//get name
				std::getline(lineStream, strRead, ' ');
				GLuint uiPos = strRead.find('\"') + 1;
				m_strName = strRead.substr(uiPos, strRead.length() - uiPos - 1);

				//get size
				std::getline(lineStream, strRead, ' ');
				m_uiSize = atoi(strRead.substr(strRead.find('=') + 1).c_str());
			}
			else if (strRead == "common")
			{
				//common info: get information for all texture pages
				std::getline(lineStream, strRead, ' '); //lineheight, don't need
				std::getline(lineStream, strRead, ' '); //base, don't need

				//get width of all texture pages
				std::getline(lineStream, strRead, ' ');
				fTextureWidth = (float)atoi(strRead.substr(strRead.find('=') + 1).c_str());

				//get height of all texture pages
				std::getline(lineStream, strRead, ' ');
				fTextureHeight = (float)atoi(strRead.substr(strRead.find('=') + 1).c_str());
			}
		}
	}

	//----------------------------------------------------------
	// Get width of char. If not found, returns 0
	//----------------------------------------------------------
	GLuint Font::GetCharWidth(const GLchar& p_cCharID)
	{
		std::map<GLchar, TextureMapping*>::iterator iter = m_mMapping.find(p_cCharID);
		return (iter == m_mMapping.end() ? 0 : iter->second->m_uiWidth);
	}

	//----------------------------------------------------------
	// Get height of char. If not found, returns 0
	//----------------------------------------------------------
	GLuint Font::GetCharHeight(const GLchar& p_cCharID)
	{
		std::map<GLchar, TextureMapping*>::iterator iter = m_mMapping.find(p_cCharID);
		return (iter == m_mMapping.end() ? 0 : iter->second->m_uiHeight);
	}

	//----------------------------------------------------------
	// Return texture for char. If not found, returns nullptr.
	//----------------------------------------------------------
	Texture* Font::GetTextureForChar(const GLchar& p_cCharID)
	{
		std::map<GLchar, TextureMapping*>::iterator iter = m_mMapping.find(p_cCharID);
		return (iter == m_mMapping.end() ? nullptr : iter->second->m_pTexture);
	}

	//----------------------------------------------------------
	// Return texture mapping for char. If not found, returns all 0's
	//----------------------------------------------------------
	void Font::GetMappingForChar(const GLchar& p_cCharID, GLfloat& p_fLeft, GLfloat& p_fRight, GLfloat& p_fTop, GLfloat& p_fBottom)
	{
		std::map<GLchar, TextureMapping*>::iterator iter = m_mMapping.find(p_cCharID);
		if (iter != m_mMapping.end())
		{
			p_fLeft = iter->second->m_fLeft;
			p_fRight = iter->second->m_fRight;
			p_fTop = iter->second->m_fTop;
			p_fBottom = iter->second->m_fBottom;
		}
		else
		{
			p_fLeft = 0;
			p_fRight = 0;
			p_fTop = 0;
			p_fBottom = 0;
		}
	}
}

