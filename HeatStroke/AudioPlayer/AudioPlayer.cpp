//===========================================================================
// AudioPlayer
// Author: David Hanna
//
// Plays one background music at a time.
// Load and keep sound effects to play at will until flushed or object deletion.
//===========================================================================

#include "AudioPlayer.h"

HeatStroke::AudioPlayer* HeatStroke::AudioPlayer::s_pAudioPlayerInstance = nullptr;

void HeatStroke::AudioPlayer::CreateInstance()
{
	assert(s_pAudioPlayerInstance == nullptr);
	s_pAudioPlayerInstance = new AudioPlayer;
}

void HeatStroke::AudioPlayer::DestroyInstance()
{
	assert(s_pAudioPlayerInstance != nullptr);
	delete s_pAudioPlayerInstance;
	s_pAudioPlayerInstance = nullptr;
}

HeatStroke::AudioPlayer* HeatStroke::AudioPlayer::Instance()
{
	assert(s_pAudioPlayerInstance != nullptr);
	return s_pAudioPlayerInstance;
}

void HeatStroke::AudioPlayer::Update()
{
	// Sound will be handled better after next sprint. Need to keep them preloaded and just
	// delete them all when this object gets deleted.
	/*
	std::vector<std::pair<sf::SoundBuffer*, sf::Sound*>>::iterator it = m_vSoundEffects.begin(), end = m_vSoundEffects.end();
	for (; it != end; it++)
	{
		if ((*it).second->getStatus() == sf::Sound::Status::Stopped)
		{
			delete (*it).first;
			delete (*it).second;
			it = m_vSoundEffects.erase(it);
		}
	}
	*/
}

bool HeatStroke::AudioPlayer::OpenMusicFromFile(const std::string& p_strFile)
{
	if (m_pCurrentMusic != nullptr)
	{
		delete m_pCurrentMusic;
	}

	m_pCurrentMusic = new sf::Music;
	return m_pCurrentMusic->openFromFile(p_strFile);
}

void HeatStroke::AudioPlayer::PlayMusic()
{
	if (m_pCurrentMusic != nullptr)
	{
		m_pCurrentMusic->setLoop(true);
		m_pCurrentMusic->play();
	}
}

void HeatStroke::AudioPlayer::PauseMusic()
{
	if (m_pCurrentMusic != nullptr)
	{
		m_pCurrentMusic->pause();
	}
}

void HeatStroke::AudioPlayer::StopMusic()
{
	if (m_pCurrentMusic != nullptr)
	{
		m_pCurrentMusic->stop();
	}
}

void HeatStroke::AudioPlayer::PlaySoundEffectFromFile(const std::string& p_strFile)
{
	sf::SoundBuffer* pSoundBuffer = new sf::SoundBuffer;
	if (!(pSoundBuffer->loadFromFile(p_strFile)))
	{
		delete pSoundBuffer;
		printf("Failed to play sound: %s\n", p_strFile.c_str());
		return;
	}

	sf::Sound* pSound = new sf::Sound;
	pSound->setBuffer(*pSoundBuffer);

	m_vSoundEffects.push_back(std::pair<sf::SoundBuffer*, sf::Sound*>(pSoundBuffer, pSound));
	pSound->play();
}