#include "AudioOptions.h"
#include "../ext/rtaudio/RtAudio.h"

AudioOptions::AudioOptions(int inputDevice,
                           int outputDevice,
                           uint32_t frameSize,
                           uint32_t sampleRate,
                           util::AudioBuffer::SampleFormat format)
{
	RtAudio audio;
	
	if (inputDevice < 0)
		inputDevice = audio.getDefaultInputDevice();
	if (outputDevice < 0)
		outputDevice = audio.getDefaultOutputDevice();

	inputDevice_ = inputDevice;
	outputDevice_ = outputDevice;
	frameSize_ = frameSize;
	sampleRate_ = sampleRate;
	format_ = format;

	outputChannels_ = audio.getDeviceInfo(outputDevice_).outputChannels;
}

uint32_t AudioOptions::GetFrameSize() const
{
	return frameSize_;
}

uint32_t AudioOptions::GetInputChannels() const
{
	return inputChannels_;
}

uint32_t AudioOptions::GetOutputChannels() const
{
	return outputChannels_;
}

uint32_t AudioOptions::GetSampleRate() const
{
	return sampleRate_;
}

util::AudioBuffer::SampleFormat AudioOptions::GetFormat() const
{
	return format_;
}

int AudioOptions::GetInputDevice() const
{
	return inputDevice_;
}

int AudioOptions::GetOutputDevice() const
{
	return outputDevice_;
}
