/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "RtpPacker.h"
#include <iostream>

RtpPacker::RtpPacker(AudioBufferProvider* provider, RtpOptions* options)
{
	provider_ = provider;
	isPackingMutex_ = new mutex();
	rtpOptions_ = options;
}

RtpPacker::~RtpPacker()
{
	if (IsPacking())
		StopPacking();

	provider_ = nullptr;

	if (isPackingMutex_ != nullptr)
	{
		delete isPackingMutex_;
		isPackingMutex_ = nullptr;
	}
}

void RtpPacker::StartPacking()
{
	if (IsPacking())
		return;

	cout << "Starting to pack" << endl;

	lock_guard<mutex> isPackingGuard(*isPackingMutex_);
	isPacking_ = true;

	queueEditMutex_ = new mutex();
	queueConsumerCondition_ = new condition_variable();
	packageQueue_ = new queue<RtpPackage const*>();

	workerThread_ = new thread([&] { PackerLoop(); });
}

void RtpPacker::StopPacking()
{
	if (!IsPacking())
		return;

	cout << "Stopping to pack";

	{
		lock_guard<mutex> isPackingGuard(*isPackingMutex_);
		isPacking_ = false;
	}

	workerThread_->join();
	delete workerThread_;
	workerThread_ = nullptr;

	{
		lock_guard<mutex> queueEditGuard(*queueEditMutex_);
		if (packageQueue_ != nullptr)
		{
			delete packageQueue_;
			packageQueue_ = nullptr;
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

	cout << "   ...done" << endl;
}

bool RtpPacker::IsPacking()
{
	lock_guard<mutex> isPackingGuard(*isPackingMutex_);
	return isPacking_;
}

RtpPackage const* RtpPacker::GetNextRtpPackage()
{
	if (!IsPacking())
		return nullptr;

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);

		if (!packageQueue_->empty())
		{
			auto pkg = packageQueue_->front();
			packageQueue_->pop();
			return pkg;
		}
	}

	mutex mtx;
	unique_lock<mutex> lock(mtx);
	queueConsumerCondition_->wait_for(lock, 100ms);

	return GetNextRtpPackage();
}

void RtpPacker::PackerLoop()
{
	while (IsPacking())
	{
		auto buffer = provider_->GetNextAudioBuffer();
		auto pkg = ConfigurePackage(buffer);

		{
			lock_guard<mutex> queueEditGuard(*queueEditMutex_);
			packageQueue_->push(pkg);
		}

		queueConsumerCondition_->notify_all();

		//Do not delete this !!!
		//delete buffer; 
	}
}

RtpPackage* RtpPacker::ConfigurePackage(util::AudioBuffer const* buffer)
{
	auto pkg = new RtpPackage();
	const auto payload = new vector<uint8_t>(buffer->data(), buffer->data() + buffer->size());

	pkg->set_version(2)
	   ->set_use_extension_headers(rtpOptions_->GetUseExtension())
	   ->set_marker_bit(false)
	   ->set_payload_type(rtpOptions_->GetPayloadType())
	   ->set_sequence_number(rtpOptions_->GetNextSequenceNumber())
	   ->set_timestamp(rtpOptions_->GetNextTimestamp())
	   ->set_synchronization_source_identifier(rtpOptions_->GetSynchronizationSourceIdentifier())
	   ->set_payload(payload);

	pkg->Build();

	delete payload;
	return pkg;
}
