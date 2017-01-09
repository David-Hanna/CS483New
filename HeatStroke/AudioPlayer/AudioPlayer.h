//===========================================================================
// AudioPlayer
// Author: David Hanna
//
// Plays one background music at a time.
// Load and keep sound effects to play at will until flushed or object deletion.
//===========================================================================

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include "Common.h"
#include "EasyXML.h"

#include <map>
#include <SFML/Audio.hpp>

namespace HeatStroke
{
	class AudioPlayer
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static AudioPlayer* Instance();

		bool OpenMusicFromFile(const std::string& p_strFile);
		void PlayMusic();
		void PauseMusic();
		void StopMusic();

		void PreloadSoundEffects(const std::string& p_strPreloadFileName);
		void PlaySoundEffect(const std::string& p_strFile);
		void FlushSoundEffects();

	private:
		static AudioPlayer* s_pAudioPlayerInstance;

		AudioPlayer() : m_pCurrentMusic(nullptr), m_mLoadedSoundEffects() {}
		~AudioPlayer() { DELETE_IF(m_pCurrentMusic); FlushSoundEffects(); }

	private:
		typedef std::pair<sf::SoundBuffer*, sf::Sound*> SoundEffect;
		typedef std::map<std::string, SoundEffect> LoadedSoundEffects;

		sf::Music* m_pCurrentMusic;
		LoadedSoundEffects m_mLoadedSoundEffects;
	};
}

#endif