#include "SoundManager.h"
#include "pch.h"

SoundManager::SoundManager()
    : m_system(nullptr)
    , m_bgmGroup(nullptr)
    , m_sfxGroup(nullptr)
    , m_bgmChannel(nullptr)
    , m_sfxIndex(0)
    , m_bgmVolume(1.0f)
    , m_sfxVolume(1.0f)
    , m_initialized(false)
{
    for (auto& channel : m_sfxChannels)
    {
        channel = nullptr;
    }
}

SoundManager::~SoundManager()
{
    Release();
}

void SoundManager::Init()
{
    if (m_initialized)
        return;

    // FMOD System 생성
    FMOD_RESULT result =FMOD::System_Create(&m_system);

    if (result != FMOD_OK)
    {
        OutputDebugStringA("FMOD System_Create 실패\n");
        return;
    }

    // FMOD 초기화
    result =m_system->init(64,FMOD_INIT_NORMAL,nullptr);

    if (result != FMOD_OK)
    {
        OutputDebugStringA("FMOD Init 실패\n");
        m_system->release();
        m_system = nullptr;

        return;
    }

    // BGM 그룹
    m_system->createChannelGroup("BGM",&m_bgmGroup);

    // SFX 그룹
    m_system->createChannelGroup("SFX",&m_sfxGroup);

    m_bgmChannel = nullptr;

    for (auto& channel : m_sfxChannels)
    {
        channel = nullptr;
    }

    m_sfxIndex = 0;
    m_initialized = true;

    OutputDebugStringA("SoundManager Init 완료");
}

void SoundManager::Update()
{
    if (!m_initialized)
        return;

    if (m_system == nullptr)
        return;

    m_system->update();
}

void SoundManager::Release()
{
    OutputDebugStringA("========== SoundManager::Release START ==========\n");
    if (m_system == nullptr)
        return;

    // 모든 사운드 정지
    StopBGM();
    StopAllSFX();

    // 모든 Sound 해제
    for (auto& pair : m_sounds)
    {
        if (pair.second != nullptr)
        {
            pair.second->release();
            pair.second = nullptr;
        }
    }

    m_sounds.clear();

    // ChannelGroup은 System이 관리
    m_bgmGroup = nullptr;
    m_sfxGroup = nullptr;

    // FMOD 종료
    m_system->close();
    m_system->release();
    m_system = nullptr;
    m_initialized = false;
}

FMOD::Sound* SoundManager::FindSound(const std::string& key)
{
    auto it = m_sounds.find(key);

    if (it == m_sounds.end())
        return nullptr;

    return it->second;
}

bool SoundManager::LoadSound(const std::string& key, const std::string& path, bool loop)
{
    if (!m_initialized)
        return false;

    if (key.empty() || path.empty())
        return false;

    // 이미 존재하면 제거
    auto it = m_sounds.find(key);

    if (it != m_sounds.end())
    {
        if (it->second != nullptr)
        {
            it->second->release();
        }

        m_sounds.erase(it);
    }

    FMOD::Sound* sound = nullptr;

    FMOD_RESULT result =
        m_system->createSound(
            path.c_str(),
            FMOD_DEFAULT,
            nullptr,
            &sound
        );

    if (result != FMOD_OK)
    {
        OutputDebugStringA(("Sound Load 실패 : " + path + "\n").c_str());
        return false;
    }

    if (loop)
    {
        sound->setMode(FMOD_LOOP_NORMAL);
    }
    else
    {
        sound->setMode(FMOD_LOOP_OFF);
    }

    m_sounds[key] = sound;
    return true;
}

void SoundManager::PlayBGM(const std::string& key, float volume)
{
    if (!m_initialized)
        return;

    FMOD::Sound* sound =FindSound(key);

    if (sound == nullptr)
    {
        OutputDebugStringA(("BGM 없음 : " + key).c_str());

        return;
    }

    // 기존 BGM 정지
    StopBGM();

    volume =std::clamp(volume,0.0f,1.0f);

    m_system->playSound(sound,m_bgmGroup,true,&m_bgmChannel);

    if (m_bgmChannel == nullptr)
        return;

    m_bgmChannel->setVolume(volume);
    m_bgmChannel->setPaused(false);
}

void SoundManager::PlaySFX(const std::string& key, float volume)
{
    if (!m_initialized)
        return;

    FMOD::Sound* sound =FindSound(key);

    if (sound == nullptr)
    {
        OutputDebugStringA(("SFX 없음 : " + key).c_str());
        return;
    }

    volume =std::clamp(volume,0.0f,1.0f);

    FMOD::Channel*& channel =m_sfxChannels[m_sfxIndex];

    // 다음 채널
    m_sfxIndex++;

    if (m_sfxIndex >= SFX_CHANNEL_COUNT)
    {
        m_sfxIndex = 0;
    }

    // 사운드 재생
    m_system->playSound(sound,m_sfxGroup,true,&channel);

    if (channel == nullptr)
        return;

    channel->setVolume(volume);
    channel->setPaused(false);
}

void SoundManager::StopBGM()
{
    if (m_bgmChannel != nullptr)
    {
        m_bgmChannel->stop();
        m_bgmChannel = nullptr;
    }
}

void SoundManager::StopAllSFX()
{
    for (auto& channel : m_sfxChannels)
    {
        if (channel != nullptr)
        {
            channel->stop();
            channel = nullptr;
        }
    }
}

void SoundManager::SetBGMVolume(float volume)
{
    volume =std::clamp(volume,0.0f,1.0f);

    m_bgmVolume = volume;
    if (m_bgmGroup != nullptr)
    {
        m_bgmGroup->setVolume(volume);
    }
}

void SoundManager::SetSFXVolume(float volume)
{
    volume =std::clamp(volume,0.0f,1.0f);

    m_sfxVolume = volume;

    if (m_sfxGroup != nullptr)
    {
        m_sfxGroup->setVolume(volume);
    }
}

float SoundManager::GetBGMVolume() const
{
	return m_bgmVolume;
}

float SoundManager::GetSFXVolume() const
{
	return m_sfxVolume;
}
