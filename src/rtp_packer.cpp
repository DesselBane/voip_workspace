/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "rtp_packer.h"

RtpPacker::RtpPacker(AudioBufferProvider* provider)
{
	provider_ = provider;
	isPackingMutex_ = new mutex();
}

RtpPacker::~RtpPacker()
{
	if (IsPacking())
		StopPacking();

	provider_ = nullptr;

	if(isPackingMutex_ != nullptr)
	{
		delete isPackingMutex_;
		isPackingMutex_ = nullptr;
	}
}

void RtpPacker::StartPacking()
{
	if (IsPacking())
		return;

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

	{
		lock_guard<mutex> isPackingGuard(*isPackingMutex_);
		isPacking_ = false;

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
	}

	workerThread_->join();
	delete workerThread_;
	workerThread_ = nullptr;
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
	queueConsumerCondition_->wait(lock);
	
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

		delete buffer; //TODO check if this is necessary
	}
}

RtpPackage* RtpPacker::ConfigurePackage(util::AudioBuffer const* buffer)
{
	auto pkg = new RtpPackage();
	const auto payload = new vector<uint8_t>(buffer->data(), buffer->data() + buffer->size());

	pkg->set_version(2)
		->set_use_extension_headers(false)
		->set_marker_bit(false)
		->set_payload_type(0)		//TODO add options for this
		->set_sequence_number(0)
		->set_timestamp(0)
		->set_synchronization_source_identifier(0)
		->set_payload(payload);

	pkg->Build();

	delete payload;
	return pkg;
}
