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

bool HeatStroke::AudioPlayer::OpenMusicFromFile(const std::string& p_strFile)
{
	if (m_pCurrentMusic != nullptr)
	{
		delete m_pCurrentMusic;
	}

	m_strCurrentMusicFile = p_strFile;
	m_pCurrentMusic = new sf::Music;
	m_pCurrentMusic->setVolume(m_fMusicVolume);
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

void HeatStroke::AudioPlayer::PreloadSoundEffects(const std::string& p_strPreloadFileName)
{
	tinyxml2::XMLDocument mPreloadDoc;
	assert(mPreloadDoc.LoadFile(p_strPreloadFileName.c_str()) == tinyxml2::XML_NO_ERROR);

	tinyxml2::XMLElement* pRootElement = mPreloadDoc.RootElement();
	assert(pRootElement != nullptr);

	tinyxml2::XMLElement* pSoundEffectsElement = pRootElement->FirstChildElement("SoundEffects");
	assert(pSoundEffectsElement != nullptr);

	for (tinyxml2::XMLElement* pSoundEffectElement = pSoundEffectsElement->FirstChildElement("SoundEffect");
		pSoundEffectElement != nullptr;
		pSoundEffectElement = pSoundEffectElement->NextSiblingElement("SoundEffect"))
	{
		std::string strSoundEffectFileName;
		EasyXML::GetRequiredStringAttribute(pSoundEffectElement, "path", strSoundEffectFileName);

#ifdef _DEBUG
		if (strSoundEffectFileName.substr(strSoundEffectFileName.length() - 5) != ".flac")
		{
			printf("AudioPlayer - %s is not in .flac format, may be inefficent\n", strSoundEffectFileName.c_str());
		}
#endif

		LoadedSoundEffects::const_iterator it = m_mLoadedSoundEffects.find(strSoundEffectFileName);
		if (it == m_mLoadedSoundEffects.end())
		{
			sf::SoundBuffer* pSoundBuffer = new sf::SoundBuffer;
			if (!(pSoundBuffer->loadFromFile(strSoundEffectFileName)))
			{
				delete pSoundBuffer;
				printf("Failed to find sound: %s\n", strSoundEffectFileName.c_str());
				return;
			}

			sf::Sound* pSound = new sf::Sound;
			pSound->setVolume(m_fSoundEffectsVolume);
			pSound->setBuffer(*pSoundBuffer);

			m_mLoadedSoundEffects[strSoundEffectFileName] = std::pair<sf::SoundBuffer*, sf::Sound*>(pSoundBuffer, pSound);
		}
		else
		{
			printf("AudioPlayer: Sound effect %s already loaded\n", strSoundEffectFileName.c_str());
		}
	}
}

void HeatStroke::AudioPlayer::PlaySoundEffect(const std::string& p_strFile, const bool loop /* = false */)
{
	LoadedSoundEffects::const_iterator it = m_mLoadedSoundEffects.find(p_strFile);
	
	if (it == m_mLoadedSoundEffects.end())
	{
#ifdef _DEBUG
		std::cout << "Sound file could not be played because it was not preloaded: " << p_strFile << "\n";
#endif
		return;
	}

	if (it->second.second->getStatus() != sf::Sound::Playing)
	{
		it->second.second->setLoop(loop);
		it->second.second->play();
	}
	else if (!loop)
	{
		it->second.second->setPlayingOffset(sf::Time());
	}
}

void HeatStroke::AudioPlayer::StopSoundEffect(const std::string& p_strFile)
{
	LoadedSoundEffects::const_iterator it = m_mLoadedSoundEffects.find(p_strFile);

	if (it == m_mLoadedSoundEffects.end())
	{
#ifdef _DEBUG
		std::cout << "Sound file could not be stopped because it was not preloaded: " << p_strFile << "\n";
#endif
		return;
	}

	it->second.second->stop();
}

void HeatStroke::AudioPlayer::FlushSoundEffects()
{
	LoadedSoundEffects::iterator it = m_mLoadedSoundEffects.begin(), end = m_mLoadedSoundEffects.end();
	for (; it != end; it++)
	{
		DELETE_IF(it->second.first);
		DELETE_IF(it->second.second);
	}

	m_mLoadedSoundEffects.clear();
}

void HeatStroke::AudioPlayer::SetMusicVolume(float p_fMusicVolume)
{
	if (p_fMusicVolume > 100.0f)
		m_fMusicVolume = 100.0f;
	else if (p_fMusicVolume < 0.0f)
		m_fMusicVolume = 0.0f;
	else
		m_fMusicVolume = p_fMusicVolume;

	if (m_pCurrentMusic != nullptr)
	{
		m_pCurrentMusic->setVolume(m_fMusicVolume);
	}
}

void HeatStroke::AudioPlayer::SetSoundEffectsVolume(float p_fSoundEffectsVolume)
{
	if (p_fSoundEffectsVolume > 100.0f)
		m_fSoundEffectsVolume = 100.0f;
	else if (p_fSoundEffectsVolume < 0.0f)
		m_fSoundEffectsVolume = 0.0f;
	else
		m_fSoundEffectsVolume = p_fSoundEffectsVolume;

	for (auto mSound : m_mLoadedSoundEffects)
	{
		mSound.second.second->setVolume(m_fSoundEffectsVolume);
	}
}