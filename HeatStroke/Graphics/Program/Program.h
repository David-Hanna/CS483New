//-----------------------------------------------------------------------------
// File:			Program.h
// Original Author:	Gordon Wood
//
// Class encapsulating a vertex shader and pixel shader combo, and allowing
// easy setting of uniforms on the program
//-----------------------------------------------------------------------------

#ifndef PROGRAM_H
#define PROGRAM_H

#include "Types.h"
#include <string>

namespace HeatStroke
{
	class Program
	{
		friend class ProgramManager;

	public:
		void Bind();

        void SetUniform(const char* p_strName, const glm::mat4& p_m);
        void SetUniform(const char* p_strName, const glm::mat3& p_m);
        void SetUniform(const char* p_strName, const glm::vec3& p_v);
        void SetUniform(const char* p_strName, const glm::vec4& p_v);
		void SetUniform(const char* p_strName, const HeatStroke::Color4& p_c);
		void SetUniform(const char* p_strName, float p_f);
		void SetUniform(const char* p_strName, int p_i);
		void SetUniform(const char* p_strName, const glm::mat4* p_m, int p_uiNumMatrices);
		void SetUniform(const char* p_strName, const glm::mat3* p_m, int p_uiNumMatrices);

	private:
		GLuint		m_uiProgram;


		// Made private to enforce usage via ProgramManager
		Program(const std::string& p_strVS, const std::string& p_strPS);
		virtual ~Program();
		
		bool CompileShader(GLuint* p_pShader, GLenum p_eType, const std::string& p_strFile);
		bool LinkProgram();
	};
}

#endif
