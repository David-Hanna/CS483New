//------------------------------------------------------------------------
// LineDrawer
//
// Created:	2013/02/18
// Author:	Carel Boers
//	
// A simple line drawing utility. This isn't the most efficient class so 
// don't plan to use for anything other than debug drawing...
//------------------------------------------------------------------------

#include "BufferManager.h"
#include "Common.h"
#include "LineDrawer.h"
#include "ProgramManager.h"

using namespace HeatStroke;

//------------------------------------------------------------------------------
// Method:    LineDrawer
// Parameter: const std::string & p_strVertexProgramPath
// Parameter: const std::string & p_strFragmentProgramPath
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
LineDrawer::LineDrawer(const std::string &p_strVertexProgramPath, const std::string &p_strFragmentProgramPath)
	:
	m_pProgram(nullptr),
	m_pVB(nullptr),
	m_pDecl(nullptr),
	m_bDirty(false)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	m_pProgram = HeatStroke::ProgramManager::CreateProgram(p_strVertexProgramPath, p_strFragmentProgramPath);

	m_pVB = HeatStroke::BufferManager::CreateVertexBuffer(&m_lLineVertices[0], sizeof(LineVertex) * m_lLineVertices.size());

	m_pDecl = new HeatStroke::VertexDeclaration();
	m_pDecl->Begin();
	m_pDecl->AppendAttribute(HeatStroke::AT_Position, 3, HeatStroke::CT_Float);
	m_pDecl->AppendAttribute(HeatStroke::AT_Color, 4, HeatStroke::CT_Float);
	m_pDecl->SetVertexBuffer(m_pVB);
	m_pDecl->End();
}

//------------------------------------------------------------------------------
// Method:    ~LineDrawer
// Returns:   
// 
// Destructor.
//------------------------------------------------------------------------------
LineDrawer::~LineDrawer()
{
	HeatStroke::BufferManager::DestroyBuffer(m_pVB);
	m_pVB = nullptr;

	delete m_pDecl;
	m_pDecl = nullptr;
	
	HeatStroke::ProgramManager::DestroyProgram(m_pProgram);
	m_pProgram = nullptr;
}

//------------------------------------------------------------------------------
// Method:    Render
// Parameter: const SceneCamera* p_pCamera
// Returns:   void
// 
// Render the list of lines.
//------------------------------------------------------------------------------
void LineDrawer::Render(const SceneCamera* p_pCamera)
{
	if (!m_lLineVertices.empty())
	{
		if (m_bDirty)
		{
			m_bDirty = false;
			HeatStroke::BufferManager::DestroyBuffer(m_pVB);
			m_pVB = HeatStroke::BufferManager::CreateVertexBuffer(&m_lLineVertices[0], sizeof(LineVertex) * m_lLineVertices.size());

			m_pDecl->Begin();
			m_pDecl->SetVertexBuffer(m_pVB);
			m_pDecl->End();
		}

		// Use shader program.
		m_pProgram->Bind();

		// Bind Uniforms
		glm::mat4 mWorldViewTransform = p_pCamera->GetViewMatrix() * m_mWorldTransform;
		m_pProgram->SetUniform("WorldTransform", m_mWorldTransform);
		m_pProgram->SetUniform("WorldViewTransform", mWorldViewTransform);
		m_pProgram->SetUniform("WorldViewProjectionTransform", p_pCamera->GetProjectionMatrix() * mWorldViewTransform);
		m_pProgram->SetUniform("WorldInverseTransposeTransform", glm::transpose(glm::inverse(glm::mat3(m_mWorldTransform))));

		// Set up source data
		m_pDecl->Bind();

		// Draw!
		glDrawArrays(GL_LINES, 0, m_lLineVertices.size());
	}
}

//------------------------------------------------------------------------------
// Method:    AddLine
// Parameter: const glm::vec3 & p_vStart
// Parameter: const glm::vec3 & p_vTo
// Parameter: const HeatStroke::Color4 & p_cColor
// Returns:   void
// 
// Adds a line to be drawn.
//------------------------------------------------------------------------------
void LineDrawer::AddLine(const glm::vec3& p_vFrom, const glm::vec3& p_vTo, const HeatStroke::Color4& p_cColor)
{
	m_lLineVertices.push_back(LineVertex(p_vFrom.x, p_vFrom.y, p_vFrom.z, p_cColor.r, p_cColor.g, p_cColor.b, p_cColor.a));
	m_lLineVertices.push_back(LineVertex(p_vTo.x, p_vTo.y, p_vTo.z, p_cColor.r, p_cColor.g, p_cColor.b, p_cColor.a));
	m_bDirty = true;
}