/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RTP_DEPACKER_H
#define VOIP_RTP_DEPACKER_H
#include "NetworkDataProvider.h"
#include <mutex>
#include <queue>
#include "AudioBufferProvider.h"

class RtpDepacker : public AudioBufferProvider
{
public:
	explicit RtpDepacker(NetworkDataProvider* provider);
	virtual ~RtpDepacker();

	void StartUnpacking();
	void StopUnpacking();
	bool IsUnpacking() const;
	util::AudioBuffer const* GetNextAudioBuffer() override;

private:
	void UnpackLoop();
	util::AudioBuffer* Unpack(vector<uint8_t> const * data);

	bool isUnpacking_ = false;
	thread* workderThread_ = nullptr;
	mutex* queueEditMutex_ = nullptr;
	mutex* isUnpackingMutex_ = nullptr;
	queue<util::AudioBuffer*>* audioQueue_ = nullptr;
	NetworkDataProvider* provider_ = nullptr;
};

#endif /* VOIP_RTP_DEPACKER_H */
