#pragma once
#include <soundcard.h>
#include <queue>
#include <mutex>

using namespace std;


class AudioBufferProvider
{
	virtual const util::AudioBuffer* getNextAudioBuffer() = 0;

protected:
	AudioBufferProvider();
	virtual ~AudioBufferProvider();
};

class AudioManager : public util::AudioIO, public AudioBufferProvider
{
	explicit AudioManager();
	virtual ~AudioManager();

	const util::AudioBuffer* getNextAudioBuffer() override;
	
	void StartRecording();
	void StopRecording();
	bool IsRecording();

	int process(util::AudioBuffer& output, util::AudioBuffer const& input) override;

private:
	queue<util::AudioBuffer const*>* audioBufferQueue_ = nullptr;
	bool isRecording_ = false;
	mutex* queueEditMutex_ = nullptr;
	mutex* queueConsumerMutex_ = nullptr;
	lock_guard<mutex>* consumerGuard = nullptr;
	mutex* isRecordingMutex_ = nullptr;

};
