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

		void SetMusicVolume(float p_fMusicVolume)				{ m_fMusicVolume = p_fMusicVolume; }
		void SetSoundEffectsVolume(float p_fSoundEffectsVolume) { m_fSoundEffectsVolume = p_fSoundEffectsVolume; }

		float GetMusicVolume() const			{ return m_fMusicVolume; }
		float GetSoundEffectsVolume() const		{ return m_fSoundEffectsVolume; }

	private:
		static AudioPlayer* s_pAudioPlayerInstance;

		AudioPlayer() :
			m_pCurrentMusic(nullptr), 
			m_vSoundEffects(),
			m_fMusicVolume(100.0f), 
			m_fSoundEffectsVolume(100.0f) {}
		~AudioPlayer() { DELETE_IF(m_pCurrentMusic); }

	private:
		sf::Music* m_pCurrentMusic;
		std::vector<std::pair<sf::SoundBuffer*, sf::Sound*>> m_vSoundEffects;

		float m_fMusicVolume;
		float m_fSoundEffectsVolume;
	};
}

#endif