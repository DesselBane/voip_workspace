/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RTP_PACKER_H
#define VOIP_RTP_PACKER_H

#include "AudioBufferProvider.h"
#include "RtpPackageProvider.h"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "RtpOptions.h"

using namespace std;

class RtpPacker : public RtpPackageProvider
{
public:
	RtpPacker(AudioBufferProvider* provider, RtpOptions* options);
	virtual ~RtpPacker();

	void StartPacking();
	void StopPacking();
	bool IsPacking();

	RtpPackage const* GetNextRtpPackage() override;

private:
	void PackerLoop();
	RtpPackage* ConfigurePackage(util::AudioBuffer const* buffer);

	AudioBufferProvider* provider_ = nullptr;
	bool isPacking_ = false;
	thread* workerThread_ = nullptr;
	mutex* isPackingMutex_ = nullptr;
	mutex* queueEditMutex_ = nullptr;
	condition_variable* queueConsumerCondition_ = nullptr;
	queue<RtpPackage const*>* packageQueue_ = nullptr;
	RtpOptions* rtpOptions_ = nullptr;
};

#endif /* VOIP_RTP_PACKER_H */
