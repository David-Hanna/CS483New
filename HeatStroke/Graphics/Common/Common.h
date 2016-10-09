//-----------------------------------------------------------------------------
// File:			Common.h
// Original Author:	Gordon Wood
//
// Common functions used in all samples
//-----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H

#include "Types.h"
#include <memory>
#include <string>
#include <fstream>
#include <FreeImage.h>

#ifdef _DEBUG
void GL_CHECK_ERROR(const char* file, int line);
#else
inline void GL_CHECK_ERROR(const char* file, int line);
#endif


namespace HeatStroke
{
// Loads in a whole file and returns the contents.
std::shared_ptr<std::string> LoadWholeFile(const std::string& p_strFile);

// Loads in a vertex and pixel shader and combines them into a program
GLuint LoadShaders(const std::string& p_strVSFile, const std::string& p_strPSFile);

// User of this function is responsible for calling FreeImage_Unload on the returned FIBITMAP.
FIBITMAP* LoadTGA(const std::string& p_strFile);
GLuint CreateTextureFromDDS(const std::string& p_strFile, unsigned int* p_pWidth = 0, unsigned int* p_pHeight = 0, bool* p_pHasMips = 0);
}

#endif
