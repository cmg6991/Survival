#pragma once
#include "Singleton.h"

enum class SoundType
{
	BGM,
	SFX
};

class SoundManager : public Singleton<SoundManager>
{
public:

private:
	SoundManager(){}
	~SoundManager(){}

	friend class Singleton<SoundManager>;
};

