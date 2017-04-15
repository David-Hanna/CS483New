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

void HeatStroke::SpriteManager::Preload(const std::string& p_strPreloadFileName)
{
	tinyxml2::XMLDocument mPreloadDoc;
	assert(mPreloadDoc.LoadFile(p_strPreloadFileName.c_str()) == tinyxml2::XML_NO_ERROR);

	tinyxml2::XMLElement* pRootElement = mPreloadDoc.RootElement();
	assert(pRootElement != nullptr);

	tinyxml2::XMLElement* pSpritesElement = pRootElement->FirstChildElement("Sprites");
	assert(pSpritesElement != nullptr);

	for (tinyxml2::XMLElement* pSpriteElement = pSpritesElement->FirstChildElement("Sprite");
		pSpriteElement != nullptr;
		pSpriteElement = pSpriteElement->NextSiblingElement("Sprite"))
	{
		std::string strMTLFileName, strMaterialName;
		EasyXML::GetRequiredStringAttribute(pSpriteElement, "path", strMTLFileName);
		EasyXML::GetRequiredStringAttribute(pSpriteElement, "mtl", strMaterialName);
		std::string strKey = Key(strMTLFileName, strMaterialName);

		LoadedSprites::const_iterator it = m_mLoadedSprites.find(strKey);
		if (it == m_mLoadedSprites.end())
		{
			Sprite* pSprite = new Sprite(strMTLFileName, strMaterialName);
			m_mLoadedSprites[strKey] = pSprite;
		}
		else
		{
			printf("SpriteManager: Sprite %s already loaded\n", strKey.c_str());
		}
	}
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

#ifdef _DEBUG
	printf("SpriteManager - Unloaded sprite: %s\n", key.c_str());
#endif

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