//=================================================================================
// FontManager
// Author: David Hanna
//
// Manages loaded fonts. 
// Ensures fonts are not loaded more than once (unless flushed).
// Ensures all fonts are deleted when this class is destroyed.
//=================================================================================

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "Common.h"
#include "Font.h"

#include <map>

namespace HeatStroke
{
	class FontManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static FontManager* Instance();

		Font* GetOrCreateFont(const std::string& p_strFontFileName);
		void Flush();

	private:
		static FontManager* s_pFontManagerInstance;

		typedef std::map<std::string, Font*> LoadedFonts;
		LoadedFonts m_mLoadedFonts;

		FontManager();
		~FontManager();
	};
}

#endif