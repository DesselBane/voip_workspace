/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "RtpDepacker.h"
#include "RtpPackageProvider.h"

RtpDepacker::RtpDepacker(NetworkDataProvider* provider)
{
	provider_ = provider;
	isUnpackingMutex_ = new mutex;
}

RtpDepacker::~RtpDepacker()
{
	provider_ = nullptr;

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
}

bool RtpDepacker::IsUnpacking() const
{
	lock_guard<mutex> isUnpackingGuard(*isUnpackingMutex_);
	return isUnpacking_;
}

util::AudioBuffer const* RtpDepacker::GetNextAudioBuffer()
{
	if(!IsUnpacking())
		return nullptr;

	lock_guard<mutex> editGuard(*queueEditMutex_);
	
	if(audioQueue_->empty())
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
		auto audioBuffer = Unpack(data);

		if (audioBuffer != nullptr)
		{
			lock_guard<mutex> editGuard(*queueEditMutex_);
			audioQueue_->push(audioBuffer);
		}

		delete data;
	}
}

util::AudioBuffer* RtpDepacker::Unpack(vector<uint8_t> const* data)
{
	auto pkg = RtpPackage::ParsePackage(data);

	if(!pkg->IsValid())
		return nullptr;

	auto buffer = new util::AudioBuffer(512, 8, 44100, util::AudioBuffer::FLOAT32); //TODO add options

	auto input = pkg->get_payload();
	auto inputData = reinterpret_cast<float*>(input->data());

	float* buff = reinterpret_cast<float*>(buffer->data());

	for (int i = 0; i < input->size() / 4; i++)
	{
		buff[8 * i + 7] = 0;
		buff[8 * i + 6] = 0;
		buff[8 * i + 5] = 0;
		buff[8 * i + 4] = 0;
		buff[8 * i + 3] = 0;
		buff[8 * i + 2] = 0;
		buff[8 * i + 1] = inputData[i];
		buff[8 * i] = inputData[i];
	}

	return buffer;
}
