/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RECEIVER_H
#define VOIP_RECEIVER_H

#include <thread>
#include <socket.h>
#include "NetworkDataProvider.h"
#include <mutex>
#include <queue>
#include "NetworkOptions.h"

class Receiver : public NetworkDataProvider
{
public:
	Receiver(NetworkOptions* networkOptions);
	~Receiver();

	void Start();
	void Stop();
	bool IsReceiving() const;
	vector<uint8_t>* GetNextDataPackage() override;

private:
	void ReceiveLoop();

	bool isReceiving_ = false;
	thread self_;
	util::UdpSocket* socket_ = nullptr;
	mutex* isReceivingMutex_ = nullptr;
	mutex* queueEditMutex_ = nullptr;
	condition_variable* consumerCondition_ = nullptr;
	queue<vector<uint8_t>*>* dataQueue_ = nullptr;
	NetworkOptions* networkOptions_ = nullptr;
};

#endif /* VOIP_RECEIVER_H */
