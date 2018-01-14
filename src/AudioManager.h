#pragma once
#include <soundcard.h>
#include <queue>
#include <mutex>
#include "AudioBufferProvider.h"

using namespace std;




class AudioManager : public util::AudioIO, public AudioBufferProvider
{
public:
	explicit AudioManager();
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
	mutex* queueConsumerMutex_ = nullptr;
	lock_guard<mutex>* consumerGuard_ = nullptr;
	mutex* isRecordingMutex_ = nullptr;

};
