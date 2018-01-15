#include "AudioManager.h"
#include <iostream>


AudioManager::AudioManager(AudioBufferProvider* provider)
{
	isRecordingMutex_ = new mutex();
	provider_ = provider;
}

AudioManager::~AudioManager()
{
	if (IsRecording())
		StopRecording();

	if (isRecordingMutex_ != nullptr)
	{
		delete isRecordingMutex_;
		isRecordingMutex_ = nullptr;
	}

	provider_ = nullptr;
}

const util::AudioBuffer* AudioManager::GetNextAudioBuffer()
{
	if (!IsRecording())
		return nullptr;

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);

		if (!audioBufferQueue_->empty())
		{
			auto retVal = audioBufferQueue_->front();
			audioBufferQueue_->pop();
			return retVal;
		}
	}

	mutex mtx;
	unique_lock<mutex> lock(mtx);

	queueConsumerCondition_->wait(lock);
	return GetNextAudioBuffer();
}

void AudioManager::StartRecording()
{
	if (IsRecording())
		return;

	cout << "Starting to record" << endl;

	lock_guard<mutex> recordingGuard(*isRecordingMutex_);
	isRecording_ = true;

	audioBufferQueue_ = new queue<util::AudioBuffer const*>();
	queueEditMutex_ = new mutex();
	queueConsumerCondition_ = new condition_variable();
}

void AudioManager::StopRecording()
{
	if(!IsRecording())
		return;

	lock_guard<mutex> recordingGuard(*isRecordingMutex_);
	isRecording_ = false;

	cout << "Stopping to record";

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);
		if (audioBufferQueue_ != nullptr)
		{
			delete audioBufferQueue_;
			audioBufferQueue_ = nullptr;
		}
	}

	if (queueEditMutex_ != nullptr)
	{
		delete queueEditMutex_;
		queueEditMutex_ = nullptr;
	}

	if (queueConsumerCondition_ != nullptr)
	{
		delete queueConsumerCondition_;
		queueConsumerCondition_ = nullptr;
	}

	cout << " ...done" << endl;
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

	queueConsumerCondition_->notify_all();

	auto audio = provider_->GetNextAudioBuffer();

	if (audio != nullptr)
	{
		cout << "audio was there" << endl;
		output = util::AudioBuffer(*audio);

		delete audio;
	}

	return 0;
}
