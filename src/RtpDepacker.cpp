/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "RtpDepacker.h"
#include "RtpPackageProvider.h"
#include <iostream>

RtpDepacker::RtpDepacker(NetworkDataProvider* provider, AudioOptions* audioOptions)
{
	provider_ = provider;
	isUnpackingMutex_ = new mutex;
	audioOptions_ = audioOptions;
}

RtpDepacker::~RtpDepacker()
{
	provider_ = nullptr;
	audioOptions_ = nullptr;

	if (isUnpackingMutex_ != nullptr)
	{
		delete isUnpackingMutex_;
		isUnpackingMutex_ = nullptr;
	}
}

void RtpDepacker::StartUnpacking()
{
	if (IsUnpacking())
		return;

	cout << "Starting to unpack" << endl;

	lock_guard<mutex> isUnpacking(*isUnpackingMutex_);
	isUnpacking_ = true;

	queueEditMutex_ = new mutex();
	audioQueue_ = new queue<util::AudioBuffer*>();

	workderThread_ = new thread([&] { UnpackLoop(); });
}

void RtpDepacker::StopUnpacking()
{
	if (!IsUnpacking())
		return;

	cout << "Stopping to unpack";

	{
		lock_guard<mutex> isUnpackingGuard(*isUnpackingMutex_);
		isUnpacking_ = false;
	}

	workderThread_->join();
	delete workderThread_;
	workderThread_ = nullptr;

	if (audioQueue_ != nullptr)
	{
		delete audioQueue_;
		audioQueue_ = nullptr;
	}

	if (queueEditMutex_ != nullptr)
	{
		delete queueEditMutex_;
		queueEditMutex_ = nullptr;
	}

	cout << " ...done" << endl;
}

bool RtpDepacker::IsUnpacking() const
{
	lock_guard<mutex> isUnpackingGuard(*isUnpackingMutex_);
	return isUnpacking_;
}

util::AudioBuffer const* RtpDepacker::GetNextAudioBuffer()
{
	if (!IsUnpacking())
		return nullptr;

	lock_guard<mutex> editGuard(*queueEditMutex_);

	if (audioQueue_->empty())
		return nullptr;

	auto retVal = audioQueue_->front();
	audioQueue_->pop();
	return retVal;
}

void RtpDepacker::UnpackLoop()
{
	while (IsUnpacking())
	{
		auto data = provider_->GetNextDataPackage();

		if (data != nullptr)
		{
			auto audioBuffer = Unpack(data);

			if (audioBuffer != nullptr)
			{
				lock_guard<mutex> editGuard(*queueEditMutex_);
				audioQueue_->push(audioBuffer);
			}

			delete data;
		}
	}
}

util::AudioBuffer* RtpDepacker::Unpack(vector<uint8_t> const* data)
{
	auto pkg = RtpPackage::ParsePackage(data);

	if (!pkg->IsValid())
		return nullptr;

	auto buffer = new util::AudioBuffer(audioOptions_->GetFrameSize(),
	                                    audioOptions_->GetOutputChannels(),
	                                    audioOptions_->GetSampleRate(),
	                                    audioOptions_->GetFormat());

	auto input = pkg->get_payload();
	const auto inputData = reinterpret_cast<float*>(input->data());

	auto buff = reinterpret_cast<float*>(buffer->data());
	const int outputChannelCount = audioOptions_->GetOutputChannels();

	for (int i = 0; i < input->size() / 4; i++)
	{
		switch (outputChannelCount)
		{
		case 8:
			buff[outputChannelCount * i + 7] = 0;

		case 7:
			buff[outputChannelCount * i + 6] = 0;

		case 6:
			buff[outputChannelCount * i + 5] = 0;

		case 5:
			buff[outputChannelCount * i + 4] = 0;

		case 4:
			buff[outputChannelCount * i + 3] = 0;

		case 3:
			buff[outputChannelCount * i + 2] = 0;

		case 2:
			buff[outputChannelCount * i + 1] = inputData[i];
		case 1:
			buff[outputChannelCount * i] = inputData[i];
			break;
		default:
			throw "Invalid channel configuration";
		}
	}

	delete pkg;
	return buffer;
}
