//============================================================================
// SpriteManager
// Author: David Hanna
//
// Manages loaded sprites. 
// Ensures sprites are only loaded from disc once (unless flushed).
// Ensures sprites are all deleted when this object is destroyed.
//============================================================================

#include "SpriteManager.h"

HeatStroke::SpriteManager* HeatStroke::SpriteManager::s_pSpriteManagerInstance = nullptr;

void HeatStroke::SpriteManager::CreateInstance()
{
	assert(s_pSpriteManagerInstance == nullptr);
	s_pSpriteManagerInstance = new SpriteManager();
}

void HeatStroke::SpriteManager::DestroyInstance()
{
	assert(s_pSpriteManagerInstance != nullptr);
	delete s_pSpriteManagerInstance;
	s_pSpriteManagerInstance = nullptr;
}

HeatStroke::SpriteManager* HeatStroke::SpriteManager::Instance()
{
	assert(s_pSpriteManagerInstance != nullptr);
	return s_pSpriteManagerInstance;
}

HeatStroke::SpriteManager::SpriteManager() :
m_mLoadedSprites()
{
}

HeatStroke::SpriteManager::~SpriteManager()
{
	Flush();
}

HeatStroke::Sprite* HeatStroke::SpriteManager::GetOrCreateSprite(const std::string& p_strMTLFileName, const std::string& p_strMaterialName)
{
	std::string key = Key(p_strMTLFileName, p_strMaterialName);

	LoadedSprites::iterator it = m_mLoadedSprites.begin(), end = m_mLoadedSprites.end();
	for (; it != end; it++)
	{
		if (key == it->first)
		{
			return it->second;
		}
	}

	Sprite* pSprite = new Sprite(p_strMTLFileName, p_strMaterialName);
	m_mLoadedSprites[key] = pSprite;
	return pSprite;
}

void HeatStroke::SpriteManager::Flush()
{
	LoadedSprites::iterator it = m_mLoadedSprites.begin(), end = m_mLoadedSprites.end();
	for (; it != end; it++)
	{
		DELETE_IF(it->second);
	}

	m_mLoadedSprites.clear();
}