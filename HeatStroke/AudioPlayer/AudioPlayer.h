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

#include <vector>
#include <SFML/Audio.hpp>

namespace HeatStroke
{
	class AudioPlayer
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static AudioPlayer* Instance();

		void Update();

		bool OpenMusicFromFile(const std::string& p_strFile);
		void PlayMusic();
		void PauseMusic();
		void StopMusic();

		void PlaySoundEffectFromFile(const std::string& p_strFile);

	private:
		static AudioPlayer* s_pAudioPlayerInstance;

		AudioPlayer() : m_pCurrentMusic(nullptr), m_vSoundEffects() {}
		~AudioPlayer() { DELETE_IF(m_pCurrentMusic); }

	private:
		sf::Music* m_pCurrentMusic;
		std::vector<std::pair<sf::SoundBuffer*, sf::Sound*>> m_vSoundEffects;
	};
}

#endif