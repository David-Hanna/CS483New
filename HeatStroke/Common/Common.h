//-----------------------------------------------------------------------------
// File:			Common.h
// Original Author:	Gordon Wood
//
// Common functions used in all samples
//-----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H

#include "Types.h"

#include <vector>
#include <string>
#include <fstream>
#include <FreeImage.h>
#include <boost/algorithm/string.hpp>

#ifdef _DEBUG
	#include <iostream>
	#include <cassert>

	void GL_CHECK_ERROR(const char* file, int line);
#else
	inline void GL_CHECK_ERROR(const char* file, int line);
#endif

#define DELETE_IF(p) if(p != nullptr) { delete p; p = nullptr; }


namespace HeatStroke
{
	namespace Common
	{
		// Loads in a whole file and returns the contents.
		std::string LoadWholeFile(const std::string& p_strFile);

		// Loads in a vertex and pixel shader and combines them into a program
		GLuint LoadShaders(const std::string& p_strVSFile, const std::string& p_strPSFile);

		// User of this function is responsible for calling FreeImage_Unload on the returned FIBITMAP.
		FIBITMAP* LoadTGA(const std::string& p_strFile);

		GLuint CreateTextureFromDDS(const std::string& p_strFile, unsigned int* p_pWidth = 0, unsigned int* p_pHeight = 0, bool* p_pHasMips = 0);

		// Splits a string into a vector of strings separated by the delimiter.
		std::vector<std::string> StringSplit(const std::string& p_strToSplit, const char* p_strDelimiter);
	}
}

#endif
