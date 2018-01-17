#include "RtpOptions.h"

RtpOptions::RtpOptions(AudioOptions* audioOptions, uint8_t payloadType, bool useExtension, uint8_t version)
{
	audioOptions_ = audioOptions;
	payloadType_ = payloadType;
	useExtension_ = useExtension;
	version_ = version;

	currentTimestamp_ = rand() % UINT32_MAX;
	sequenceNumber_ = rand() % UINT16_MAX;

	timestampMutex_ = new mutex();
	sequenceNumberMutex_ = new mutex();

	timestampIncrease_ = audioOptions_->GetFrameSize();
}

RtpOptions::~RtpOptions()
{
	if(timestampMutex_ != nullptr)
	{
		delete timestampMutex_;
		timestampMutex_ = nullptr;
	}
	if(sequenceNumberMutex_ != nullptr)
	{
		delete sequenceNumberMutex_;
		sequenceNumberMutex_ = nullptr;
	}

	audioOptions_ = nullptr;
}

uint32_t RtpOptions::GetSynchronizationSourceIdentifier()
{
	return synchronizationSourceIdentifier_;
}

uint16_t RtpOptions::GetNextSequenceNumber()
{
	lock_guard<mutex> sequenceGuard(*sequenceNumberMutex_);
	sequenceNumber_ += 1;
	sequenceNumber_ %= UINT16_MAX;
	return sequenceNumber_;
}

uint32_t RtpOptions::GetNextTimestamp()
{
	lock_guard<mutex> timestampGuard(*timestampMutex_);
	currentTimestamp_ += timestampIncrease_;
	currentTimestamp_ %= UINT32_MAX;
	return currentTimestamp_;
}

uint8_t RtpOptions::GetPayloadType()
{
	return payloadType_;
}

bool RtpOptions::GetUseExtension()
{
	return useExtension_;
}

uint8_t RtpOptions::GetVersion()
{
	return version_;
}
