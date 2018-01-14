#pragma once
#include <audiobuffer.h>

class AudioBufferProvider
{
public:
	virtual util::AudioBuffer const* GetNextAudioBuffer() = 0;

protected:
	AudioBufferProvider();
	virtual ~AudioBufferProvider();
};
