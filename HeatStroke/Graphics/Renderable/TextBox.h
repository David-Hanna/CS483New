//-----------------------------------------------------------------------------
// File:			TextBox.h
// Original Author:	Bradley Cooper
//
// Class describing a textbox to display text in quads.
//
//-----------------------------------------------------------------------------
#ifndef W_TEXTBOX_H
#define W_TEXTBOX_H

#include "Types.h"
#include "Renderable.h"
#include "Font.h"
#include "TextureManager.h"
#include "BufferManager.h"
#include "VertexDeclaration.h"
#include "MaterialManager.h"
#include <glm.hpp>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>

namespace HeatStroke
{
	class TextBox : public Renderable
	{
	public:
		//-------------------------------------------------------------------------
		// PUBLIC ENUMS
		//-------------------------------------------------------------------------
		enum HORIZONTAL_ALIGNMENT {
			H_ALIGN_LEFT = 0,
			H_ALIGN_CENTER,
			H_ALIGN_RIGHT
		};
		enum VERTICAL_ALIGNMENT {
			V_ALIGN_TOP = 0,
			V_ALIGN_MIDDLE,
			V_ALIGN_BOTTOM
		};
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PUBLIC INTERFACE
		//-------------------------------------------------------------------------
		TextBox(Font* p_pFont, const std::string& p_strMessage, const GLfloat& p_fWidth = 200.0f, const GLfloat& p_fHeight = 200.0f);
		virtual ~TextBox();
		virtual void SetTransform(const glm::mat4& p_mWorldTransform);
		virtual void SetSize(const GLfloat& p_fWidth, const GLfloat& p_fHeight);
		virtual void SetText(const std::string& p_strMessage);
		virtual void SetPosition(const GLfloat& p_fXPos, const GLfloat& p_fYPos);
		virtual void SetColour(const glm::vec4& p_vColour);
		virtual void SetAlignment(const HORIZONTAL_ALIGNMENT& p_eHorizontal, const VERTICAL_ALIGNMENT& p_eVertical);
		virtual void Render(const SceneCamera* p_pCamera);

		virtual std::string GetText() const { return m_strMessage; };
		virtual GLfloat GetHeight() const { return m_fHeight; };
		virtual GLfloat GetWidth() const { return m_fWidth; };
		virtual const glm::vec2& GetPosition() const { return m_vPosition; }
		//-------------------------------------------------------------------------

	private:
		//-------------------------------------------------------------------------
		// PRIVATE TYPES
		//-------------------------------------------------------------------------
		struct Texture_Vertex
		{
			GLfloat x, y, z;
			GLfloat u, v;

			Texture_Vertex(const GLfloat& p_fX, const GLfloat& p_fY, const GLfloat& p_fZ, const GLfloat& p_fU, const GLfloat& p_fV) :
				x(p_fX), y(p_fY), z(p_fZ), u(p_fU), v(p_fV) {}
		};

		struct Texture_Page_Quads
		{
			VertexBuffer*		m_pVB;
			IndexBuffer*		m_pIB;
			VertexDeclaration*	m_pDecl;
			Material*			m_pMaterial;
			glm::mat4			m_mWorld;

			Texture_Page_Quads(VertexBuffer* p_pVB, IndexBuffer* p_pIB, VertexDeclaration* p_pDecl, Material* p_pMaterial, glm::mat4 p_mWorld) :
				m_pVB(p_pVB), m_pIB(p_pIB), m_pDecl(p_pDecl), m_pMaterial(p_pMaterial), m_mWorld(p_mWorld) {}
		};

		struct Texture_Page_Source
		{
			Texture*					m_pTexture;
			GLuint						m_uiCount;
			std::vector<Texture_Vertex>	m_mVertices;
			std::vector<GLuint>			m_mIndices;

			Texture_Page_Source() : m_uiCount(0) {};
		};
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE METHODS
		//-------------------------------------------------------------------------
		virtual void CreateQuads();
		virtual bool FitMessageIntoTextBox(std::vector<std::string>& p_mLines, std::vector<glm::vec2>& p_mLineSizes, const GLfloat& p_fTextScaling);
		virtual void FillSourceInfo(std::vector<Texture_Page_Source>& p_mSources, std::vector<std::string>& p_mLines, std::vector<glm::vec2>& p_mLineSizes, const GLfloat& p_fTextScaling);
		virtual void DeleteQuads();
		//-------------------------------------------------------------------------

		//-------------------------------------------------------------------------
		// PRIVATE MEMBERS
		//-------------------------------------------------------------------------
		Font*								m_pFont;
		std::string							m_strMessage;
		GLfloat								m_fWidth;
		GLfloat								m_fHeight;
		glm::vec2							m_vPosition;
		glm::vec4							m_vColour;
		HORIZONTAL_ALIGNMENT				m_eHorizontal;
		VERTICAL_ALIGNMENT					m_eVertical;
		std::vector<Texture_Page_Quads>		m_mQuads;
		//-------------------------------------------------------------------------
	};

}

#endif

