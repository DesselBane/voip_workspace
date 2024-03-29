/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_SENDER_H
#define VOIP_SENDER_H
#include <vector>
#include <socket.h>
#include <mutex>
#include "RtpPackageProvider.h"
#include "NetworkOptions.h"

using namespace std;

class Sender
{
public:
	Sender(RtpPackageProvider* provider, NetworkOptions* networkOptions);
	~Sender();

	void StartSending();
	void StopSending();
	bool IsSending();

private:
	void Send(const vector<uint8_t>& data);
	void SendLoop();

	bool isSending_ = false;
	mutex* isSendingMutex_ = nullptr;
	util::UdpSocket* socket_ = nullptr;
	thread* workerThread_ = nullptr;
	RtpPackageProvider* provier_ = nullptr;
	NetworkOptions* networkOptions_;
};

#endif /* VOIP_SENDER_H */
