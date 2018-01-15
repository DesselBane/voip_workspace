/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "receiver.h"
#include "RtpPackageProvider.h"
#include <iostream>

using namespace std;

Receiver::Receiver()
{
	isReceivingMutex_ = new mutex();
}

Receiver::~Receiver()
{
	if (IsReceiving())
		Stop();

	if(isReceivingMutex_ != nullptr)
	{
		delete isReceivingMutex_;
		isReceivingMutex_ = nullptr;
	}
}

void Receiver::Start(util::Ipv4SocketAddress const * listenAddress, util::Ipv4SocketAddress* receiveFromAddress)
{
	if (IsReceiving())
		return;

	lock_guard<mutex> isReceivingGuard(*isReceivingMutex_);
	isReceiving_ = true;

	receiveAddress_ = receiveFromAddress;

	dataQueue_ = new queue<vector<uint8_t>*>();
	queueEditMutex_ = new mutex();
	consumerCondition_ = new condition_variable();

	socket_ = new util::UdpSocket();
	socket_->open();
	socket_->bind(*listenAddress);

	self_ = std::thread([&] { ReceiveLoop(); });
}

void Receiver::Stop()
{
	if(!IsReceiving())
		return;

	lock_guard<mutex> isReceivingGuard(*isReceivingMutex_);

	isReceiving_ = false;

	socket_->close();
	self_.join();
	delete socket_;
	delete receiveAddress_;

	if(consumerCondition_ != nullptr)
	{
		delete consumerCondition_;
		consumerCondition_ = nullptr;
	}

	if(queueEditMutex_ != nullptr)
	{
		delete queueEditMutex_;
		queueEditMutex_ = nullptr;
	}

	if(dataQueue_ != nullptr)
	{
		delete dataQueue_;
		dataQueue_ = nullptr;
	}
}

bool Receiver::IsReceiving() const
{
	lock_guard<mutex> isReceivingGuard(*isReceivingMutex_);
	return isReceiving_;
}

vector<uint8_t>* Receiver::GetNextDataPackage()
{
	if(!IsReceiving())
		return nullptr;

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);
		if(!dataQueue_->empty())
		{
			auto data = dataQueue_->front();
			dataQueue_->pop();
			return data;
		}
	}

	mutex mtx;
	unique_lock<mutex> lock(mtx);

	consumerCondition_->wait(lock);
	return GetNextDataPackage();
}

void Receiver::ReceiveLoop()
{
	while (IsReceiving())
	{
		auto inputBuffer = new vector<uint8_t>(4096, 0); //TODO receive byte length dependes on mode etc
		socket_->recvfrom(*receiveAddress_, *inputBuffer, 4096);

		{
			lock_guard<mutex> editGuard(*queueEditMutex_);
			dataQueue_->push(inputBuffer);
		}

		consumerCondition_->notify_all();
	}
}
