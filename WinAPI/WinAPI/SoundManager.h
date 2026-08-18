#pragma once
#include "Singleton.h"
#include "pch.h"

#include "../core/inc/fmod.hpp"

#if defined(_WIN64)
#pragma comment(lib, "../core/lib/x64/fmod_vc.lib")
#else
#pragma comment(lib, "../core/lib/x86/fmod_vc.lib")
#endif

class SoundManager : public Singleton<SoundManager>
{
public:
	///  필수코드
	void Init();
	void Update();
	void Release();

	// 사운드 등록
	bool LoadSound(
		const std::string& key,
		const std::string& path,
		bool loop = false
	);

	// 재생
	void PlayBGM(
		const std::string& key,
		float volume = 1.0f
	);

	void PlaySFX(
		const std::string& key,
		float volume = 1.0f
	);

	// 정지
	void StopBGM();
	void StopAllSFX();

	// 볼륨
	void SetBGMVolume(float volume);
	void SetSFXVolume(float volume);

	float GetBGMVolume() const;
	float GetSFXVolume() const;


private:
	SoundManager();
	~SoundManager();

	FMOD::Sound* FindSound(const std::string& key);

private:
	FMOD::System* m_system;

	// BGM / SFX 그룹
	FMOD::ChannelGroup* m_bgmGroup;
	FMOD::ChannelGroup* m_sfxGroup;

	// 현재 BGM
	FMOD::Channel* m_bgmChannel;

	// SFX 채널
	static constexpr int SFX_CHANNEL_COUNT = 16;

	std::array<FMOD::Channel*, SFX_CHANNEL_COUNT> m_sfxChannels;

	int m_sfxIndex;

	// 사운드
	std::unordered_map<std::string, FMOD::Sound*> m_sounds;

	// 볼륨
	float m_bgmVolume;
	float m_sfxVolume;

	bool m_initialized;


	friend class Singleton<SoundManager>;
};

