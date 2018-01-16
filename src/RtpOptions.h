#pragma once
#include <cstdint>
#include "AudioOptions.h"
#include <mutex>

using namespace std;

class RtpOptions
{
public:
	explicit RtpOptions(AudioOptions* audioOptions,uint8_t payloadType, bool useExtension = false,uint8_t version = 2);
	virtual ~RtpOptions();

	uint32_t GetSynchronizationSourceIdentifier();
	uint16_t GetNextSequenceNumber();
	uint32_t GetNextTimestamp();
	uint8_t GetPayloadType();
	bool GetUseExtension();
	uint8_t GetVersion();

private:
	uint32_t synchronizationSourceIdentifier_;
	uint64_t currentTimestamp_;
	uint8_t payloadType_;
	bool useExtension_;
	uint32_t sequenceNumber_;
	uint8_t version_;
	uint32_t timestampIncrease_;
	
	AudioOptions* audioOptions_ = nullptr;
	mutex* timestampMutex_ = nullptr;
	mutex* sequenceNumberMutex_ = nullptr;
};
