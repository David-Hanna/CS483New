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

#include "Program.h"
#include "VertexBuffer.h"
#include "VertexDeclaration.h"
#include <vector>

namespace HeatStroke
{
class LineDrawer
{
private:
	//-------------------------------------------------------------------------
	// Private types
	//-------------------------------------------------------------------------
	struct LineData
	{
		GLfloat x,y,z;
		GLfloat r, g, b, a;
		LineData(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
			: x(_x), y(_y), z(_z), r(_r), g(_g), b(_b), a(_a) { }
	};
	typedef std::vector<LineData*> LineVector;

	struct Vertex
	{
		GLfloat x,y,z;
		GLfloat r, g, b, a;
	};

public:
	//-------------------------------------------------------------------------
	// Public methods.
	//-------------------------------------------------------------------------
	LineDrawer();
	~LineDrawer();

	void Init(const std::string &p_strVertexProgramPath, const std::string &p_strFragmentProgramPath);
	void Render(const glm::mat4& p_mProjectionMatrix, const glm::mat4& p_mViewMatrix);
	void AddLine(const glm::vec3& p_vStart, const glm::vec3& p_vTo, const HeatStroke::Color4& p_cColor = HeatStroke::Color4(1.0f, 1.0f, 1.0f, 1.0f));

private:
	//-------------------------------------------------------------------------
	// Private members.
	//-------------------------------------------------------------------------

	// Shader
	HeatStroke::Program* m_pProgram;

	// Vertex buffer and declaration
	HeatStroke::VertexBuffer* m_pVB;
	HeatStroke::VertexDeclaration* m_pDecl;

	// List of lines to draw; cleared each frame after being rendered
	LineVector m_lVertexVector;
};
}

#endif
