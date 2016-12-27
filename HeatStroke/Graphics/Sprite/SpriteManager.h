//============================================================================
// SpriteManager
// Author: David Hanna
//
// Manages loaded sprites. 
// Ensures sprites are only loaded from disc once (unless flushed).
// Ensures sprites are all deleted when this object is destroyed.
//============================================================================

#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "Common.h"
#include "Sprite.h"

#include <map>

namespace HeatStroke
{
	class SpriteManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static SpriteManager* Instance();

		Sprite* GetOrCreateSprite(const std::string& p_strMTLFileName, const std::string& p_strMaterialName);
		void Flush();

	private:
		static SpriteManager* s_pSpriteManagerInstance;

		typedef std::map<std::string, Sprite*> LoadedSprites;
		LoadedSprites m_mLoadedSprites;

		SpriteManager();
		~SpriteManager();

		std::string Key(const std::string& p_strMTLFileName, const std::string& p_strMaterialName)
			{ return p_strMTLFileName + "///" + p_strMaterialName; }
	};
}

#endif