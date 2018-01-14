#include "AudioManager.h"


AudioManager::AudioManager()
{
	isRecordingMutex_ = new mutex();
}

AudioManager::~AudioManager()
{
	if (IsRecording())
		StopRecording();

	if(isRecordingMutex_ != nullptr)
	{
		delete isRecordingMutex_;
		isRecordingMutex_ = nullptr;
	}
}

const util::AudioBuffer* AudioManager::GetNextAudioBuffer()
{
	if(!IsRecording())
		return nullptr;

	{
		lock_guard<mutex> consumerGuard(*queueConsumerMutex_);
	}
	{
		lock_guard<mutex> editGuard(*queueEditMutex_);

		if(!audioBufferQueue_->empty())
		{
			auto retVal = audioBufferQueue_->front();
			audioBufferQueue_->pop();
			return retVal;
		}
	}

	consumerGuard_ = new lock_guard<mutex>(*queueConsumerMutex_);
	return GetNextAudioBuffer();
}

void AudioManager::StartRecording()
{
	if(IsRecording())
		return;

	lock_guard<mutex> recordingGuard(*isRecordingMutex_);
	isRecording_ = true;

	audioBufferQueue_ = new queue<util::AudioBuffer const*>();
	queueEditMutex_ = new mutex();
	queueConsumerMutex_ = new mutex();

}

void AudioManager::StopRecording()
{
	lock_guard<mutex> recordingGuard(*isRecordingMutex_);
	isRecording_ = false;

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);
		if (audioBufferQueue_ != nullptr)
		{
			delete audioBufferQueue_;
			audioBufferQueue_ = nullptr;
		}
	}

	if(queueEditMutex_ != nullptr)
	{
		delete queueEditMutex_;
		queueEditMutex_ = nullptr;
	}

	if (consumerGuard_ != nullptr)
	{
		delete consumerGuard_;
		consumerGuard_ = nullptr;
	}

	if(queueConsumerMutex_ != nullptr)
	{
		delete queueConsumerMutex_;
		queueConsumerMutex_ = nullptr;
	}
}

bool AudioManager::IsRecording()
{
	lock_guard<mutex> recordingGuard(*isRecordingMutex_);
	return isRecording_;
}

int AudioManager::process(util::AudioBuffer& output, util::AudioBuffer const& input)
{
	if (!IsRecording())
		return -1;

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);
		audioBufferQueue_->push(&input);
	}
	if(consumerGuard_ != nullptr)
	{
		delete consumerGuard_;
		consumerGuard_ = nullptr;
	}
	
	//TODO receive and play audio
}
