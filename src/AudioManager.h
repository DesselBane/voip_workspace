#pragma once
#include <soundcard.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "AudioBufferProvider.h"

using namespace std;

class AudioManager : public util::AudioIO, public AudioBufferProvider
{
public:
	explicit AudioManager(AudioBufferProvider* provider);
	virtual ~AudioManager();

	const util::AudioBuffer* GetNextAudioBuffer() override;

	void StartRecording();
	void StopRecording();
	bool IsRecording();

	int process(util::AudioBuffer& output, util::AudioBuffer const& input) override;

private:
	queue<util::AudioBuffer const*>* audioBufferQueue_ = nullptr;
	bool isRecording_ = false;
	mutex* queueEditMutex_ = nullptr;
	condition_variable* queueConsumerCondition_ = nullptr;
	mutex* isRecordingMutex_ = nullptr;
	AudioBufferProvider* provider_;
};
