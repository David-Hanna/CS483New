//-----------------------------------------------------------------------------
// File:			Sprite.cpp
// Original Author:	Carel Boers
//
// Class representing a 2D textured quad.
//
//-----------------------------------------------------------------------------

#include "Sprite.h"
#include "Common.h"
#include "BufferManager.h"
#include "MaterialManager.h"

using namespace HeatStroke;

struct Vertex
{
	GLfloat x,y;
	GLfloat u,v;
};

//----------------------------------------------------------
// Constructor
//----------------------------------------------------------
Sprite::Sprite(const std::string& p_strTexture, const std::string& p_strVertexProgram, const std::string& p_strFragmentProgram, const glm::vec2& p_vDimensions) 
	:
	m_pVB(nullptr),
	m_pDecl(nullptr),
	m_pProgram(nullptr)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GL_CHECK_ERROR(__FILE__, __LINE__);

	const float halfX = p_vDimensions.x / 2.0f;
	const float halfY = p_vDimensions.y / 2.0f;

	const Vertex squareVertices[] = {
		{ -halfX, -halfY,	0, 0 }, 
		{ -halfX, halfY,	0, 1 }, 
		{ halfX, halfY,		1, 1 }, 

		{ halfX, halfY,		1, 1 }, 
		{ halfX, -halfY,	1, 0 }, 
		{ -halfX, -halfY,	0, 0 }, 
	};

	m_pProgram = HeatStroke::ProgramManager::CreateProgram(p_strVertexProgram, p_strFragmentProgram);
	m_pVB = HeatStroke::BufferManager::CreateVertexBuffer(squareVertices, sizeof(Vertex) * 6);

	m_pDecl = new HeatStroke::VertexDeclaration();
	m_pDecl->Begin();
	m_pDecl->AppendAttribute(HeatStroke::AT_Position, 2, HeatStroke::CT_Float);
	m_pDecl->AppendAttribute(HeatStroke::AT_TexCoord1, 2, HeatStroke::CT_Float);
	m_pDecl->SetVertexBuffer(m_pVB);
	m_pDecl->End();

	glGenTextures(1,&m_uiTex);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	glBindTexture(GL_TEXTURE_2D, m_uiTex);
	GL_CHECK_ERROR(__FILE__, __LINE__);

	FIBITMAP* image = LoadTGA(p_strTexture);
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		FreeImage_GetWidth(image), 
		FreeImage_GetHeight(image), 
		0, 
		GL_BGRA, 
		GL_UNSIGNED_BYTE, 
		FreeImage_GetBits(image)
	);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	FreeImage_Unload(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GL_CHECK_ERROR(__FILE__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}

//----------------------------------------------------------
// Destructor
//----------------------------------------------------------
Sprite::~Sprite()
{
	HeatStroke::BufferManager::DestroyBuffer(m_pVB);
	delete m_pDecl;
	HeatStroke::ProgramManager::DestroyProgram(m_pProgram);
}

//----------------------------------------------------------
// Updates this sprite
//----------------------------------------------------------
void Sprite::Update(float p_fDelta)
{

}

//----------------------------------------------------------
// Renders this sprite
//----------------------------------------------------------
void Sprite::Render(const Camera* p_pCamera)
{
	// Can't render without a camera.
	assert(p_pCamera != nullptr);

	// Use shader program.
	m_pProgram->Bind();

	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, (int)m_uiTex);
	GL_CHECK_ERROR(__FILE__, __LINE__);

	// Bind Uniforms
	m_pProgram->SetUniform("World", m_mWorldTransform);
	m_pProgram->SetUniform("Sampler", 0);

	// Set up source data
	m_pDecl->Bind();

	// Draw!
	glDrawArrays(GL_TRIANGLES, 0, 6);
	GL_CHECK_ERROR(__FILE__, __LINE__);
}