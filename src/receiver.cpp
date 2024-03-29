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

Receiver::Receiver(NetworkOptions* networkOptions)
{
	isReceivingMutex_ = new mutex();
	networkOptions_ = networkOptions;
}

Receiver::~Receiver()
{
	if (IsReceiving())
		Stop();

	if (isReceivingMutex_ != nullptr)
	{
		delete isReceivingMutex_;
		isReceivingMutex_ = nullptr;
	}
}

void Receiver::Start()
{
	if (IsReceiving())
		return;

	cout << "Starting to receive" << endl;

	lock_guard<mutex> isReceivingGuard(*isReceivingMutex_);
	isReceiving_ = true;

	dataQueue_ = new queue<vector<uint8_t>*>();
	queueEditMutex_ = new mutex();
	consumerCondition_ = new condition_variable();

	socket_ = new util::UdpSocket();
	socket_->open();
	socket_->bind(*networkOptions_->GetSourceIp());

	self_ = std::thread([&] { ReceiveLoop(); });
}

void Receiver::Stop()
{
	if (!IsReceiving())
		return;

	cout << "Stopping to receive";

	{
		lock_guard<mutex> isReceivingGuard(*isReceivingMutex_);
		isReceiving_ = false;
	}

	socket_->close();
	self_.join();
	delete socket_;

	if (consumerCondition_ != nullptr)
	{
		delete consumerCondition_;
		consumerCondition_ = nullptr;
	}

	if (queueEditMutex_ != nullptr)
	{
		delete queueEditMutex_;
		queueEditMutex_ = nullptr;
	}

	if (dataQueue_ != nullptr)
	{
		delete dataQueue_;
		dataQueue_ = nullptr;
	}

	cout << " ...done" << endl;
}

bool Receiver::IsReceiving() const
{
	lock_guard<mutex> isReceivingGuard(*isReceivingMutex_);
	return isReceiving_;
}

vector<uint8_t>* Receiver::GetNextDataPackage()
{
	if (!IsReceiving())
		return nullptr;

	{
		lock_guard<mutex> editGuard(*queueEditMutex_);
		if (!dataQueue_->empty())
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
		auto inputBuffer = new vector<uint8_t>(4096, 0); 
		socket_->recvfrom(*networkOptions_->GetSourceIp(), *inputBuffer, 4096);

		if(inputBuffer != nullptr){
			lock_guard<mutex> editGuard(*queueEditMutex_);
			dataQueue_->push(inputBuffer);
		}

		consumerCondition_->notify_all();
	}
}
