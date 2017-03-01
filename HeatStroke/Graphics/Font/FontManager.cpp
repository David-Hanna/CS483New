//=================================================================================
// FontManager
// Author: David Hanna
//
// Manages loaded fonts. 
// Ensures fonts are not loaded more than once (unless flushed).
// Ensures all fonts are deleted when this class is destroyed.
//=================================================================================

#include "FontManager.h"

HeatStroke::FontManager* HeatStroke::FontManager::s_pFontManagerInstance = nullptr;

void HeatStroke::FontManager::CreateInstance()
{
	assert(s_pFontManagerInstance == nullptr);
	s_pFontManagerInstance = new FontManager();
}

void HeatStroke::FontManager::DestroyInstance()
{
	assert(s_pFontManagerInstance != nullptr);
	delete s_pFontManagerInstance;
	s_pFontManagerInstance = nullptr;
}

HeatStroke::FontManager* HeatStroke::FontManager::Instance()
{
	assert(s_pFontManagerInstance != nullptr);
	return s_pFontManagerInstance;
}

HeatStroke::FontManager::FontManager() :
m_mLoadedFonts()
{
}

HeatStroke::FontManager::~FontManager()
{
	Flush();
}

HeatStroke::Font* HeatStroke::FontManager::GetOrCreateFont(const std::string& p_strFontFileName)
{
	LoadedFonts::iterator it = m_mLoadedFonts.begin(), end = m_mLoadedFonts.end();
	for (; it != end; it++)
	{
		if (p_strFontFileName == it->first)
		{
			return it->second;
		}
	}

#ifdef _DEBUG
	printf("FontManager - Unloaded font: %s\n", p_strFontFileName.c_str());
#endif

	Font* pFont = new Font(p_strFontFileName);
	m_mLoadedFonts[p_strFontFileName] = pFont;
	return pFont;
}

void HeatStroke::FontManager::Flush()
{
	LoadedFonts::iterator it = m_mLoadedFonts.begin(), end = m_mLoadedFonts.end();
	for (; it != end; it++)
	{
		DELETE_IF(it->second);
	}

	m_mLoadedFonts.clear();
}