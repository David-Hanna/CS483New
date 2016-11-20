//-----------------------------------------------------------------------------
// File:			TextBox.cpp
// Original Author:	Bradley Cooper
//
// See header for notes.
//
//-----------------------------------------------------------------------------
#include "TextBox.h"

namespace HeatStroke
{
	//----------------------------------------------------------
	// Constructor
	//----------------------------------------------------------
	TextBox::TextBox(Font* p_pFont, const std::string& p_strMessage, const GLfloat& p_fWidth, const GLfloat& p_fHeight, const std::string& p_strVertexProgramPath, const std::string& p_strFragmentProgramPath) :
		m_pFont(p_pFont), m_strMessage(p_strMessage), m_fWidth(p_fWidth), m_fHeight(p_fHeight), m_strVertexProgramPath(p_strVertexProgramPath), m_strFragmentProgramPath(p_strFragmentProgramPath)
	{
		m_eHorizontal = H_ALIGN_LEFT;
		m_eVertical = V_ALIGN_TOP;
		m_vColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //white, opaque
		m_vPosition = glm::vec2(0.0f, 720.0f); //top-left of screen
		CreateQuads();
	}

	//----------------------------------------------------------
	// Destructor
	//----------------------------------------------------------
	TextBox::~TextBox()
	{
		DeleteQuads();
	}

	//----------------------------------------------------------
	// 'Destructor' for quad information
	//----------------------------------------------------------
	void TextBox::DeleteQuads()
	{
		//iterate through all quads and delete all information
		for (std::vector<Texture_Page_Quads>::iterator iter = m_mQuads.begin(); iter != m_mQuads.end(); ++iter)
		{
			BufferManager::DestroyBuffer(iter->m_pVB);
			BufferManager::DestroyBuffer(iter->m_pIB);
			MaterialManager::DestroyMaterial(iter->m_pMaterial);
			delete iter->m_pDecl;
		}
		std::vector<Texture_Page_Quads>().swap(m_mQuads); //empty map
	}

	//----------------------------------------------------------
	// Sets height and width of textbox and recreates quads if necessary
	//----------------------------------------------------------
	void TextBox::SetSize(const GLfloat& p_fWidth, const GLfloat& p_fHeight)
	{
		if (p_fWidth != m_fWidth || p_fHeight != m_fHeight)
		{
			m_fWidth = p_fWidth;
			m_fHeight = p_fHeight;
			CreateQuads();
		}
	}

	//----------------------------------------------------------
	// Sets alignment for text (top left by default)
	//----------------------------------------------------------
	void TextBox::SetAlignment(const HORIZONTAL_ALIGNMENT& p_eHorizontal, const VERTICAL_ALIGNMENT& p_eVertical)
	{
		if (p_eHorizontal != m_eHorizontal || p_eVertical != m_eVertical)
		{
			m_eHorizontal = p_eHorizontal;
			m_eVertical = p_eVertical;
			CreateQuads();
		}
	}

	//----------------------------------------------------------
	// Sets string for textbox
	//----------------------------------------------------------
	void TextBox::SetText(const std::string& p_strMessage)
	{
		//TODO: printf like functionality
		if (p_strMessage != m_strMessage)
		{
			m_strMessage = p_strMessage;
			CreateQuads();
		}
	}

	//----------------------------------------------------------
	// Sets position for textbox (origin at center), measured from top-left of screen
	//----------------------------------------------------------
	void TextBox::SetPosition(const GLfloat& p_fXPos, const GLfloat& p_fYPos)
	{
		m_vPosition = glm::vec2(p_fXPos, 720.0f-p_fYPos);
		glm::mat4 mWorld = glm::translate(glm::vec3(m_vPosition, 0.0f));

		//iterate through all renderable quads and set the matrix uniform
		for (std::vector<Texture_Page_Quads>::iterator iter = m_mQuads.begin(); iter != m_mQuads.end(); ++iter)
		{
			iter->m_mWorld = mWorld;
		}
	}

	//----------------------------------------------------------
	// Sets colour for string in textbox
	//----------------------------------------------------------
	void TextBox::SetColour(const glm::vec4& p_vColour)
	{
		m_vColour = p_vColour;

		//iterate through all renderable quads and set the color uniform
		for (std::vector<Texture_Page_Quads>::iterator iter = m_mQuads.begin(); iter != m_mQuads.end(); ++iter)
		{
			iter->m_pMaterial->SetUniform("UnifColor", m_vColour);
		}
	}

	//----------------------------------------------------------
	// 'Wrapper' for creating quads to display text, filling up m_Quads with data to render
	//----------------------------------------------------------
	void TextBox::CreateQuads()
	{
		static const GLfloat s_fTEXT_SCALING_SHRINK = 0.05f;	//how much to shrink the text scale per shrinkage
		static const GLfloat s_fTEXT_SCALING_MIN = 0.1f;		//the smallest you can shrink the text

		//obviously can't create the quads
		if (m_fHeight <= 0.0f || m_fWidth <= 0.0f)
		{
			assert(false && "Height or Width must be greater than 0 for textbox.");
			return;
		}
		
		//---------------------------
		// Separate text into lines and create source information for Quads
		//---------------------------

		std::vector<std::string> mLines;
		std::vector<glm::vec2> mLineSizes;

		//shrink text until the whole textbox is filled (or shrinks too small)
		GLfloat fTextScaling = 1.0f;
		while (fTextScaling >= s_fTEXT_SCALING_MIN && FitMessageIntoTextBox(mLines, mLineSizes, fTextScaling) == false)
			fTextScaling -= s_fTEXT_SCALING_SHRINK;

		//if it doesn't fit, can't create quads
		if (fTextScaling < s_fTEXT_SCALING_MIN)
		{
			assert(false && "Height or Width too small to create textbox.");
			return;
		}

		std::vector<Texture_Page_Source> sources;
		FillSourceInfo(sources, mLines, mLineSizes, fTextScaling);
		
		//---------------------------
		// Create renderable quads from vertices and indices
		//---------------------------
		DeleteQuads(); //erase contents of quads for new information
		glm::mat4 mWorld = glm::translate(glm::vec3(m_vPosition, 0.0f));
		static GLuint uiFontCount = 0; //give each font material a unique name
		for (std::vector<Texture_Page_Source>::iterator iter3 = sources.begin(); iter3 != sources.end(); ++iter3)
		{
			VertexBuffer* pVB = BufferManager::CreateVertexBuffer(&iter3->m_mVertices[0], sizeof(Texture_Vertex) * iter3->m_uiCount * 4);
			IndexBuffer* pIB = BufferManager::CreateIndexBuffer(&iter3->m_mIndices[0], sizeof(GLuint) * iter3->m_uiCount * 6);

			VertexDeclaration* pDecl = new VertexDeclaration();
			pDecl->Begin();
			pDecl->AppendAttribute(AT_Position, 3, CT_Float);
			pDecl->AppendAttribute(AT_TexCoord1, 2, CT_Float);
			pDecl->SetVertexBuffer(pVB);
			pDecl->SetIndexBuffer(pIB);
			pDecl->End();

			Material* pMat = MaterialManager::CreateMaterial(m_pFont->GetFontName() +
				"-" + std::to_string(m_pFont->GetFontSize()) + "-" + std::to_string(uiFontCount++));
			pMat->SetProgram(m_strVertexProgramPath, m_strFragmentProgramPath);
			pMat->SetTexture("texture", iter3->m_pTexture);
			pMat->SetUniform("UnifColor", m_vColour);

			m_mQuads.push_back(Texture_Page_Quads(pVB, pIB, pDecl, pMat, mWorld));
		}
	}

	//----------------------------------------------------------
	// Separate message into lines that fit in the text box
	// Returns false if the text cannot fit properly within the 
	// height/width of the textbox, true otherwise
	//----------------------------------------------------------
	bool TextBox::FitMessageIntoTextBox(std::vector<std::string>& p_mLines, std::vector<glm::vec2>& p_mLineSizes, const GLfloat& p_fTextScaling)
	{
		//empty map
		std::vector<std::string>().swap(p_mLines);
		std::vector<glm::vec2>().swap(p_mLineSizes);

		std::stringstream ssText(m_strMessage);
		std::string strWord, strLine;
		GLfloat fCurrentLineWidth = 0.0f, fCurrentLineHeight = 0.0f;

		GLfloat fSpaceSize = m_pFont->GetCharWidth(' ') * p_fTextScaling;

		while (!ssText.eof())
		{
			GLfloat fWordWidth = 0.0f, fLineHeight = 0.0f;
			std::getline(ssText, strWord, ' ');

			//get info on each char from the font file
			for (unsigned int i = 0; i < strWord.length(); ++i)
			{
				GLfloat fCharWidth = m_pFont->GetCharWidth(strWord[i]) * p_fTextScaling;
				GLfloat fCharHeight = m_pFont->GetCharHeight(strWord[i]) * p_fTextScaling;

				//add char width to line, and find tallest char
				fWordWidth += fCharWidth;
				fLineHeight = (fCharHeight > fLineHeight ? fCharHeight : fLineHeight);
			}

			if (fWordWidth > m_fWidth || fCurrentLineHeight > m_fHeight) //check if text will fit in the textbox
			{
				return false;
			}
			else if (strLine == "") //line is empty: don't add space before word
			{
				strLine = strWord;
				fCurrentLineWidth = fWordWidth;
			}
			else //line isn't empty: see if we can append text or leave it to next line
			{
				GLfloat fTempWidth = fCurrentLineWidth + fSpaceSize + fWordWidth; //line + " " + word
				if (fTempWidth > m_fWidth)
				{
					//line too big: push current line and set up next line
					p_mLines.push_back(strLine);
					p_mLineSizes.push_back(glm::vec2(fCurrentLineWidth, fLineHeight));

					strLine = strWord;
					fCurrentLineWidth = fWordWidth;

					//check if we have enough height to make another line (assuming it has the same height as this line)
					fCurrentLineHeight += fLineHeight;
					if (fCurrentLineHeight + fLineHeight > m_fHeight)
						return false;
				}
				else //there's still room left on this line
				{
					strLine.append(" " + strWord);
					fCurrentLineWidth = fTempWidth;
				}
			}

			if (ssText.eof())
			{
				//push back the last line
				fCurrentLineHeight += fLineHeight;
				p_mLines.push_back(strLine);
				p_mLineSizes.push_back(glm::vec2(fCurrentLineWidth, fLineHeight));
			}
		}

		return true;
	}

	//----------------------------------------------------------
	// Take lines and fill source array vertices and indices
	//----------------------------------------------------------
	void TextBox::FillSourceInfo(std::vector<Texture_Page_Source>& p_mSources, std::vector<std::string>& p_mLines, std::vector<glm::vec2>& p_mLineSizes, const GLfloat& p_fTextScaling)
	{
		std::string strLine;
		GLfloat fCurrentHeight = 0.0f, fMaxWidth = 0.0f, fMaxHeight = 0.0f;
		GLfloat fSpaceSize = m_pFont->GetCharWidth(' ') * p_fTextScaling;

		//get max width and height
		std::vector<glm::vec2>::iterator itSizeIter = p_mLineSizes.begin();
		for (; itSizeIter != p_mLineSizes.end(); ++itSizeIter)
		{
			fMaxWidth += itSizeIter->x * p_fTextScaling;
			fMaxHeight += itSizeIter->y * p_fTextScaling;
		}
		itSizeIter = p_mLineSizes.begin();

		//get y offset to match alignment set by the user
		GLfloat fHeightAlignOffset = 0.0f; //default for top alignment
		if (m_eVertical == V_ALIGN_MIDDLE)
			fHeightAlignOffset = -(p_fTextScaling / 2.0f) * (m_fHeight - fMaxHeight);
		else if (m_eVertical == V_ALIGN_BOTTOM)
			fHeightAlignOffset = -(p_fTextScaling) * (m_fHeight - fMaxHeight);

		//iterate over every line and then every character in the line to create the vertices and indices
		GLuint uiLineNum = 0;
		for (std::vector<std::string>::iterator itLineIter = p_mLines.begin(); itLineIter != p_mLines.end(); ++itLineIter)
		{
			strLine = *itLineIter;
			GLfloat fCurrentWidth = 0.0f;

			assert(itSizeIter != p_mLineSizes.end()); //should have same length
			glm::vec2 lineSize = *itSizeIter;

			//get x offset to match alignment set by the user
			GLfloat fWidthAlignOffset = 0.0f; //default for left alignment
			if (m_eHorizontal == H_ALIGN_CENTER)
				fWidthAlignOffset = (p_fTextScaling / 2.0f) * (m_fWidth - lineSize.x);
			else if (m_eHorizontal == H_ALIGN_RIGHT)
				fWidthAlignOffset = p_fTextScaling * (m_fWidth - lineSize.x);

			for (unsigned int i = 0; i < strLine.length(); ++i)
			{
				GLchar cCharID = strLine[i];

				//no need to render spaces
				if (cCharID == ' ')
				{
					fCurrentWidth += fSpaceSize;
					continue;
				}

				//get info for this char from the font class
				GLfloat fLeft, fRight, fTop, fBottom;
				m_pFont->GetMappingForChar(cCharID, fLeft, fRight, fTop, fBottom);
				Texture* pTexture = m_pFont->GetTextureForChar(cCharID);
				GLfloat fCharWidth = m_pFont->GetCharWidth(cCharID) * p_fTextScaling;
				GLfloat fCharHeight = m_pFont->GetCharHeight(cCharID) * p_fTextScaling;

				//font may have multiple texture pages, so add all indices and vertices per texture page together for less draw calls
				Texture_Page_Source texturePage;
				GLuint uiIndex = 0;
				std::vector<Texture_Page_Source>::iterator itSourceIter = p_mSources.begin();
				for (; itSourceIter != p_mSources.end(); ++itSourceIter)
				{
					if (pTexture == itSourceIter->m_pTexture) //found
					{
						texturePage = *itSourceIter;
						break;
					}
					++uiIndex;
				}

				if (itSourceIter == p_mSources.end()) //not found, create and push
				{
					texturePage.m_pTexture = pTexture;
					p_mSources.push_back(texturePage);
				}

				//calculate offsets once
				GLfloat fXOffset = fWidthAlignOffset + fCurrentWidth;
				GLfloat fYOffset = fHeightAlignOffset - fCurrentHeight;
				GLuint uiIndexOffset = texturePage.m_uiCount * 4;

				//create and push vertices in order: top-left, bottom-left, bottom-right, and top-right
				texturePage.m_mVertices.push_back(Texture_Vertex(fXOffset, fYOffset, 0.0f, fLeft, fTop));
				texturePage.m_mVertices.push_back(Texture_Vertex(fXOffset, fYOffset - fCharHeight, 0.0f, fLeft, fBottom));
				texturePage.m_mVertices.push_back(Texture_Vertex(fXOffset + fCharWidth, fYOffset - fCharHeight, 0.0f, fRight, fBottom));
				texturePage.m_mVertices.push_back(Texture_Vertex(fXOffset + fCharWidth, fYOffset, 0.0f, fRight, fTop));

				//create and push indices, offsetting by the number of vertices already pushed in
				texturePage.m_mIndices.push_back(uiIndexOffset);		//top left
				texturePage.m_mIndices.push_back(uiIndexOffset + 1);	//bot left
				texturePage.m_mIndices.push_back(uiIndexOffset + 2);	//bot right
				texturePage.m_mIndices.push_back(uiIndexOffset);		//top left
				texturePage.m_mIndices.push_back(uiIndexOffset + 2);	//bot right
				texturePage.m_mIndices.push_back(uiIndexOffset + 3);	//top right
				++texturePage.m_uiCount;

				p_mSources[uiIndex] = texturePage;

				//set up for next char
				fCurrentWidth += fCharWidth;
			}

			//set up for next line
			++uiLineNum;
			++itSizeIter;
			fCurrentHeight += lineSize.y;
		}
	}

	//----------------------------------------------------------
	// Renders all quads necessary for this textbox
	//----------------------------------------------------------
	void TextBox::Render(const SceneCamera* p_pCamera)
	{
		//iterate through all quads and render
		for (std::vector<Texture_Page_Quads>::iterator itQuadIter = m_mQuads.begin(); itQuadIter != m_mQuads.end(); ++itQuadIter)
		{
			itQuadIter->m_pDecl->Bind();
			itQuadIter->m_pMaterial->SetUniform("ProjViewWorld", p_pCamera->GetViewProjectionMatrix() * itQuadIter->m_mWorld);
			itQuadIter->m_pMaterial->Apply();

			glDrawElements(GL_TRIANGLES, itQuadIter->m_pIB->GetNumIndices(), GL_UNSIGNED_INT, 0);
		}
	}
}