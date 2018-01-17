#pragma once

#include <cstdint>
#include "audiobuffer.h"


class AudioOptions
{
public:
	explicit AudioOptions(int inputDevice = -1,
	                      int outputDevice = -1,
	                      uint32_t frameSize = 512,
	                      uint32_t sampleRate = 44100,
	                      util::AudioBuffer::SampleFormat format = util::AudioBuffer::INT16);

	virtual ~AudioOptions(){}

	uint32_t GetFrameSize() const;
	uint32_t GetInputChannels() const;
	uint32_t GetOutputChannels() const;
	uint32_t GetSampleRate() const;
	util::AudioBuffer::SampleFormat GetFormat() const;
	int GetInputDevice() const;
	int GetOutputDevice() const;

private:
	uint32_t frameSize_;
	uint32_t inputChannels_ = 1; //Can only send Mono
	uint32_t outputChannels_;
	uint32_t sampleRate_;
	util::AudioBuffer::SampleFormat format_;
	int inputDevice_ = -1;
	int outputDevice_ = -1;
};
