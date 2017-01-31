//------------------------------------------------------------------------
// LineDrawer
//
// Created:	2013/02/18
// Author:	Carel Boers
//	
// A simple line drawing utility. This isn't the most efficient class so 
// don't plan to use for anything other than debug drawing...
//------------------------------------------------------------------------

#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include "Renderable.h"
#include "Program.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"
#include <vector>

namespace HeatStroke
{
class LineDrawer : Renderable
{
private:
	//-------------------------------------------------------------------------
	// Private types
	//-------------------------------------------------------------------------
	struct LineVertex
	{
		GLfloat x,y,z;
		GLfloat r, g, b, a;
		LineVertex(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
			: x(_x), y(_y), z(_z), r(_r), g(_g), b(_b), a(_a) { }

		LineVertex() : x(0), y(0), z(0), r(0), g(0), b(0), a(0) { }
	};
	typedef std::vector<LineVertex> LineVertexList;

	struct Vertex
	{
		GLfloat x, y, z;
		GLfloat r, g, b, a;
	};

public:
	//-------------------------------------------------------------------------
	// Public methods.
	//-------------------------------------------------------------------------
	LineDrawer(const std::string &p_strVertexProgramPath, const std::string &p_strFragmentProgramPath);
	~LineDrawer();

	void Render(const SceneCamera* p_pCamera);
	void AddLine(const glm::vec3& p_vStart, const glm::vec3& p_vTo, const HeatStroke::Color4& p_cColor = HeatStroke::Color4(1.0f, 1.0f, 1.0f, 1.0f));

	void		SetTransform(const glm::mat4& p_mWorldTransform)	{ m_mWorldTransform = p_mWorldTransform; }
	glm::mat4&	GetTransform()										{ return m_mWorldTransform; }

private:
	//-------------------------------------------------------------------------
	// Private members.
	//-------------------------------------------------------------------------

	// Dirty boolean for added lines
	mutable bool m_bDirty;

	// World transform
	glm::mat4 m_mWorldTransform;

	// Shader
	HeatStroke::Program* m_pProgram;

	// Vertex buffer and declaration
	HeatStroke::VertexBuffer* m_pVB;
	HeatStroke::VertexDeclaration* m_pDecl;

	// List of lines to draw
	LineVertexList m_lLineVertices;
};
}

#endif
